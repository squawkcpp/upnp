#include "server.h"

#include  <iostream>

#include "fmt/format.h"
#include "rapidxml_ns.hpp"
#include "spdlog/spdlog.h"

#include "http/mod/exec.h"
#include "http/mod/file.h"
#include "http/mod/match.h"
#include "http/mod/method.h"

#include "_utils.h"
#include "didl.h"

namespace upnp {

UpnpCommand::TYPE UpnpCommand::parse( const std::string& command ) {
    if( strcmp( command.c_str(),  "Browse" ) == 0 )
    { return BROWSE; }
    if( strcmp( command.c_str(),  "GetProtocolInfo" ) == 0 )
    { return GET_PROTOCOL_INFO; }

    return NONE;
}

UpnpCommand Server::parse( const std::string& body ) {
    UpnpCommand _upnp_command;
    rapidxml_ns::xml_document<> doc;
    doc.parse<0>( const_cast< char* >(  body.c_str() ) );
    auto root_node = doc.first_node();
    FOR( "Body", root_node, [&_upnp_command]( rapidxml_ns::xml_node<>* body_node ) {
        std::cout << body_node->local_name() << std::endl;
        FOR( "*", body_node, [&_upnp_command]( rapidxml_ns::xml_node<>* browse_node ) {
            std::cout << browse_node->local_name() << std::endl;
            _upnp_command.type = _upnp_command.parse( browse_node->local_name() );
            FOR( "*", browse_node, [&_upnp_command]( rapidxml_ns::xml_node<>* _flag_node ) {
                _upnp_command.values[_flag_node->local_name() ] = _flag_node->value();
            });
        });
    });
    return _upnp_command;
}

http::http_status Server::cds( http::Request& request, http::Response& response ) {
    UpnpCommand _upnp_command = parse( request.str() );
    std::cout << "cds: " << _upnp_command << std::endl;
    if(_upnp_command.type == UpnpCommand::BROWSE ) {
        if ( _upnp_command.contains( BROWSE_FLAG ) &&
             _upnp_command.get( BROWSE_FLAG ) == BROWSE_METADATA ) {

            std::cout << "browse metadata" << std::endl;
//        } else if ( _upnp_command.contains( BROWSE_FLAG ) &&
//                    _upnp_command.get( BROWSE_FLAG ) == BROWSE_DIRECT_CHILDREN &&
//                    _upnp_command.get( OBJECT_ID ) == "0" ) {

//            std::cout << "browse root nodes" << std::endl;

        } else if ( _upnp_command.contains( BROWSE_FLAG ) &&
                    _upnp_command.get( BROWSE_FLAG ) == BROWSE_DIRECT_CHILDREN ) {

            std::cout << "browse children:" << _upnp_command.get( OBJECT_ID ) << std::endl;
            ////            _container.www->bind( http::mod::Match<std::string>( "^\\/+(root|file|ebook|movie|album|serie|artist|image|[[:digit:]]+)\\/+nodes$", data::KEY_KEY ),
            ////            _container.www->bind( http::mod::Match< std::string, std::string >( "^\\/+(ebook|movie|album|serie|artist|image)\\/+(.*)$", data::KEY_TYPE, cds::PARAM_NAME ),

            int _index = ( _upnp_command.contains( "StartingIndex" ) ? std::stoi( _upnp_command.get( "StartingIndex" ) ) : 0 );
            int _count = ( _upnp_command.contains( "RequestedCount" ) ? std::stoi( _upnp_command.get( "RequestedCount" ) ) : 0 );
            std::string _object_id = _upnp_command.get( OBJECT_ID );
            if( _object_id == "0" ) {
                _object_id = "/";
            }

            Didl _didl( redis_, config_ );
            data::children( redis_, _object_id, _index, _count, [this,&_didl]( const std::string& key ) {
                _didl.write( key, data::children_count( redis_, key ), data::node( redis_, key ) );
            });

            std::cout << _didl.str() << std::endl;
            response << soap_envelope( _didl.str(), _didl.count(), data::children_count( redis_, _object_id ) );

        } else {
            std::cout << "unknown request: " << _upnp_command.get( BROWSE_FLAG ) << ":" << _upnp_command.get( OBJECT_ID ) << std::endl;

        }
    } else if(_upnp_command.type == UpnpCommand::GET_PROTOCOL_INFO ) {
        std::cout << "get protocol info " << std::endl;
    } else {
        std::cout << "other type " << _upnp_command.type << std::endl;
    }

    response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::XML ) );
    return http::http_status::OK;
}

http::http_status Server::cms( http::Request& request, http::Response& response ) {
    response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::XML ) );
    return http::http_status::OK;
}

http::http_status Server::description( http::Request& request, http::Response& response ) {
    rapidxml_ns::xml_document<> doc;

    auto root_n = element<rapidxml_ns::xml_node<>>( &doc, &doc, "root", "" );
    attr( &doc, root_n, "xmlns", "urn:schemas-upnp-org:device-1-0" );
    attr( &doc, root_n, "xmlns:sec", "http://www.sec.co.kr/dlna" );

    auto spec_n = element<rapidxml_ns::xml_node<>>( &doc, root_n, "specVersion", "" );
    element<rapidxml_ns::xml_node<>>( &doc, spec_n, "major", "1" );
    element<rapidxml_ns::xml_node<>>( &doc, spec_n, "minor", "0" );

    auto device_n = element<rapidxml_ns::xml_node<>>( &doc, root_n, "device", "" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "deviceType", "urn:schemas-upnp-org:device:MediaServer:1" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "friendlyName", config_->name );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "manufacturer", "" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "manufacturerURL", "https://github.com/squawkcpp" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "sec:ProductCap", "smi,DCM10,getMediaInfo.sec,getCaptionInfo.sec" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "sec:ProductCap", "smi,DCM10,getMediaInfo.sec,getCaptionInfo.sec" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "modelDescription", "" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "modelName", "Squawk Media Server" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "modelNumber", "1" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "modelURL", "https://github.com/squawkcpp" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "serialNumber", "" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "UDN", config_->uuid );
    auto _dlna_doc_n = element<rapidxml_ns::xml_node<>>( &doc, device_n, "dlna:X_DLNADOC", "DMS-1.50" );
    attr( &doc, _dlna_doc_n, "xmlns:dlna", "urn:schemas-dlna-org:device-1-0" );
    auto _icon_list = element<rapidxml_ns::xml_node<>>( &doc, device_n, "iconList", "" );
    auto _icon_48 = element<rapidxml_ns::xml_node<>>( &doc, _icon_list, "icon", "" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_48, "mimeType", "image/png" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_48, "width", "48" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_48, "height", "48" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_48, "depth", "24" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_48, "url", fmt::format( "{}/icons/squawk48.png", config_->cds_uri ) );

    auto _icon_64 = element<rapidxml_ns::xml_node<>>( &doc, _icon_list, "icon", "" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_64, "mimeType", "image/png" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_64, "width", "64" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_64, "height", "64" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_64, "depth", "24" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_64, "url", fmt::format( "{}/icons/squawk64.png", config_->cds_uri ) );

    auto _icon_128 = element<rapidxml_ns::xml_node<>>( &doc, _icon_list, "icon", "" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_128, "mimeType", "image/png" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_128, "width", "128" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_128, "height", "128" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_128, "depth", "24" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_128, "url", fmt::format( "{}/icons/squawk128.png", config_->cds_uri ) );

    auto _service_list = element<rapidxml_ns::xml_node<>>( &doc, device_n, "serviceList", "" );
    auto _service_cds = element<rapidxml_ns::xml_node<>>( &doc, _service_list, "service", "" );
    element<rapidxml_ns::xml_node<>>( &doc, _service_cds , "serviceType", "urn:schemas-upnp-org:service:ContentDirectory:1" );
    element<rapidxml_ns::xml_node<>>( &doc, _service_cds , "serviceId", "urn:upnp-org:serviceId:ContentDirectory" );
    element<rapidxml_ns::xml_node<>>( &doc, _service_cds , "controlURL", "/ctl/cds" );
    element<rapidxml_ns::xml_node<>>( &doc, _service_cds , "eventSubURL", "/ctl/cds" );
    element<rapidxml_ns::xml_node<>>( &doc, _service_cds , "SCPDURL", "/cds.xml" );

    auto _service_cms = element<rapidxml_ns::xml_node<>>( &doc, _service_list, "service", "" );
    element<rapidxml_ns::xml_node<>>( &doc, _service_cms , "serviceType", "urn:schemas-upnp-org:service:ConnectionManager:1" );
    element<rapidxml_ns::xml_node<>>( &doc, _service_cms , "serviceId", "urn:upnp-org:serviceId:ConnectionManager" );
    element<rapidxml_ns::xml_node<>>( &doc, _service_cms , "controlURL", "/ctl/cms" );
    element<rapidxml_ns::xml_node<>>( &doc, _service_cms , "eventSubURL", "/ctl/cms" );
    element<rapidxml_ns::xml_node<>>( &doc, _service_cms , "SCPDURL", "/cms.xml" );

    element<rapidxml_ns::xml_node<>>( &doc, device_n, "URLBase", fmt::format("http://{}:{}", config_->listen_address, config_->http_port ) );

    std::string s = "<?xml version=\"1.0\"?>\n";
    rapidxml_ns::print( std::back_inserter(s), doc, rapidxml_ns::print_no_indenting );
    response << s;
    response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::XML ) );
    return http::http_status::OK;
}
}//namespace upnp
