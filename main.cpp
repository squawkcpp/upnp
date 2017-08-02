#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "cxxopts.hpp"
#include "spdlog/spdlog.h"
#include <boost/filesystem.hpp>

#include "http/mod/exec.h"
#include "http/mod/file.h"
#include "http/mod/match.h"
#include "http/mod/method.h"

#include "src/config.h"
#include "src/datastore.h"
#include "src/server.h"
#include "src/ssdpserver.h"
#include "src/_utils.h"

using namespace std::placeholders;

static const char* PARAM_LISTEN_ADDRESS = "listen";
static const char* PARAM_MULTICAST_ADDRESS = "multicast";
static const char* PARAM_MULTICAST_PORT = "multicast-port";
static const char* PARAM_HTTP_PORT = "http-port";
static const char* PARAM_NAME = "name";
static const char* PARAM_CDS_URI = "cds";
static const char* PARAM_REDIS = "redis";

struct Container {
    upnp::config_t config;
    data::redox_ptr redox;
    std::shared_ptr< upnp::Server > server;
    std::shared_ptr< http::Server< http::HttpServer > > www;
    std::shared_ptr< upnp::SSDPServerImpl > ssdp;
};

void signalHandler( int signum ) {
    std::cout << "UPNP::Interrupt signal (" << signum << ") received.\n";
    exit( signum );
}

int main( int argc, char* argv[] ) {
    //parse args
    cxxopts::Options options("squawk upnp cli", "Command line interface for the squawk upnp media server.");
    options.add_options()
        ( PARAM_LISTEN_ADDRESS, "Listen for SSDP multicast messages in the network.", cxxopts::value<std::string>(), "IP" )
        ( PARAM_HTTP_PORT, "API Webserver IP port to bind to.", cxxopts::value<std::string>(), "PORT" )
        ( PARAM_MULTICAST_ADDRESS, "SSDP multicast IP-Adress to bind to.", cxxopts::value<std::string>(), "IP" )
        ( PARAM_MULTICAST_PORT, "SSDP multicast port to bind to.", cxxopts::value<std::string>(), "PORT" )
        ( PARAM_NAME, "Server display name (default: empty)).", cxxopts::value<std::string>(), "UUID" )
        ( PARAM_CDS_URI, "CDS uri.", cxxopts::value<std::string>(), "URI" )
        ( PARAM_REDIS, "Redis Database (default: localhost)", cxxopts::value<std::string>()->default_value("localhost"), "HOST" )
        ( "help", "Print help")
      ;
    options.parse(argc, argv);

    if( options.count( "help" ) ) {
         std::cout << options.help({"", "Group"}) << std::endl;
         exit(0);
    }

    Container _container;

    auto& _redis_server = options[PARAM_REDIS].as<std::string>();
    _container.redox = data::make_connection( _redis_server, 6379 );
    //load config from database
    if( data::config_exists( _container.redox ) ) {
        _container.config = upnp::json( data::config( _container.redox ) );
    }

    if ( options.count( PARAM_HTTP_PORT ) )
    { _container.config->http_port = options[PARAM_HTTP_PORT].as<std::string>(); }
    if ( options.count( PARAM_MULTICAST_ADDRESS ) )
    { _container.config->multicast_address = options[PARAM_MULTICAST_ADDRESS].as<std::string>(); }
    if ( options.count( PARAM_MULTICAST_PORT ) )
    { _container.config->multicast_port = options[PARAM_MULTICAST_PORT].as<std::string>(); }
    if ( options.count( PARAM_LISTEN_ADDRESS ) )
    { _container.config->listen_address = options[PARAM_LISTEN_ADDRESS].as<std::string>(); }
    if ( options.count( PARAM_NAME ) )
    { _container.config->name = options[PARAM_NAME].as<std::string>(); }
    if ( options.count( PARAM_CDS_URI ) )
    { _container.config->cds_uri = options[PARAM_CDS_URI].as<std::string>(); }

    auto device_uri_ = fmt::format( "http://{}:{}/root.xml", _container.config->listen_address, _container.config->http_port );
    std::map< std::string, std::string > _namespaces ( {
        { _container.config->uuid, device_uri_ },
        { upnp::NS_ROOT_DEVICE, device_uri_ },
        { upnp::NS_MEDIASERVER, device_uri_ },
        { upnp::NS_CONTENT_DIRECTORY, device_uri_ }
    });

    //store configuration
    if( !upnp::validate( _container.config ) ) return -1;
    data::config( _container.redox, upnp::json( _container.config ) );

    /* setup logger */
    spdlog::set_level(spdlog::level::trace);
    auto console = spdlog::stdout_color_mt(upnp::LOGGER);
    console->info( "Start UPNP server ({}:{}/{}:{})", _container.config->listen_address, _container.config->http_port,
                  _container.config->multicast_address, _container.config->multicast_port );

    /* Setup and start the server **/
    _container.server = std::make_shared< upnp::Server >( _redis_server, 6379 );

    _container.www = std::shared_ptr< http::Server< http::HttpServer > >(
        new http::Server< http::HttpServer >( _container.config->listen_address, _container.config->http_port ) );

    _container.www->bind( http::mod::Match<>( "^\\/config$" ),
        http::mod::Exec( std::bind( &upnp::Server::config, _container.server, _1, _2 ) ),
        http::mod::Http() );

    _container.www->bind( http::mod::Match<>( "^\\/root.xml$" ),
        http::mod::Exec( std::bind( &upnp::Server::description, _container.server, _1, _2 ) ),
        http::mod::Http() );

    _container.www->bind( http::mod::Match<>( "^\\/cds.xml$" ),
        http::mod::Exec( [](http::Request& request, http::Response& response ) -> http::http_status {
            response << upnp::CDS_DESCRIPTION;
            response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::XML ) );
            return http::http_status::OK;
        }),
        http::mod::Http() );

    _container.www->bind( http::mod::Match<>( "^\\/cms.xml" ),
        http::mod::Exec( [](http::Request& request, http::Response& response ) -> http::http_status {
            response << upnp::CMS_DESCRIPTION;
            response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::XML ) );
            return http::http_status::OK;
        }),
        http::mod::Http() );

    _container.www->bind( http::mod::Match<>( "^/ctl/cds$" ),
        http::mod::Exec( std::bind( &upnp::Server::cds, _container.server, _1, _2 ) ),
        http::mod::Http() );

    _container.www->bind( http::mod::Match<>( "^/ctl/cms$" ),
        http::mod::Exec( std::bind( &upnp::Server::cms, _container.server, _1, _2 ) ),
        http::mod::Http() );

    //create ssdp server
    _container.ssdp = std::make_shared< upnp::SSDPServerImpl >( _container.config, _namespaces );
    _container.ssdp->announce();

    // register signal SIGINT and signal handler
    signal(SIGINT, signalHandler);

    while(1)
    { sleep(1); }

    //wait
    return 0;
}
