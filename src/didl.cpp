#include "didl.h"

#include "spdlog/spdlog.h"

#include "_utils.h"

namespace upnp {
const std::array< std::string, 11 > Didl::CLASS_NAMES = {
"object.container.storageFolder", "object.item.audioItem.musicTrack", "object.item.movie",
"object.container.series", "object.item.photo", "object.item" /*ebook*/, "object.item",
"object.container.album.musicAlbum", "object.item.photo", "object.item.movie",
"object.container" };

const std::array< data::NodeType::Enum, 4 > Didl::CLASS_CONTAINER = {
data::NodeType::Enum::album, data::NodeType::Enum::artist, data::NodeType::Enum::folder,
data::NodeType::Enum::serie };

Didl::Didl( data::redis_ptr redis, config_t config ) : redis_(redis), config_(config), result_( 0 ) {
    root_node_ = element<rapidxml_ns::xml_node<>>( &doc_, &doc_, DIDL_ROOT_NODE, "" );
    attr( &doc_, root_node_, "xmlns", XML_NS_DIDL );
    attr( &doc_, root_node_, "xmlns:dc", XML_NS_PURL );
    attr( &doc_, root_node_, "xmlns:dlna", XML_NS_DLNA );
    attr( &doc_, root_node_, "xmlns:upnp", XML_NS_UPNP );
    attr( &doc_, root_node_, "xmlns:pv", XML_NS_PV );
}

void Didl::write( const std::string& key, const std::map< std::string, std::string >& values ) {

    if( values.find( param::CLASS ) == values.end() )
    { return; }

    data::NodeType::Enum _type = data::NodeType::parse( values.at( param::CLASS ) );
    auto _container_n = element<rapidxml_ns::xml_node<>>( &doc_, root_node_,
        ( std::find( CLASS_CONTAINER.begin(), CLASS_CONTAINER.end(), _type ) != CLASS_CONTAINER.end() ? DIDL_ELEMENT_CONTAINER : DIDL_ELEMENT_ITEM ), "" );
    attr( &doc_, _container_n, DIDL_ATTRIBUTE_ID, key.c_str() );
    std::string _parent_id = values.at( param::PARENT );
    if( values.at( param::PARENT )=="" ) _parent_id = "-1";
    if( values.at( param::PARENT )=="root" ) _parent_id = "0";
    attr( &doc_, _container_n, DIDL_ATTRIBUTE_PARENT_ID, _parent_id );
    attr( &doc_, _container_n, DIDL_ATTRIBUTE_RESTRICTED, "1" );
    attr( &doc_, _container_n, DIDL_ATTRIBUTE_CHILD_COUNT, std::to_string( data::children_count( redis_, key ) ) );

    if( _type == data::NodeType::Enum::episode ) {
        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "dc:title", fmt::format( "S{0}{1} {2}",
                values.at( param::SEASON ), values.at( param::EPISODE ), values.at( param::NAME ) ) );
    } else {
        element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "dc:title", values.at( param::NAME ) );
    }

    element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:class", CLASS_NAMES.at( _type ) );

    if( values.find( param::YEAR ) != values.end() )
    { element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "dc:data", values.at( param::YEAR ) ); }
    if( values.find( param::ARTIST ) != values.end() )
    { element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:artist", values.at( param::ARTIST ) ); }
    if( values.find( param::TRACK ) != values.end() )
    { element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:originalTrackNumber", values.at( param::TRACK ) ); }
    if( values.find( param::ALBUM ) != values.end() )
    { element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:album", values.at( param::ALBUM ) ); }
    if( values.find( param::COMMENT ) != values.end() )
    { element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:description", values.at( param::COMMENT ) ); }
    if( values.find( param::YEAR ) != values.end() )
    { element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "dc:date", values.at( param::YEAR ) ); }
    if( values.find( param::GENRE ) != values.end() )
    { element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "upnp:genre", values.at( param::GENRE ) ); }

    if( _type == data::NodeType::Enum::audio ) {
        auto _cover_n = element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "albumArtURI",
            fmt::format( "{0}{1}", config_->cds_uri, data::get( redis_, values.at( param::PARENT ), param::MED ) ) );
        attr( &doc_, _cover_n, "dlna:profileID", "JPEG_MED" );
    } else if( values.find( param::THUMB ) != values.end() ) {
        auto _cover_n = element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "albumArtURI",
            fmt::format( "{0}{1}", config_->cds_uri, values.at( param::THUMB ) ) );
        attr( &doc_, _cover_n, "dlna:profileID", "JPEG_TN" );
    }

    if( _type == data::NodeType::Enum::file ) {
        auto _res_n = element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "res", fmt::format( "{0}/res/{1}{2}",
            config_->cds_uri, key, values.at( "ext" ) ) );

        attr( &doc_, _res_n, "protocolInfo", fmt::format( "http-get:*:{}:DLNA.ORG_OP=11;DLNA.ORG_FLAGS=01700000000000000000000000000000", values.at( "mimeType" ) ) );

        if( values.find( "size" ) != values.end() )
        { attr( &doc_, _res_n, "size", values.at( "size" ) ); }
        if( values.find( "mimeType" ) != values.end() )
        { attr( &doc_, _res_n, "mime-type", values.at( "mimeType" ) ); }
    } else if( _type == data::NodeType::Enum::audio ) {
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
    } else if ( _type == data::NodeType::Enum::image || _type == data::NodeType::Enum::cover ) {
        auto _res_n = element<rapidxml_ns::xml_node<>>( &doc_, _container_n, "res", fmt::format( "{0}/res/{1}{2}",
            config_->cds_uri, key, values.at( "ext" ) ) );

        attr( &doc_, _res_n, "protocolInfo", fmt::format( "http-get:*:{}:DLNA.ORG_OP=11;DLNA.ORG_FLAGS=01700000000000000000000000000000", values.at( "mimeType" ) ) );

        if( values.find( "size" ) != values.end() )
        { attr( &doc_, _res_n, "size", values.at( "size" ) ); }
        if( values.find( "mimeType" ) != values.end() )
        { attr( &doc_, _res_n, "mime-type", values.at( "mimeType" ) ); }

        if( values.find( "width" ) != values.end() &&  values.find( "height" ) != values.end() )
        { attr( &doc_, _res_n, "resolution", fmt::format( "{0}x{1}", values.at( "width" ), values.at( "height" ) ) ); }
        if( values.find( "BitsPerSample" ) != values.end() ) //TODO BitsPerSample?
        { attr( &doc_, _res_n, "colorDepth", values.at( "BitsPerSample" ) ); }
    } else if( _type == data::NodeType::movie ) {
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

        if( values.find( "width" ) != values.end() && values.find( "height" ) != values.end() )
        { attr( &doc_, _res_n, "resolution", fmt::format( "{0}x{1}", values.at( "width" ), values.at( "height" ) ) ); }
        if( values.find( "BitsPerSample" ) != values.end() ) //TODO BitsPerSample?
        { attr( &doc_, _res_n, "colorDepth", values.at( "BitsPerSample" ) ); }
    } else if( _type == data::NodeType::movie ) {
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
    }
    ++result_;
}
}//namespace upnp
