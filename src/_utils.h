#ifndef _UTILS_H
#define _UTILS_H

#include <algorithm>
#include <regex>
#include <map>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/functional/hash.hpp>

#include <http/constant.h>

#include "fmt/format.h"

namespace upnp {

static const std::string LOGGER = "upnp";

static const std::string NS_ROOT_DEVICE = "upnp:rootdevice";
static const std::string NS_MEDIASERVER = "urn:schemas-upnp-org:device:MediaServer:1";
static const std::string NS_CONTENT_DIRECTORY = "urn:schemas-upnp-org:service:ContentDirectory:1";
static const std::string NS_CONNECTION_MANAGER = "urn:schemas-upnp-org:service:ConnectionManager:1";
static const std::string NS_MEDIA_RECEIVER_REGISTRAR = "urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1";

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

static const char* CDS_DESCRIPTION = R"xml(<?xml version="1.0" encoding="utf-8"?>
<scpd xmlns="urn:schemas-upnp-org:service-1-0">
 <specVersion>
    <major>1</major>
    <minor>0</minor>
 </specVersion>
 <actionList>
    <action>
       <name>Browse</name>
       <argumentList>
          <argument>
             <name>ObjectID</name>
             <direction>in</direction>
             <relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>
          </argument>
          <argument>
             <name>BrowseFlag</name>
             <direction>in</direction>
             <relatedStateVariable>A_ARG_TYPE_BrowseFlag</relatedStateVariable>
          </argument>
          <argument>
             <name>Filter</name>
             <direction>in</direction>
             <relatedStateVariable>A_ARG_TYPE_Filter</relatedStateVariable>
          </argument>
          <argument>
             <name>StartingIndex</name>
             <direction>in</direction>
             <relatedStateVariable>A_ARG_TYPE_Index</relatedStateVariable>
          </argument>
          <argument>
             <name>RequestedCount</name>
             <direction>in</direction>
             <relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>
          </argument>
          <argument>
             <name>SortCriteria</name>
             <direction>in</direction>
             <relatedStateVariable>A_ARG_TYPE_SortCriteria</relatedStateVariable>
          </argument>
          <argument>
             <name>Result</name>
             <direction>out</direction>
             <relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>
          </argument>
          <argument>
             <name>NumberReturned</name>
             <direction>out</direction>
             <relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>
          </argument>
          <argument>
             <name>TotalMatches</name>
             <direction>out</direction>
             <relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>
          </argument>
          <argument>
             <name>UpdateID</name>
             <direction>out</direction>
             <relatedStateVariable>A_ARG_TYPE_UpdateID</relatedStateVariable>
          </argument>
       </argumentList>
    </action>
    <action>
       <name>GetSearchCapabilities</name>
       <argumentList>
          <argument>
             <name>SearchCaps</name>
             <direction>out</direction>
             <relatedStateVariable>SearchCapabilities</relatedStateVariable>
          </argument>
       </argumentList>
    </action>
    <action>
       <name>GetSortCapabilities</name>
       <argumentList>
          <argument>
             <name>SortCaps</name>
             <direction>out</direction>
             <relatedStateVariable>SortCapabilities</relatedStateVariable>
          </argument>
       </argumentList>
    </action>
    <action>
       <name>GetSystemUpdateID</name>
       <argumentList>
          <argument>
             <name>Id</name>
             <direction>out</direction>
             <relatedStateVariable>SystemUpdateID</relatedStateVariable>
          </argument>
       </argumentList>
    </action>
 </actionList>
 <serviceStateTable>
    <stateVariable sendEvents="no">
       <name>A_ARG_TYPE_BrowseFlag</name>
       <dataType>string</dataType>
       <allowedValueList>
          <allowedValue>BrowseMetadata</allowedValue>
          <allowedValue>BrowseDirectChildren</allowedValue>
       </allowedValueList>
    </stateVariable>
    <stateVariable sendEvents="no">
       <name>SystemUpdateID</name>
       <dataType>ui4</dataType>
    </stateVariable>
    <stateVariable sendEvents="no">
       <name>ContainerUpdateIDs</name>
       <dataType>string</dataType>
    </stateVariable>
    <stateVariable sendEvents="no">
       <name>A_ARG_TYPE_Count</name>
       <dataType>ui4</dataType>
    </stateVariable>
    <stateVariable sendEvents="no">
       <name>A_ARG_TYPE_SortCriteria</name>
       <dataType>string</dataType>
    </stateVariable>
    <stateVariable sendEvents="no">
       <name>SortCapabilities</name>
       <dataType>string</dataType>
    </stateVariable>
    <stateVariable sendEvents="no">
       <name>A_ARG_TYPE_Index</name>
       <dataType>ui4</dataType>
    </stateVariable>
    <stateVariable sendEvents="no">
       <name>A_ARG_TYPE_ObjectID</name>
       <dataType>string</dataType>
    </stateVariable>
    <stateVariable sendEvents="no">
       <name>A_ARG_TYPE_UpdateID</name>
       <dataType>ui4</dataType>
    </stateVariable>
    <stateVariable sendEvents="no">
       <name>A_ARG_TYPE_Result</name>
       <dataType>string</dataType>
    </stateVariable>
    <stateVariable sendEvents="no">
       <name>SearchCapabilities</name>
       <dataType>string</dataType>
    </stateVariable>
    <stateVariable sendEvents="no">
       <name>A_ARG_TYPE_Filter</name>
       <dataType>string</dataType>
    </stateVariable>
 </serviceStateTable>
</scpd>)xml";

static const char* CMS_DESCRIPTION = R"xml(<?xml version="1.0" encoding="utf-8"?>
<scpd xmlns="urn:schemas-upnp-org:service-1-0">
<specVersion>
   <major>1</major>
   <minor>0</minor>
</specVersion>
<actionList>
   <action>
      <name>GetCurrentConnectionIDs</name>
      <argumentList>
         <argument>
            <name>ConnectionIDs</name>
            <direction>out</direction>
            <relatedStateVariable>CurrentConnectionIDs</relatedStateVariable>
         </argument>
      </argumentList>
   </action>
   <action>
      <name>GetCurrentConnectionInfo</name>
      <argumentList>
         <argument>
            <name>ConnectionID</name>
            <direction>in</direction>
            <relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>
         </argument>
         <argument>
            <name>RcsID</name>
            <direction>out</direction>
            <relatedStateVariable>A_ARG_TYPE_RcsID</relatedStateVariable>
         </argument>
         <argument>
            <name>AVTransportID</name>
            <direction>out</direction>
            <relatedStateVariable>A_ARG_TYPE_AVTransportID</relatedStateVariable>
         </argument>
         <argument>
            <name>ProtocolInfo</name>
            <direction>out</direction>
            <relatedStateVariable>A_ARG_TYPE_ProtocolInfo</relatedStateVariable>
         </argument>
         <argument>
            <name>PeerConnectionManager</name>
            <direction>out</direction>
            <relatedStateVariable>A_ARG_TYPE_ConnectionManager</relatedStateVariable>
         </argument>
         <argument>
            <name>PeerConnectionID</name>
            <direction>out</direction>
            <relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>
         </argument>
         <argument>
            <name>Direction</name>
            <direction>out</direction>
            <relatedStateVariable>A_ARG_TYPE_Direction</relatedStateVariable>
         </argument>
         <argument>
            <name>Status</name>
            <direction>out</direction>
            <relatedStateVariable>A_ARG_TYPE_ConnectionStatus</relatedStateVariable>
         </argument>
      </argumentList>
   </action>
   <action>
      <name>GetProtocolInfo</name>
      <argumentList>
         <argument>
            <name>Source</name>
            <direction>out</direction>
            <relatedStateVariable>SourceProtocolInfo</relatedStateVariable>
         </argument>
         <argument>
            <name>Sink</name>
            <direction>out</direction>
            <relatedStateVariable>SinkProtocolInfo</relatedStateVariable>
         </argument>
      </argumentList>
   </action>
</actionList>
<serviceStateTable>
   <stateVariable sendEvents="no">
      <name>A_ARG_TYPE_ProtocolInfo</name>
      <dataType>string</dataType>
   </stateVariable>
   <stateVariable sendEvents="no">
      <name>A_ARG_TYPE_ConnectionStatus</name>
      <dataType>string</dataType>
      <allowedValueList>
         <allowedValue>OK</allowedValue>
         <allowedValue>ContentFormatMismatch</allowedValue>
         <allowedValue>InsufficientBandwidth</allowedValue>
         <allowedValue>UnreliableChannel</allowedValue>
         <allowedValue>Unknown</allowedValue>
      </allowedValueList>
   </stateVariable>
   <stateVariable sendEvents="no">
      <name>A_ARG_TYPE_AVTransportID</name>
      <dataType>i4</dataType>
   </stateVariable>
   <stateVariable sendEvents="no">
      <name>A_ARG_TYPE_RcsID</name>
      <dataType>i4</dataType>
   </stateVariable>
   <stateVariable sendEvents="no">
      <name>A_ARG_TYPE_ConnectionID</name>
      <dataType>i4</dataType>
   </stateVariable>
   <stateVariable sendEvents="no">
      <name>A_ARG_TYPE_ConnectionManager</name>
      <dataType>string</dataType>
   </stateVariable>
   <stateVariable sendEvents="yes">
      <name>SourceProtocolInfo</name>
      <dataType>string</dataType>
   </stateVariable>
   <stateVariable sendEvents="yes">
      <name>SinkProtocolInfo</name>
      <dataType>string</dataType>
   </stateVariable>
   <stateVariable sendEvents="no">
      <name>A_ARG_TYPE_Direction</name>
      <dataType>string</dataType>
      <allowedValueList>
         <allowedValue>Input</allowedValue>
         <allowedValue>Output</allowedValue>
      </allowedValueList>
   </stateVariable>
   <stateVariable sendEvents="yes">
      <name>CurrentConnectionIDs</name>
      <dataType>string</dataType>
   </stateVariable>
</serviceStateTable>
</scpd>)xml";
}//namespace upnp
#endif // _UTILS_H
