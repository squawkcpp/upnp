#include "server.h"

#include  <iostream>

#include "cxxopts.hpp"
#include "fmt/format.h"
#include "rapidxml_ns.hpp"
#include "spdlog/spdlog.h"

#include "http/mod/exec.h"
#include "http/mod/file.h"
#include "http/mod/match.h"
#include "http/mod/method.h"

#include "_utils.h"

namespace upnp {

Server::Server(std::shared_ptr< http::Server< http::HttpServer > > web_server, /** @param web_server the server to attach the API uri's. */
       const std::string& redis, /** @param redis redis host */
       const short port /** @param port the redis port. */ ) {

    spdlog::set_level(spdlog::level::trace);
    auto console = spdlog::stdout_color_mt(LOGGER);

    web_server->bind( http::mod::Match<>( "^/upnp$" ),
        http::mod::Exec( [this](http::Request&, http::Response& response ) -> http::http_status {
            response << "<html><head><title>Squawk UPNP Server</title></head><body><h1>Squawk UPNP Server</h1></body></html>";
            response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::HTML ) );
            return http::http_status::OK;
        }),
        http::mod::Http() );
}

std::map< std::string, std::string > Server::parse( const std::string& body ) {
    rapidxml_ns::xml_document<> doc;
    doc.parse<0>( const_cast< char* >(  body.c_str() ) );
    auto root_node = doc.first_node( "Envelope" );
    for (rapidxml_ns::xml_node<> * __r_sieblings = root_node->first_node(); __r_sieblings; __r_sieblings = __r_sieblings->next_sibling() ) {
        std::cout << "node: " << __r_siebling->name() << std::endl;
    }
}

}//namespace upnp
