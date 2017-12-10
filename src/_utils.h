#ifndef _UTILS_H
#define _UTILS_H

#include <sys/utsname.h>

#include <algorithm>
#include <regex>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/functional/hash.hpp>

#include "rapidxml_ns.hpp"
#include "rapidxml_ns_print.hpp"

#include <http/constant.h>

#include "fmt/format.h"

namespace upnp {

static const std::string LOGGER = "upnp";
static const std::string VERSION = UPNP_VERSION;

static const std::string NS_ROOT = "root";
static const std::string NS_ROOT_DEVICE = "upnp:rootdevice";
static const std::string NS_MEDIASERVER = "urn:schemas-upnp-org:device:MediaServer:1";
static const std::string NS_CONTENT_DIRECTORY = "urn:schemas-upnp-org:service:ContentDirectory:1";
static const std::string NS_CONNECTION_MANAGER = "urn:schemas-upnp-org:service:ConnectionManager:1";
static const std::string NS_MEDIA_RECEIVER_REGISTRAR = "urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1";

/** @brief Object ID */
static const  std::string OBJECT_ID = "ObjectID";
static const  std::string BROWSE_FLAG = "BrowseFlag";
static const  std::string BROWSE_METADATA = "BrowseMetadata";
static const  std::string BROWSE_DIRECT_CHILDREN = "BrowseDirectChildren";
static const  std::string BROWSE_RESPONSE = "BrowseResponse";

/** @brief DIDL root node name */
const static std::string DIDL_ROOT_NODE = "DIDL-Lite";
/** @brief DIDL container name */
const static std::string DIDL_ELEMENT_CONTAINER = "container";
/** @brief DIDL item name */
const static std::string DIDL_ELEMENT_ITEM = "item";
/** @brief DIDL element class */
const static std::string DIDL_ELEMENT_CLASS = "class";
/** @brief DIDL TITLE */
const static std::string DIDL_ELEMENT_TITLE = "title";
/** @brief DIDL id attribute name */
const static std::string DIDL_ATTRIBUTE_ID = "id";
/** @brief DIDL parent id attribute name */
const static std::string DIDL_ATTRIBUTE_PARENT_ID = "parentID";
/** @brief DIDL restricted attribute name */
const static std::string DIDL_ATTRIBUTE_RESTRICTED = "restricted";
/** @brief DIDL child count attribute name */
const static std::string DIDL_ATTRIBUTE_CHILD_COUNT = "childCount";
/** @brief DIDL album art uri */
const static std::string DIDL_ALBUM_ART_URI = "albumArtURI";

/** @brief XML DIDL NAMESPACE */
const static std::string XML_NS_DIDL = "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/";
/** @brief XML PURL NAMESPACE */
const static std::string XML_NS_PURL = "http://purl.org/dc/elements/1.1/";
/** @brief XML DLNA NAMESPACE */
const static std::string XML_NS_DLNA = "urn:schemas-dlna-org:metadata-1-0/";
/** @brief XML DLNA Metadata NAMESPACE */
const static std::string XML_NS_DLNA_METADATA = "urn:schemas-dlna-org:metadata-1-0/";
/** @brief XML PV NAMESPACE */
const static std::string XML_NS_PV = "http://www.pv.com/pvns/";
/** @brief XML UPNP NAMESPACE */
const static std::string XML_NS_UPNP = "urn:schemas-upnp-org:metadata-1-0/upnp/";

/** @brief Results returned */
const static std::string NUMBER_RESULT = "NumberReturned";
/** @brief total matches */
const static std::string TOTAL_MATCHES = "TotalMatches";
/** @brief cds update id */
const static std::string UPDATE_ID = "UpdateID";

template< class N, class Fn >
inline void FOR( const std::string& local_name, N root_node, Fn fn ) {
    for( rapidxml_ns::xml_node<>* __r_sieblings = root_node->first_node();
         __r_sieblings;
         __r_sieblings = __r_sieblings->next_sibling() ) {

        if( strcmp("*", local_name.c_str() ) == 0 || strcmp( __r_sieblings->local_name(), local_name.c_str() ) == 0 )
        { fn( __r_sieblings ); }
    }
}

inline std::string uname() {
    struct utsname uts;
    uname ( &uts );
    std::ostringstream system;
    system << uts.sysname << "/" << uts.version;
    return system.str();
};

/** @brief The SSDP Response */
struct SsdpResponse {

    SsdpResponse ( http::http_status status, std::string request_line, std::map< std::string, std::string > headers ) :
        status ( status ), request_line ( request_line ), headers ( headers ) {}

    http::http_status status;
    std::string request_line;
    std::map< std::string, std::string > headers;
};

inline std::string create_header ( std::string request_line, std::map< std::string, std::string > headers ) {
    std::ostringstream os;
    os << request_line + std::string ( "\r\n" );

    for ( auto & iter : headers ) {
        os << iter.first << ": " << iter.second << "\r\n";
    }

    os << "\r\n";
    return os.str();
}

/** @brief SSDP event item. */
struct SsdpEvent {
public:

    SsdpEvent ( const SsdpEvent& ) = default;
    SsdpEvent ( SsdpEvent&& ) = default;
    SsdpEvent& operator= ( const SsdpEvent& ) = default;
    SsdpEvent& operator= ( SsdpEvent&& ) = default;
    ~SsdpEvent() {}

    /**
      * Create the json stream.
      */
    friend std::ostream& operator<< ( std::ostream& out, const SsdpEvent & upnp_device ) {
            out << "{\"host\":\"" << upnp_device.host << "\",\"location\":\"" << upnp_device.location << "\",\"nt\":\"" << upnp_device.nt << "\"," <<
                    "\"nts\":\"" <<  upnp_device.nts << "\",\"server\":\"" << upnp_device.server << "\",\"usn\":\"" << upnp_device.usn << "\"," <<
                    "\"last_seen\":" << upnp_device.last_seen << ",\"cache_control\":" << upnp_device.cache_control << "}";
            return out;
    }

    std::string host;
    std::string location;
    std::string nt;
    std::string nts;
    std::string server;
    std::string usn;

    time_t last_seen;
    time_t cache_control;
};

template< class T >
inline rapidxml_ns::xml_node<>* element( rapidxml_ns::xml_document<>* doc, T* parent, const std::string& name, const std::string& value ) {
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

static std::string soap_envelope( const std::string& body, int results, int total ) {
    rapidxml_ns::xml_document<> doc;

    auto root_n = element<rapidxml_ns::xml_node<>>( &doc, &doc, "s:Envelope", "" );
    attr( &doc, root_n, "xmlns:s", "http://schemas.xmlsoap.org/soap/envelope/" );
    attr( &doc, root_n, "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" );
    attr( &doc, root_n, "s:encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/" );

    auto body_n = element<rapidxml_ns::xml_node<>>( &doc, root_n, "s:Body", "" );
    auto _browseResponse_n = element<rapidxml_ns::xml_node<>>( &doc, body_n, "u:BrowseResponse", "" );
    attr( &doc, _browseResponse_n, "xmlns:u", "urn:schemas-upnp-org:service:ContentDirectory:1" );

    auto _result_n = element<rapidxml_ns::xml_node<>>( &doc, _browseResponse_n, "Result", body );
    attr( &doc, _result_n, "xsi:type", "xsd:string" );

    auto _number_result_n = element<rapidxml_ns::xml_node<>>( &doc, _browseResponse_n, NUMBER_RESULT, std::to_string( results ) );
    attr( &doc, _number_result_n, "xsi:type", "xsd:int" );
    auto _total_n = element<rapidxml_ns::xml_node<>>( &doc, _browseResponse_n, TOTAL_MATCHES, std::to_string( total ) );
    attr( &doc, _total_n, "xsi:type", "xsd:int" );
    auto _update_id_n = element<rapidxml_ns::xml_node<>>( &doc, _browseResponse_n, UPDATE_ID, "1" ); //TODO
    attr( &doc, _update_id_n, "xsi:type", "xsd:int" );

    //write xml to string
    std::string s = "<?xml version=\"1.0\"?>\n";
    rapidxml_ns::print( std::back_inserter(s), doc, rapidxml_ns::print_no_indenting );
    return s;
}

static const char* CDS_DESCRIPTION = R"xml(<?xml version="1.0"?>
<scpd xmlns="urn:schemas-upnp-org:service-1-0"><specVersion><major>1</major><minor>0</minor></specVersion><actionList><action><name>GetSearchCapabilities</name><argumentList><argument><name>SearchCaps</name><direction>out</direction><relatedStateVariable>SearchCapabilities</relatedStateVariable></argument></argumentList></action><action><name>GetSortCapabilities</name><argumentList><argument><name>SortCaps</name><direction>out</direction><relatedStateVariable>SortCapabilities</relatedStateVariable></argument></argumentList></action><action><name>GetSystemUpdateID</name><argumentList><argument><name>Id</name><direction>out</direction><relatedStateVariable>SystemUpdateID</relatedStateVariable></argument></argumentList></action><action><name>Browse</name><argumentList><argument><name>ObjectID</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable></argument><argument><name>BrowseFlag</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_BrowseFlag</relatedStateVariable></argument><argument><name>Filter</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_Filter</relatedStateVariable></argument><argument><name>StartingIndex</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_Index</relatedStateVariable></argument><argument><name>RequestedCount</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable></argument><argument><name>SortCriteria</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_SortCriteria</relatedStateVariable></argument><argument><name>Result</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable></argument><argument><name>NumberReturned</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable></argument><argument><name>TotalMatches</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable></argument><argument><name>UpdateID</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_UpdateID</relatedStateVariable></argument></argumentList></action><action><name>Search</name><argumentList><argument><name>ContainerID</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable></argument><argument><name>SearchCriteria</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_SearchCriteria</relatedStateVariable></argument><argument><name>Filter</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_Filter</relatedStateVariable></argument><argument><name>StartingIndex</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_Index</relatedStateVariable></argument><argument><name>RequestedCount</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable></argument><argument><name>SortCriteria</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_SortCriteria</relatedStateVariable></argument><argument><name>Result</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable></argument><argument><name>NumberReturned</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable></argument><argument><name>TotalMatches</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable></argument><argument><name>UpdateID</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_UpdateID</relatedStateVariable></argument></argumentList></action></actionList><serviceStateTable><stateVariable sendEvents="yes"><name>TransferIDs</name><dataType>string</dataType></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_ObjectID</name><dataType>string</dataType></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_Result</name><dataType>string</dataType></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_SearchCriteria</name><dataType>string</dataType></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_BrowseFlag</name><dataType>string</dataType><allowedValueList><allowedValue>BrowseMetadata</allowedValue><allowedValue>BrowseDirectChildren</allowedValue></allowedValueList></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_Filter</name><dataType>string</dataType></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_SortCriteria</name><dataType>string</dataType></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_Index</name><dataType>ui4</dataType></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_Count</name><dataType>ui4</dataType></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_UpdateID</name><dataType>ui4</dataType></stateVariable><stateVariable sendEvents="no"><name>SearchCapabilities</name><dataType>string</dataType></stateVariable><stateVariable sendEvents="no"><name>SortCapabilities</name><dataType>string</dataType></stateVariable><stateVariable sendEvents="yes"><name>SystemUpdateID</name><dataType>ui4</dataType></stateVariable></serviceStateTable></scpd>)xml";

static const char* CMS_DESCRIPTION = R"xml(<?xml version="1.0"?>
<scpd xmlns="urn:schemas-upnp-org:service-1-0"><specVersion><major>1</major><minor>0</minor></specVersion><actionList><action><name>GetProtocolInfo</name><argumentList><argument><name>Source</name><direction>out</direction><relatedStateVariable>SourceProtocolInfo</relatedStateVariable></argument><argument><name>Sink</name><direction>out</direction><relatedStateVariable>SinkProtocolInfo</relatedStateVariable></argument></argumentList></action><action><name>GetCurrentConnectionIDs</name><argumentList><argument><name>ConnectionIDs</name><direction>out</direction><relatedStateVariable>CurrentConnectionIDs</relatedStateVariable></argument></argumentList></action><action><name>GetCurrentConnectionInfo</name><argumentList><argument><name>ConnectionID</name><direction>in</direction><relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable></argument><argument><name>RcsID</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_RcsID</relatedStateVariable></argument><argument><name>AVTransportID</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_AVTransportID</relatedStateVariable></argument><argument><name>ProtocolInfo</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_ProtocolInfo</relatedStateVariable></argument><argument><name>PeerConnectionManager</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_ConnectionManager</relatedStateVariable></argument><argument><name>PeerConnectionID</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable></argument><argument><name>Direction</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_Direction</relatedStateVariable></argument><argument><name>Status</name><direction>out</direction><relatedStateVariable>A_ARG_TYPE_ConnectionStatus</relatedStateVariable></argument></argumentList></action></actionList><serviceStateTable><stateVariable sendEvents="yes"><name>SourceProtocolInfo</name><dataType>string</dataType></stateVariable><stateVariable sendEvents="yes"><name>SinkProtocolInfo</name><dataType>string</dataType></stateVariable><stateVariable sendEvents="yes"><name>CurrentConnectionIDs</name><dataType>string</dataType></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_ConnectionStatus</name><dataType>string</dataType><allowedValueList><allowedValue>OK</allowedValue><allowedValue>ContentFormatMismatch</allowedValue><allowedValue>InsufficientBandwidth</allowedValue><allowedValue>UnreliableChannel</allowedValue><allowedValue>Unknown</allowedValue></allowedValueList></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_ConnectionManager</name><dataType>string</dataType></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_Direction</name><dataType>string</dataType><allowedValueList><allowedValue>Input</allowedValue><allowedValue>Output</allowedValue></allowedValueList></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_ProtocolInfo</name><dataType>string</dataType></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_ConnectionID</name><dataType>i4</dataType></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_AVTransportID</name><dataType>i4</dataType></stateVariable><stateVariable sendEvents="no"><name>A_ARG_TYPE_RcsID</name><dataType>i4</dataType></stateVariable></serviceStateTable></scpd>)xml";

}//namespace upnp
#endif // _UTILS_H
