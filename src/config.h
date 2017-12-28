#ifndef CONFIG_H
#define CONFIG_H

#include <arpa/inet.h>
#include <ifaddrs.h>
// #include <uuid/uuid.h>

#include <string>
#include <vector>

#include "fmt/format.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

namespace upnp {
struct Config {
    std::string listen_address, http_port, multicast_address,
                multicast_port, name, cds_uri, uuid;
};

inline std::string _get_ip() {
    std::string ip_ = "127.0.0.1";
    struct ifaddrs* ifAddrStruct = NULL;

    getifaddrs( &ifAddrStruct );
    char addressBuffer[INET_ADDRSTRLEN];
    for( struct ifaddrs* ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next ) {
        if( (ifa->ifa_addr)->sa_family == AF_INET ) {

            void* tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;

            inet_ntop( AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN );

            if( strcmp( ifa->ifa_name, "lo" ) ) {
                ip_ = addressBuffer;
            }
        }
    }
    if( ifAddrStruct != NULL ) {
        freeifaddrs( ifAddrStruct );
    }
    return ip_;
}

inline bool validate( std::shared_ptr< Config > config ) {
    if( config->http_port.empty() ) {
        config->http_port = "9003";
    } if( config->multicast_address.empty() ) {
        config->multicast_address = "239.255.255.250";
    } if(config->multicast_port.empty() ) {
        config->multicast_port = "1900";
    } if( config->listen_address.empty() ) {
        config->listen_address = _get_ip();
    } if( config->name.empty() ) {
        config->name = fmt::format( "Squawk Media Server: {}", "1.0" );
    } if( config->cds_uri.empty() ) {
        config->cds_uri = "";
    } if( config->uuid.empty() ) {
//TODO        uuid_t out;
//        uuid_generate_random((unsigned char *)&out);
//        char buffer[37];
//        uuid_unparse((unsigned char *)&out, buffer);
//        config->uuid = std::string(buffer);
    }
    return true;
}

const static char* CONFIG_HTTP_PORT = "http_port";
const static char* CONFIG_MULTICAST_ADDRESS = "multicast_address";
const static char* CONFIG_MULTICAST_PORT = "multicast_port";
const static char* CONFIG_LISTEN_ADDRESS = "listen_address";
const static char* CONFIG_DISPLAY_NAME = "name";
const static char* CONFIG_CDS_URI = "cds_uri";
const static char* CONFIG_UUID = "uuid";

inline std::string json( std::shared_ptr< Config > config ) {
    using namespace rapidjson;
    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    writer.StartObject();

    writer.String( CONFIG_HTTP_PORT );
    writer.String( config->http_port.c_str() );

    writer.String( CONFIG_MULTICAST_ADDRESS );
    writer.String( config->multicast_address.c_str() );

    writer.String( CONFIG_MULTICAST_PORT );
    writer.String( config->multicast_port.c_str() );

    writer.String( CONFIG_LISTEN_ADDRESS );
    writer.String( config->listen_address.c_str() );

    writer.String( CONFIG_DISPLAY_NAME );
    writer.String( config->name.c_str() );

    writer.String( CONFIG_CDS_URI );
    writer.String( config->cds_uri.c_str() );

    writer.String( CONFIG_UUID );
    writer.String( config->uuid.c_str() );

    writer.EndObject();
    return sb.GetString();
}
inline std::shared_ptr< Config > json( const std::string& config ) {
    using namespace rapidjson;
    Document document;
    document.Parse(config.c_str());

    auto _config = std::make_shared< Config >();

    if(document.HasMember(CONFIG_HTTP_PORT) && document[CONFIG_HTTP_PORT].IsString() )
    { _config->http_port = document[CONFIG_HTTP_PORT].GetString(); }

    if(document.HasMember(CONFIG_MULTICAST_ADDRESS) && document[CONFIG_MULTICAST_ADDRESS].IsString() )
    { _config->multicast_address = document[CONFIG_MULTICAST_ADDRESS].GetString(); }

    if(document.HasMember(CONFIG_MULTICAST_PORT) && document[CONFIG_MULTICAST_PORT].IsString() )
    { _config->multicast_port = document[CONFIG_MULTICAST_PORT].GetString(); }

    if(document.HasMember(CONFIG_LISTEN_ADDRESS) && document[CONFIG_LISTEN_ADDRESS].IsString() )
    { _config->listen_address = document[CONFIG_LISTEN_ADDRESS].GetString(); }

    if(document.HasMember(CONFIG_DISPLAY_NAME) && document[CONFIG_DISPLAY_NAME].IsString() )
    { _config->name= document[CONFIG_DISPLAY_NAME].GetString(); }

    if(document.HasMember(CONFIG_CDS_URI) && document[CONFIG_CDS_URI].IsString() )
    { _config->cds_uri= document[CONFIG_CDS_URI].GetString(); }

    if(document.HasMember(CONFIG_UUID) && document[CONFIG_UUID].IsString() )
    { _config->uuid= document[CONFIG_UUID].GetString(); }

    return _config;
}
typedef std::shared_ptr< Config > config_t;
}//namespace upnp
#endif // CONFIG_H
