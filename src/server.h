#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <memory>
#include <string>

#include "rapidxml_ns.hpp"
#include "rapidxml_ns_print.hpp"
#include "rapidxml_ns_utils.hpp"

#include "http/server.h"
#include "http/httpserver.h"

#include "config.h"
#include "datastore.h"

/** @brief UPNP Server implementation. */
namespace upnp {

/** @brief The UPNP Command struct. */
struct UpnpCommand {
    enum TYPE{ NONE, BROWSE, GET_PROTOCOL_INFO };
    TYPE type = NONE;
    std::map< std::string, std::string > values;
    static TYPE parse( const std::string& command );
};

template< class T >
inline T* element( rapidxml_ns::xml_document<>* doc, T* parent, const std::string& name, const std::string& value ) {
    rapidxml_ns::xml_node<> *_n = doc->allocate_node(rapidxml_ns::node_element,
                                                     doc->allocate_string(name.c_str()),
                                                     doc->allocate_string(value.c_str()) );
    parent->append_node(_n);
    return _n;
}
template< class T >
inline void attr( rapidxml_ns::xml_document<>* doc, T* parent, const std::string& name, const std::string& value ) {
    rapidxml_ns::xml_attribute<> *_attr = doc->allocate_attribute( doc->allocate_string(name.c_str()), doc->allocate_string(value.c_str()) );
    parent->append_attribute(_attr);
}

/** @brief The UPNP Server class. */
class Server {
public:
    /** @brief The CDS Server CTOR. */
    Server(const std::string& redis, /** @param redis redis host */
           const short port /** @param port the redis port. */ )

        : redis_( data::make_connection( redis, port ) ),
          config_( json( data::config( redis_ ) ) ) {}

    static UpnpCommand parse( const std::string& body );

    http::http_status config( http::Request& request, http::Response& response ) {
        response << data::config( redis_ );
        response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::JSON ) );
        response.parameter( "Access-Control-Allow-Origin", "*" );
        return http::http_status::OK;
    }

    http::http_status description( http::Request& request, http::Response& response );
    http::http_status cds( http::Request& request, http::Response& response );
    http::http_status cms( http::Request& request, http::Response& response );

private:
    data::redox_ptr redis_;
    config_t config_;
};

}//namespace upnp
#endif // SERVER_H
