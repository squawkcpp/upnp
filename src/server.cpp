#include "server.h"

#include <iostream>

#include <sys/utsname.h>

#include "fmt/format.h"
#include "rapidxml_ns.hpp"
#include "spdlog/spdlog.h"

#include "http/request.h"
#include "http/response.h"
#include "http/utils/httpparser.h"
#include "http/httpclient.h"

#include "http/mod/exec.h"
#include "http/mod/file.h"
#include "http/mod/match.h"
#include "http/mod/method.h"

#include "_utils.h"
#include "didl.h"

namespace upnp {

UpnpCommand::TYPE UpnpCommand::parse( const std::string& command ) {
    //the content directory methods
    if( strcmp( command.c_str(),  "Browse" ) == 0 )
    { return BROWSE; }
    if( strcmp( command.c_str(),  "GetSearchCapabilities" ) == 0 )
    { return SEARCH_CAPABILITIES; }
    if( strcmp( command.c_str(),  "GetSortCapabilities" ) == 0 )
    { return SORT_CAPABILITIES; }
    if( strcmp( command.c_str(),  "GetSystemUpdateID" ) == 0 )
    { return SYSTEM_UPDATE_ID; }

    //the content manager methods
    if( strcmp( command.c_str(),  "GetCurrentConnectionIDs" ) == 0 )
    { return CURRENT_CONNECTION_IDS; }
    if( strcmp( command.c_str(),  "GetCurrentConnectionInfo" ) == 0 )
    { return CURRENT_CONNECTION_INFO; }
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
        FOR( "*", body_node, [&_upnp_command]( rapidxml_ns::xml_node<>* browse_node ) {
            _upnp_command.type = _upnp_command.parse( browse_node->local_name() );
            FOR( "*", browse_node, [&_upnp_command]( rapidxml_ns::xml_node<>* _flag_node ) {
                _upnp_command.values[_flag_node->local_name() ] = _flag_node->value();
            });
        });
    });
    return _upnp_command;
}

Server::Server(const std::string& redis, /** @param redis redis host */
       const short port /** @param port the redis port. */ )

    : redis_( data::make_connection( redis, port ) ),
      config_( json( data::config( redis_ ) ) ) {

    if ( !sub_.connect( redis, port ) ) {
        spdlog::get ( LOGGER )->error ( "can not subscribe to redis queue" );
    } else {
        sub_.subscribe ( "CDS_SUBSCRIBE", [this] ( const std::string & topic, const std::string & msg ) {
            spdlog::get ( LOGGER )->debug ( "CDS SUBSCRIBE:{}:{}", topic, msg );
            //persist the url
            redis_->command( {redis::ZADD, data::make_key( "upnp", "event", "cds" ), "0", msg } );
            //fire initial event

            http::utils::UrlParser p { msg };
            http::HttpClient< http::Http > _client ( p.host(), p.proto() );
            http::Request _request ( p.path() );
            _request.method( "NOTIFY" );

            _request.parameter ( http::header::HOST , fmt::format ( "{}:{}", p.host(), p.proto() ) );
            _request.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::XML ) );
            _request.parameter ( http::header::CONNECTION, http::header::CONNECTION_CLOSE ); // TODO how to set that correctly
            _request.parameter ( http::header::CACHE_CONTROL, "no-cache" );
            _request.parameter ( "SID", fmt::format ("uuid:{}", config_->uuid ) );
            _request.parameter ( "NT", "upnp:event" );
            _request.parameter ( "NTS", "upnp:propchange" );
            _request.parameter ( "SEQ", "0" ); // TODO parameter

            /* TODO generate xml*/  _request << R"xml(<e:propertyset xmlns:e="urn:schemas-upnp-org:event-1-0" xmlns:s="urn:schemas-upnp-org:service:ContentDirectory:1"><e:property><TransferIDs></TransferIDs></e:property><e:property><SystemUpdateID>666</SystemUpdateID></e:property></e:propertyset>)xml";
            std::stringstream _ss;
            auto _response = _client.get ( _request, _ss );
            if( _response.status() != http::http_status::OK ) {
                spdlog::get ( LOGGER )->debug ( "CDS NOTIFY ERROR:{}", static_cast< int >( _response.status() ) );
            }

        } );
        sub_.subscribe ( "CDS_UNSUBSCRIBE", [] ( const std::string & topic, const std::string & msg ) {
            spdlog::get ( LOGGER )->debug ( "CDS UNSUBSCRIBE:{}:{}", topic, msg );
            //persist the url
            //fire initial event
        } );
        sub_.subscribe ( "CMS_SUBSCRIBE", [] ( const std::string & topic, const std::string & msg ) {
            sleep( 1 );
            spdlog::get ( LOGGER )->debug ( "CMS SUBSCRIBE:{}:{}", topic, msg );
            //persist the url
            //fire initial event
            http::utils::UrlParser p { msg };
            std::cout << p.host() << ":" << p.proto() << p.path() << std::endl;
            http::HttpClient< http::Http > _client ( p.host(), p.proto() );
            http::Request _request ( p.path() );
            _request.method( "NOTIFY" );
            char* _content = R"xml(<e:propertyset xmlns:e="urn:schemas-upnp-org:event-1-0" xmlns:s="urn:schemas-upnp-org:service:ConnectionManager:1"><e:property><SourceProtocolInfo>http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_TN,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_SM,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_MED,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_LRG,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_HD_50_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_HD_60_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_HP_HD_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_HD_AAC_MULT5_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_HD_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_HD_MPEG1_L3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_SD_AAC_MULT5_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_SD_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_SD_MPEG1_L3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_HD_NA_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_NA_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_EU_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG1,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_SD_AAC_MULT5,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_SD_AC3,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF15_AAC_520,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF30_AAC_940,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L31_HD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L32_HD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L3L_SD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_HP_HD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_HD_1080i_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_HD_720p_AAC,http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_ASP_AAC,http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_SP_VGA_AAC,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_50_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_50_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_60_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_60_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HP_HD_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AAC_MULT5,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AAC_MULT5_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_MPEG1_L3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_MPEG1_L3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AAC_MULT5,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AAC_MULT5_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_MPEG1_L3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_MPEG1_L3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_EU,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_EU_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA_T,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPLL_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPML_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPML_MP3,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_FULL,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_PRO,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_FULL,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_PRO,http-get:*:video/3gpp:DLNA.ORG_PN=MPEG4_P2_3GPP_SP_L0B_AAC,http-get:*:video/3gpp:DLNA.ORG_PN=MPEG4_P2_3GPP_SP_L0B_AMR,http-get:*:audio/mpeg:DLNA.ORG_PN=MP3,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMABASE,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAFULL,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAPRO,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMALSL,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMALSL_MULT5,http-get:*:audio/mp4:DLNA.ORG_PN=AAC_ISO_320,http-get:*:audio/3gpp:DLNA.ORG_PN=AAC_ISO_320,http-get:*:audio/mp4:DLNA.ORG_PN=AAC_ISO,http-get:*:audio/mp4:DLNA.ORG_PN=AAC_MULT5_ISO,http-get:*:audio/L16;rate=44100;channels=2:DLNA.ORG_PN=LPCM,http-get:*:image/jpeg:*,http-get:*:video/avi:*,http-get:*:video/divx:*,http-get:*:video/x-matroska:*,http-get:*:video/mpeg:*,http-get:*:video/mp4:*,http-get:*:video/x-ms-wmv:*,http-get:*:video/x-msvideo:*,http-get:*:video/x-flv:*,http-get:*:video/x-tivo-mpeg:*,http-get:*:video/quicktime:*,http-get:*:audio/mp4:*,http-get:*:audio/x-wav:*,http-get:*:audio/x-flac:*,http-get:*:application/ogg:*</SourceProtocolInfo></e:property><e:property><SinkProtocolInfo></SinkProtocolInfo></e:property><e:property><CurrentConnectionIDs>0</CurrentConnectionIDs></e:property></e:propertyset>)xml";
            _request.write( _content, std::streamsize( strlen( _content )) );
            std::stringstream _ss;
            SPDLOG_DEBUG( spdlog::get ( LOGGER ), "send event to: " + msg );
            auto _response = _client.get ( _request, _ss );
            std::cout << "NOTIFY:" << _response << std::endl;
        } );
        sub_.subscribe ( "CMS_UNSUBSCRIBE", [] ( const std::string & topic, const std::string & msg ) {
            spdlog::get ( LOGGER )->debug ( "CMS UNSUBSCRIBE:{}:{}", topic, msg );
            //persist the url
            //fire initial event
        } );
    }
}

http::http_status Server::cds( http::Request& request, http::Response& response ) {

    if( request.method() == "SUBSCRIBE" ) {
        std::stringstream _ss; //TODO
        _ss << request;
        SPDLOG_DEBUG( spdlog::get( LOGGER ), "event/cds: {0}", _ss.str() );

        response.parameter ( http::header::CONTENT_LENGTH, "0" );
        response.parameter ( "Timeout", "Second-1800" ); // TODO parameter

        std::string _callback = request.parameter( "Callback");
        if( !_callback.empty() ) {
            if( _callback.at(0) == '<' ) {
                _callback.erase( 0, 1 );
            } if( _callback.at(_callback.length() - 1 ) == '>'  ) {
                _callback.erase( _callback.length() - 1 );
            }
            redis_->publish ( "CDS_SUBSCRIBE", _callback );
        } else {
            SPDLOG_DEBUG( spdlog::get( LOGGER ), "SUBSCRIBE with empty callback: ", request.str() );
        }

    } else if( request.method() == "UNSUBSCRIBE" ) {
        SPDLOG_DEBUG( spdlog::get( LOGGER ), "event/cds: {0}", request.str() );
        response.parameter ( http::header::CONTENT_LENGTH, "0" );
        response.parameter ( "SID", fmt::format ("uuid:{}", config_->uuid ) );
        response.parameter ( "EXT", "" );

        std::string _callback = request.parameter( "Callback");
        if( !_callback.empty() ) {
            if( _callback.at(0) == '<' ) {
                _callback.erase( 0, 1 );
            } if( _callback.at(_callback.length() - 1 ) == '>'  ) {
                _callback.erase( _callback.length() - 1 );
            }
            redis_->publish ( "CDS_UNSUBSCRIBE", _callback );
        } else {
            SPDLOG_DEBUG( spdlog::get( LOGGER ), "CDS_SUBSCRIBE with empty callback: ", request.str() );
        }

    } else if( request.method() == "POST" ) {
        UpnpCommand _upnp_command = parse( request.str() );
        SPDLOG_DEBUG( spdlog::get( LOGGER ), "cds request: {}", _upnp_command.str() );
        if(_upnp_command.type == UpnpCommand::BROWSE ) {

            //get request attributes
            int _index = ( _upnp_command.contains( "StartingIndex" ) ? std::stoi( _upnp_command.get( "StartingIndex" ) ) : 0 );
            int _count = ( _upnp_command.contains( "RequestedCount" ) ? std::stoi( _upnp_command.get( "RequestedCount" ) ) : 0 );
            if( _count== 0 ) _count = -1; //Browse all is -1 in redis.
            std::string _object_id = _upnp_command.get( OBJECT_ID );
            if( _object_id == "" || _object_id == "0" ) {
                _object_id = "root";
            }

            Didl _didl( redis_, config_ );
            if ( _upnp_command.contains( BROWSE_FLAG ) &&
                 _upnp_command.get( BROWSE_FLAG ) == BROWSE_METADATA ) {

                SPDLOG_DEBUG( spdlog::get( LOGGER ), "browse metadata: ", _object_id );
                auto _node = data::node( redis_, _object_id );
                _didl.write( _upnp_command.get( OBJECT_ID ), _node );

                SPDLOG_DEBUG( spdlog::get( LOGGER ), _didl.str() );
                response << soap_envelope( _didl.str(), 1, 1 );

            } else if ( _upnp_command.contains( BROWSE_FLAG ) &&
                        _upnp_command.get( BROWSE_FLAG ) == BROWSE_DIRECT_CHILDREN ) {

                SPDLOG_DEBUG( spdlog::get( LOGGER ), "browse children: ", _object_id );
                data::NodeType::Enum _type = data::NodeType::parse( data::get( redis_, _object_id, param::CLASS ) ); 
                if( _type == data::NodeType::album ) {
                    data::files( redis_, _object_id, data::NodeType::audio, _index, _count, [this,&_didl,&_upnp_command]( const std::string& key ) {
                        _didl.write( key, data::node( redis_, key ) );
                    });
                } else {
                    data::children( redis_, _object_id, _index, _count, "alpha" /** TODO */ , "asc", "", [this,&_didl,&_upnp_command]( const std::string& key ) {
                        _didl.write( key, data::node( redis_, key ) );
                    });
                }
                SPDLOG_DEBUG( spdlog::get( LOGGER ), _didl.str() );
                response << soap_envelope( _didl.str(), _didl.count(), data::children_count( redis_, _object_id ) );

            } else {
                SPDLOG_DEBUG( spdlog::get( LOGGER ), "unknown browse request: {0}:{1}" , _upnp_command.get( BROWSE_FLAG ), _upnp_command.get( OBJECT_ID ) );
            }

        } else if(_upnp_command.type == UpnpCommand::SORT_CAPABILITIES ) {
            response << R"xml(<?xml version="1.0" encoding="utf-8"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><s:Body><u:GetSortCapabilitiesResponse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1"><SortCaps>dc:title,dc:date,upnp:class,upnp:album,upnp:originalTrackNumber</SortCaps></u:GetSortCapabilitiesResponse></s:Body></s:Envelope>)xml";

        } else if(_upnp_command.type == UpnpCommand::SEARCH_CAPABILITIES ) {
            response << R"xml(<?xml version="1.0" encoding="utf-8"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><s:Body><u:GetSearchCapabilitiesResponse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1"><SearchCaps>dc:creator,dc:date,dc:title,upnp:album,upnp:actor,upnp:artist,upnp:class,upnp:genre,@refID</SearchCaps></u:GetSearchCapabilitiesResponse></s:Body></s:Envelope>)xml";

        } else if(_upnp_command.type == UpnpCommand::SYSTEM_UPDATE_ID ) {
            response << R"xml(<?xml version="1.0" encoding="utf-8"?>
                        <s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><s:Body><u:GetSystemUpdateIDResponse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1"><Id>32</Id></u:GetSystemUpdateIDResponse></s:Body></s:Envelope>)xml";

        } else {
            SPDLOG_DEBUG( spdlog::get( LOGGER ), "unknown request: {0}:{1}" , _upnp_command.get( BROWSE_FLAG ), _upnp_command.get( OBJECT_ID ) );
        }
    } else {
        SPDLOG_DEBUG( spdlog::get( LOGGER), "unknown method: {0}:{1} -> {2}", request.method(), request.uri(), request.str() );
    }
    response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::XML ) );
    return http::http_status::OK;
}

http::http_status Server::cms( http::Request& request, http::Response& response ) {

    if( request.method() == "SUBSCRIBE" ) {
        SPDLOG_DEBUG( spdlog::get( LOGGER ), "event/cms: {0}", request.str() );
        response.parameter ( http::header::CONTENT_LENGTH, "0" );
        response.parameter ( "SID", fmt::format ("uuid:{}", config_->uuid ) );
        response.parameter ( "Timeout", "Second-1800" ); // TODO parameter

        std::string _callback = request.parameter( "Callback");
        if( !_callback.empty() ) {
            if( _callback.at(0) == '<' ) {
                _callback.erase( 0, 1 );
            } if( _callback.at(_callback.length() - 1 ) == '>'  ) {
                _callback.erase( _callback.length() - 1 );
            }
            redis_->publish ( "CMS_SUBSCRIBE", _callback );
        } else {
            SPDLOG_DEBUG( spdlog::get( LOGGER ), "CMS_SUBSCRIBE with empty callback: ", request.str() );
        }

    } else if( request.method() == "UNSUBSCRIBE" ) {
        SPDLOG_DEBUG( spdlog::get( LOGGER ), "event/cms unsubscribe: {0}", request.str() );
        response.parameter ( http::header::CONTENT_LENGTH, "0" );
        response.parameter ( "SID", fmt::format ("uuid:{}", config_->uuid ) );
        redis_->publish ( "CMS_UNSUBSCRIBE", request.str() );

    } else if( request.method() == "POST" ) {
        UpnpCommand _upnp_command = parse( request.str() );
        SPDLOG_DEBUG( spdlog::get( LOGGER ), "cms request: {}", _upnp_command.str() );
        if( _upnp_command.type == UpnpCommand::GET_PROTOCOL_INFO ) {

            rapidxml_ns::xml_document<> doc;

            auto root_n = element<rapidxml_ns::xml_node<>>( &doc, &doc, "s:Envelope", "" );
            attr( &doc, root_n, "xmlns:s", "http://schemas.xmlsoap.org/soap/envelope/" );
            attr( &doc, root_n, "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" );
            attr( &doc, root_n, "s:encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/" );

            auto body_n = element<rapidxml_ns::xml_node<>>( &doc, root_n, "s:Body", "" );
            auto _browseResponse_n = element<rapidxml_ns::xml_node<>>( &doc, body_n, "u:GetProtocolInfoResponse", "" );
            attr( &doc, _browseResponse_n, "xmlns:u", "urn:schemas-upnp-org:service:ConnectionManager:1" );

            auto _source_n = element<rapidxml_ns::xml_node<>>( &doc, _browseResponse_n, "Source", "http-get:*:audio/mpeg:*,http-get:*:text/plain:*,http-get:*:video/mp4:*,http-get:*:video/mpeg:*,http-get:*:video/quicktime:*" /*SOURCE_TYPES*/ );
            attr( &doc, _source_n, "xsi:type", "xsd:string" );

            auto _sink_n = element<rapidxml_ns::xml_node<>>( &doc, _browseResponse_n, "Sink", "" );
            attr( &doc, _sink_n, "xsi:type", "xsd:string" );

            std::string s = "<?xml version=\"1.0\"?>\n";
            rapidxml_ns::print( std::back_inserter(s), doc, rapidxml_ns::print_no_indenting );
            response <<  s;

            response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::XML ) );
            response.status ( http::http_status::OK );

        } else {
            SPDLOG_DEBUG( spdlog::get( LOGGER), "unknown request: {}:{} -> {}", request.method(), request.uri(), request.str() );
        }
    } else {
        SPDLOG_DEBUG( spdlog::get( LOGGER), "unknown method: {}:{} -> {}", request.method(), request.uri(), request.str() );
    }

    response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::XML ) );
    return http::http_status::OK;
}

http::http_status Server::description( http::Request&, http::Response& response ) {
    rapidxml_ns::xml_document<> doc;

    auto root_n = element<rapidxml_ns::xml_node<>>( &doc, &doc, "root", "" );
    attr( &doc, root_n, "xmlns", "urn:schemas-upnp-org:device-1-0" );
//TODO    attr( &doc, root_n, "xmlns:sec", "http://www.sec.co.kr/dlna" );

    auto spec_n = element<rapidxml_ns::xml_node<>>( &doc, root_n, "specVersion", "" );
    element<rapidxml_ns::xml_node<>>( &doc, spec_n, "major", "1" );
    element<rapidxml_ns::xml_node<>>( &doc, spec_n, "minor", "0" );

    auto device_n = element<rapidxml_ns::xml_node<>>( &doc, root_n, "device", "" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "deviceType", "urn:schemas-upnp-org:device:MediaServer:1" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "friendlyName", config_->name );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "manufacturer", "loudlabs" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "manufacturerURL", "https://github.com/squawkcpp" );
//    element<rapidxml_ns::xml_node<>>( &doc, device_n, "sec:ProductCap", "smi,DCM10,getMediaInfo.sec,getCaptionInfo.sec" );
//    element<rapidxml_ns::xml_node<>>( &doc, device_n, "sec:ProductCap", "smi,DCM10,getMediaInfo.sec,getCaptionInfo.sec" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "modelDescription", "Squawk Content Directory Server." );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "modelName", "Squawk Media Server" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "modelNumber", "0" /* TODO VERSION */ );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "modelURL", "https://github.com/squawkcpp" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "serialNumber", "0" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "UDN", fmt::format ("uuid:{}", config_->uuid ) );
    auto _dlna_doc_n = element<rapidxml_ns::xml_node<>>( &doc, device_n, "dlna:X_DLNADOC", "DMS-1.50" );
    attr( &doc, _dlna_doc_n, "xmlns:dlna", "urn:schemas-dlna-org:device-1-0" );
    element<rapidxml_ns::xml_node<>>( &doc, device_n, "presentationURL", "/" );

    auto _icon_list = element<rapidxml_ns::xml_node<>>( &doc, device_n, "iconList", "" );
    auto _icon_48 = element<rapidxml_ns::xml_node<>>( &doc, _icon_list, "icon", "" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_48, "mimetype", "image/png" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_48, "width", "48" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_48, "height", "48" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_48, "depth", "24" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_48, "url", "/squawk48.png" );

    auto _icon_64 = element<rapidxml_ns::xml_node<>>( &doc, _icon_list, "icon", "" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_64, "mimetype", "image/png" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_64, "width", "64" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_64, "height", "64" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_64, "depth", "24" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_64, "url", "/squawk64.png" );

    auto _icon_128 = element<rapidxml_ns::xml_node<>>( &doc, _icon_list, "icon", "" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_128, "mimetype", "image/png" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_128, "width", "128" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_128, "height", "128" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_128, "depth", "24" );
    element<rapidxml_ns::xml_node<>>( &doc, _icon_128, "url", "/squawk128.png" );

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

//TODO    auto _service_ms_media = element<rapidxml_ns::xml_node<>>( &doc, _service_list, "service", "" );
//    element<rapidxml_ns::xml_node<>>( &doc, _service_ms_media , "serviceType", "urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1" );
//    element<rapidxml_ns::xml_node<>>( &doc, _service_ms_media , "serviceId", "urn:microsoft.com:serviceId:X_MS_MediaReceiverRegistrar" );
//    element<rapidxml_ns::xml_node<>>( &doc, _service_ms_media , "controlURL", "/ctl/X_MS_MediaReceiverRegistrar" );
//    element<rapidxml_ns::xml_node<>>( &doc, _service_ms_media , "eventSubURL", "/evt/X_MS_MediaReceiverRegistrar" );
//    element<rapidxml_ns::xml_node<>>( &doc, _service_ms_media , "SCPDURL", "/X_MS_MediaReceiverRegistrar.xml" );

    //TODO element<rapidxml_ns::xml_node<>>( &doc, device_n, "URLBase", fmt::format("http://{}:{}", config_->listen_address, config_->http_port ) );

    std::string s = "<?xml version=\"1.0\"?>\r\n";
    rapidxml_ns::print( std::back_inserter(s), doc, rapidxml_ns::print_no_indenting );
    response << s;
    response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::XML ) );
    response.parameter ( "EXT", "" );
    return http::http_status::OK;
}
}//namespace upnp
