#include <iostream>
#include <string>
#include <vector>

#include "cxxopts.hpp"
#include <boost/filesystem.hpp>

#include "http/mod/exec.h"
#include "http/mod/file.h"
#include "http/mod/match.h"
#include "http/mod/method.h"

#include "src/server.h"

using namespace std::placeholders;

void signalHandler( int signum ) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    exit( signum );
}

int main( int argc, char* argv[] ) {
    //parse args
    cxxopts::Options options("squawk upnp cli", "Command line interface for the squawk upnp media server.");
    options.add_options()
        ( "http-ip", "API Webserver IP-Adress to bind to.", cxxopts::value<std::string>(), "IP" )
        ( "http-port", "API Webserver IP Port to bind to.", cxxopts::value<std::string>(), "PORT" )
        ( "redis", "Redis Database (default: localhost)", cxxopts::value<std::string>()->default_value("localhost"), "HOST" )
        ( "help", "Print help")
      ;
    options.parse(argc, argv);

    if( options.count( "help" ) ) {
         std::cout << options.help({"", "Group"}) << std::endl;
         exit(0);
    }

    auto& _redis_server = options["redis"].as<std::string>();

    std::string _ip = "localhost", _port = "8080";
    if ( options.count( "http-ip" ) )
    { _ip = options["http-ip"].as<std::string>(); }
    if ( options.count( "http-port" ) )
    { _port = options["http-port"].as<std::string>(); }

    //start server
    std::cout << "Start squawk upnp server." << std::endl;

    /** Setup and start the HTTP Server **/
    auto _web_server = std::shared_ptr< http::Server< http::HttpServer > >( new http::Server< http::HttpServer >( _ip, _port ) );
    upnp::Server _server( _web_server, _redis_server, 6379 );

    // register signal SIGINT and signal handler
    signal(SIGINT, signalHandler);

    while(1)
    { sleep(1); }

    //wait
    return 0;
}
