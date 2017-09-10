#include "didl.h"

#include "spdlog/spdlog.h"

#include "_utils.h"

namespace upnp {
Didl::Didl( data::redis_ptr redis, config_t config ) : redis_(redis), config_(config), result_( 0 ) {
    root_node_ = element<rapidxml_ns::xml_node<>>( &doc_, &doc_, DIDL_ROOT_NODE, "" );
    attr( &doc_, root_node_, "xmlns", XML_NS_DIDL );
    attr( &doc_, root_node_, "xmlns:dc", XML_NS_PURL );
    attr( &doc_, root_node_, "xmlns:dlna", XML_NS_DLNA );
    attr( &doc_, root_node_, "xmlns:upnp", XML_NS_UPNP );
    attr( &doc_, root_node_, "xmlns:pv", XML_NS_PV );
}

void Didl::write( const std::string& key, const std::map< std::string, std::string >& values ) {

    if( data::NodeType::parse( values.at( data::KEY_CLASS ) ) == data::NodeType::Enum::folder ) {
        auto _container_n = element<rapidxml_ns::xml_node<>>( &doc_, root_node_, DIDL_ELEMENT_CONTAINER, "" );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_ID, key.c_str() );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_PARENT_ID, values.at( data::KEY_PARENT ) );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_RESTRICTED, "1" );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_CHILD_COUNT, std::to_string( data::children_count( redis_, key ) ) );

        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "dc:title", values.at( data::KEY_NAME ) );
        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:class", "object.container.storageFolder" );
        ++result_;

    } else if( data::NodeType::parse( values.at( data::KEY_CLASS ) ) == data::NodeType::Enum::serie ) {
        auto _container_n = element<rapidxml_ns::xml_node<>>( &doc_, root_node_, DIDL_ELEMENT_CONTAINER, "" );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_ID, key.c_str() );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_PARENT_ID, values.at( data::KEY_PARENT ) );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_RESTRICTED, "1" );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_CHILD_COUNT, std::to_string( data::children_count( redis_, key ) ) );

        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "dc:title", values.at( data::KEY_NAME ) );
        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:class", "object.container.series" );

        if( values.find( "year" ) != values.end() )
        { element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "dc:data", values.at( "year" ) ); }

        if( values.find( "thumb" ) != values.end() ) {
            auto _cover_n = element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "albumArtURI", values.at( "thumb" ) );
            attr( &doc_, _cover_n, "dlna:profileID", "JPEG_TN" );
        }
        ++result_;

    } else if( data::NodeType::parse( values.at( data::KEY_CLASS ) ) == data::NodeType::Enum::album ) {

        auto _container_n = element<rapidxml_ns::xml_node<>>( &doc_, root_node_, DIDL_ELEMENT_CONTAINER, "" );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_ID, key.c_str() );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_PARENT_ID, values.at( data::KEY_PARENT ) );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_RESTRICTED, "1" );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_CHILD_COUNT, std::to_string( data::children_count( redis_, key ) ) );

        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "dc:title", values.at( data::KEY_NAME ) );
        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:class", "object.container.album.musicAlbum" );

        //get the artists
        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:artist", values.at( "artist" ) );
        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "dc:data", values.at( "year" ) );

        if( values.find( "thumb" ) != values.end() ) {
            auto _cover_n = element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "albumArtURI", fmt::format( "{0}{1}", config_->cds_uri, values.at( "thumb" ) ) );
            attr( &doc_, _cover_n, "dlna:profileID", "JPEG_TN" );
        }
        ++result_;


    } else if( data::NodeType::parse( values.at( data::KEY_CLASS ) ) == data::NodeType::Enum::file ) {

        auto _container_n = element<rapidxml_ns::xml_node<>>( &doc_, root_node_, "item", "" );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_ID, key.c_str() );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_PARENT_ID, values.at( data::KEY_PARENT ) );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_RESTRICTED, "1" );

        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "dc:title", values.at( data::KEY_NAME ) );
        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:class", "object.item" );

        auto _res_n = element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "res", fmt::format( "{0}/res/{1}{2}",
            config_->cds_uri, key, values.at( "ext" ) ) );

        attr( &doc_, _res_n, "protocolInfo", fmt::format( "http-get:*:{}:DLNA.ORG_OP=11;DLNA.ORG_FLAGS=01700000000000000000000000000000", values.at( "mimeType" ) ) );

        if( values.find( "size" ) != values.end() )
        { attr( &doc_, _res_n, "size", values.at( "size" ) ); }
        if( values.find( "mimeType" ) != values.end() )
        { attr( &doc_, _res_n, "mime-type", values.at( "mimeType" ) ); }
        ++result_;


    } else if( data::NodeType::parse( values.at( data::KEY_CLASS ) ) == data::NodeType::Enum::audio ) {

        auto _container_n = element<rapidxml_ns::xml_node<>>( &doc_, root_node_, "item", "" );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_ID, key.c_str() );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_PARENT_ID, values.at( data::KEY_PARENT ) );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_RESTRICTED, "1" );

        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "dc:title", values.at( data::KEY_NAME ) );
        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:class", "object.item.audioItem.musicTrack" );

        if( values.find( "track" ) != values.end() )
        { element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:originalTrackNumber", values.at( "track" ) ); }
        if( values.find( "album" ) != values.end() )
        { element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:album", values.at( "album" ) ); }
        if( values.find( "comment" ) != values.end() )
        { element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:description", values.at( "comment" ) ); }
        if( values.find( "artist" ) != values.end() )
        { element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:artist", values.at( "artist" ) ); }
        if( values.find( "year" ) != values.end() )
        { element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "dc:date", values.at( "year" ) ); }
        if( values.find( "genre" ) != values.end() )
        { element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:genre", values.at( "genre" ) ); }

        auto _cover_n = element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "albumArtURI",
            fmt::format( "{0}{1}", config_->cds_uri, data::get( redis_, values.at( data::KEY_PARENT ), "thumb" ) ) );
        attr( &doc_, _cover_n, "dlna:profileID", "JPEG_TN" );

        auto _res_n = element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "res", fmt::format( "{0}/res/{1}{2}",
            config_->cds_uri, key, values.at( "ext" ) ) );

        attr( &doc_, _res_n, "protocolInfo", fmt::format( "http-get:*:{}:DLNA.ORG_OP=11;DLNA.ORG_FLAGS=01700000000000000000000000000000", values.at( "mimeType" ) ) );

        if( values.find( "size" ) != values.end() )
        { attr( &doc_, _res_n, "size", values.at( "size" ) ); }
        if( values.find( "mimeType" ) != values.end() )
        { attr( &doc_, _res_n, "mime-type", values.at( "mimeType" ) ); }
        if( values.find( "playlength" ) != values.end() )
        { attr( &doc_, _res_n, "duration", values.at( "playlength" ) ); }
        if( values.find( "bitrate" ) != values.end() )
        { attr( &doc_, _res_n, "bitrate", values.at( "bitrate" ) ); }
        if( values.find( "bps" ) != values.end() )
        { attr( &doc_, _res_n, "bitsPerSample", values.at( "bps" ) ); }
        if( values.find( "samplerate" ) != values.end() )
        { attr( &doc_, _res_n, "sampleFrequency", values.at( "samplerate" ) ); }
        if( values.find( "channels" ) != values.end() )
        { attr( &doc_, _res_n, "nrAudioChannels", values.at( "channels" ) ); }
        ++result_;

////TODO        std::time_t last_playback_time_ = db::get< const char * >( statement, didl::_DidlMusicTrack::playback_time );
////        xml_writer_->element ( item_element_, upnp::XML_NS_UPNP, "lastPlaybackTime", fmt::format("{:%Y-%m-%d %h:%m:%s}", *std::localtime( &last_playback_time_ ) ) );
//        xml_writer_->element ( item_element_, upnp::XML_NS_UPNP, "playbackCount", std::to_string( db::get< int >( statement, didl::DidlMusicTrack::playback_count ) ) );
//        xml_writer_->element ( item_element_, upnp::XML_NS_UPNP, "rating", std::to_string( db::get< int >( statement, didl::DidlMusicTrack::rating ) ) );


    } else if( data::NodeType::parse( values.at( data::KEY_CLASS ) ) == data::NodeType::Enum::image ||
               data::NodeType::parse( values.at( data::KEY_CLASS ) ) == data::NodeType::Enum::cover ) {

        auto _container_n = element<rapidxml_ns::xml_node<>>( &doc_, root_node_, "item", "" );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_ID, key.c_str() );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_PARENT_ID, values.at( data::KEY_PARENT ) );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_RESTRICTED, "1" );

        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "dc:title", values.at( data::KEY_NAME ) );
        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:class", "object.item.photo" );

        if( values.find( "track" ) != values.end() )
        { element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:originalTrackNumber", values.at( "track" ) ); }

        auto _res_n = element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "res", fmt::format( "{0}/res/{1}{2}",
            config_->cds_uri, key, values.at( "ext" ) ) );

        attr( &doc_, _res_n, "protocolInfo", fmt::format( "http-get:*:{}:DLNA.ORG_OP=11;DLNA.ORG_FLAGS=01700000000000000000000000000000", values.at( "mimeType" ) ) );

        if( values.find( "size" ) != values.end() )
        { attr( &doc_, _res_n, "size", values.at( "size" ) ); }
        if( values.find( "mimeType" ) != values.end() )
        { attr( &doc_, _res_n, "mime-type", values.at( "mimeType" ) ); }

        { attr( &doc_, _res_n, "resolution", fmt::format( "{0}x{1}", values.at( "width" ), values.at( "height" ) ) ); }
        if( values.find( "BitsPerSample" ) != values.end() ) //TODO BitsPerSample?
        { attr( &doc_, _res_n, "colorDepth", values.at( "BitsPerSample" ) ); }
        ++result_;

    } else if( data::NodeType::parse( values.at( data::KEY_CLASS ) ) == data::NodeType::Enum::movie ) {

        auto _container_n = element<rapidxml_ns::xml_node<>>( &doc_, root_node_, "item", "" );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_ID, key.c_str() );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_PARENT_ID, values.at( data::KEY_PARENT ) );
        attr( &doc_, _container_n, DIDL_ATTRIBUTE_RESTRICTED, "1" );

        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "dc:title", values.at( data::KEY_NAME ) );
        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:class", "object.item.movie" );

        auto _res_n = element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "res", fmt::format( "{0}/res/{1}{2}",
            config_->cds_uri, key, values.at( "ext" ) ) );

        attr( &doc_, _res_n, "protocolInfo", fmt::format( "http-get:*:{}:DLNA.ORG_OP=11;DLNA.ORG_FLAGS=01700000000000000000000000000000", values.at( "mimeType" ) ) );

        if( values.find( "size" ) != values.end() )
        { attr( &doc_, _res_n, "size", values.at( "size" ) ); }
        //TODO xml_writer_->attribute ( dlna_res_node, "", "dlnaProfile", item.dlnaProfile() );

        if( values.find( "mimeType" ) != values.end() )
        { attr( &doc_, _res_n, "mime-type", values.at( "mimeType" ) ); }
        if( values.find( "playlength" ) != values.end() )
        { attr( &doc_, _res_n, "duration", values.at( "playlength" ) ); }
        if( values.find( "bitrate" ) != values.end() )
        { attr( &doc_, _res_n, "bitrate", values.at( "bitrate" ) ); }

        { attr( &doc_, _res_n, "resolution", fmt::format( "{0}x{1}", values.at( "width" ), values.at( "height" ) ) ); }
        if( values.find( "BitsPerSample" ) != values.end() ) //TODO BitsPerSample?
        { attr( &doc_, _res_n, "colorDepth", values.at( "BitsPerSample" ) ); }
        ++result_;

    } else {
        SPDLOG_DEBUG( spdlog::get( LOGGER ), "class not found: {0}", values.at( data::KEY_CLASS ) );
    }
}
}//namespace upnp
