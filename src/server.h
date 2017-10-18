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

#include "_utils.h"
#include "config.h"
#include "datastore.h"

/** @brief UPNP Server implementation. */
namespace upnp {

/** @brief The UPNP Command struct. */
struct UpnpCommand {
    enum TYPE{ NONE, BROWSE, GET_PROTOCOL_INFO, SORT_CAPABILITIES, SEARCH_CAPABILITIES, SYSTEM_UPDATE_ID,
               CURRENT_CONNECTION_IDS, CURRENT_CONNECTION_INFO };
    TYPE type = NONE;
    std::map< std::string, std::string > values;
    static TYPE parse( const std::string& command );
    static std::string str( TYPE t ) {
        switch(t) {
            case BROWSE: return BROWSE_FLAG;
            case GET_PROTOCOL_INFO: return "GetProtocolInfo";
            case SORT_CAPABILITIES: return "GetSortCapabilities";
            case SEARCH_CAPABILITIES: return "GetSearchCapabilities";
            case SYSTEM_UPDATE_ID: return "GetSystemUpdateID";
            case CURRENT_CONNECTION_IDS: return "GetCurrentConnectionIDs";
            case CURRENT_CONNECTION_INFO: return "GetCurrentConnectionInfo";
            default: return "(none)";
        }
    }

    template< class T >
    inline bool contains( T& value ) {
        return( values.find( value ) != values.end() );
    }

    template< class T >
    inline std::string get( T& key ) {
        return( values[ key ] );
    }
    /** output as string. */
    friend std::ostream& operator<< ( std::ostream& out, const UpnpCommand & c ) {
            out << "{\"type\":\"" << UpnpCommand::str( c.type ) << "\"";
            for( auto& v : c.values ) {
                out << ", \"" << v.first << "\": \"" << v.second << "\"";

            }
            out << "}";
            return out;
    }
    std::string str() {
        std::stringstream _stream;
        _stream << *this;
        return _stream.str();
    }

};

/** @brief The UPNP Server class. */
class Server {
public:
    /** @brief The CDS Server CTOR. */
    Server(const std::string& redis, /** @param redis redis host */
           const short port /** @param port the redis port. */ );

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
    data::redis_ptr redis_;
    redox::Subscriber sub_;
    config_t config_;
};

}//namespace upnp
#endif // SERVER_H
