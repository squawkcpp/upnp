#include <string>
#include <map>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

#include "../src/_utils.h"
#include "../src/server.h"
#include "../src/device.h"

TEST(UpnpParseTest, parse_request) {

    std::string request = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?><s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><u:GetProtocolInfo xmlns:u=\"urn:schemas-upnp-org:service:ConnectionManager:1\"/></s:Body></s:Envelope>";
    upnp::UpnpCommand upnp_command = upnp::Server::parse( request );
    EXPECT_EQ( upnp::UpnpCommand::GET_PROTOCOL_INFO, upnp_command.type);
}

TEST(UpnpParseTest, command_browse ) {

    std::string _request = R"xml(<?xml version="1.0" encoding="utf-8" standalone="yes"?><s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:Browse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1"><ObjectID>0</ObjectID><BrowseFlag>BrowseDirectChildren</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>16</RequestedCount><SortCriteria></SortCriteria></u:Browse></s:Body></s:Envelope>)xml";

    upnp::UpnpCommand _command = upnp::Server::parse( _request );
    EXPECT_EQ( upnp::UpnpCommand::BROWSE, _command.type );
    EXPECT_EQ( 6, _command.values.size() );
    EXPECT_EQ( "0", _command.get( "ObjectID" ) );
    EXPECT_EQ( "BrowseDirectChildren", _command.get( "BrowseFlag" ) );
    EXPECT_EQ( "16", _command.get( "RequestedCount" ) );
}

TEST( UpnpTest, ParseRequest ) {
/*
<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns0="urn:schemas-upnp-org:service:ContentDirectory:1" s:encodingStyle=XML_NS_SOAPENC>
   <s:Body>
      <ns0:Browse>
         <ObjectID>0</ObjectID>
         <BrowseFlag>BrowseMetadata</BrowseFlag>
         <Filter>*</Filter>
         <StartingIndex>0</StartingIndex>
         <RequestedCount>0</RequestedCount>
         <SortCriteria />
      </ns0:Browse>
   </s:Body>
</s:Envelope>
*/
    const char * request = "<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:ns0=\"urn:schemas-upnp-org:service:ContentDirectory:1\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><ns0:Browse><ObjectID>0</ObjectID><BrowseFlag>BrowseMetadata</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>0</RequestedCount><SortCriteria /></ns0:Browse></s:Body></s:Envelope>";
    upnp::UpnpCommand command = upnp::Server::parse( request );
    EXPECT_EQ( upnp::UpnpCommand::BROWSE, command.type );

    EXPECT_EQ( 6U, command.values.size() );
    EXPECT_EQ( "0", command.get( "ObjectID" ) );
    EXPECT_EQ( "BrowseMetadata", command.get( "BrowseFlag" ) );
    EXPECT_EQ( "*", command.get( "Filter" ) );
    EXPECT_EQ( "0", command.get( "StartingIndex" ) );
    EXPECT_EQ( "0", command.get( "RequestedCount" ) );
    EXPECT_EQ( "", command.get( "SortCriteria" ) );
}
TEST( UpnpTest, ParseRequest2 ) {
/*
 *<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle=" h t t p ://schemas.xmlsoap.org/soap/encoding/">
   <s:Body>
      <u:Browse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1">
         <ObjectID>0</ObjectID>
         <BrowseFlag>BrowseDirectChildren</BrowseFlag>
         <Filter>*</Filter>
         <StartingIndex>0</StartingIndex>
         <RequestedCount>16</RequestedCount>
         <SortCriteria />
      </u:Browse>
   </s:Body>
</s:Envelope>
*/
    const char * request = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?><s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><u:Browse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\"><ObjectID>0</ObjectID><BrowseFlag>BrowseDirectChildren</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>16</RequestedCount><SortCriteria></SortCriteria></u:Browse></s:Body></s:Envelope>";

    upnp::UpnpCommand _command = upnp::Server::parse( request );
    EXPECT_EQ( upnp::UpnpCommand::BROWSE, _command.type );

    EXPECT_TRUE( _command.contains( "ObjectID" ) );
    EXPECT_EQ( _command.get( "BrowseFlag" ), "BrowseDirectChildren" );
}
TEST( UpnpTest, ParseRequestStdout ) {
/*
 *<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="h t t p ://schemas.xmlsoap.org/soap/encoding/">
   <s:Body>
      <u:Browse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1">
         <ObjectID>0</ObjectID>
         <BrowseFlag>BrowseDirectChildren</BrowseFlag>
         <Filter>*</Filter>
         <StartingIndex>0</StartingIndex>
         <RequestedCount>16</RequestedCount>
         <SortCriteria />
      </u:Browse>
   </s:Body>
</s:Envelope>
*/
    const char * request = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?><s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><u:Browse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\"><ObjectID>0</ObjectID><BrowseFlag>BrowseDirectChildren</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>16</RequestedCount><SortCriteria></SortCriteria></u:Browse></s:Body></s:Envelope>";
    const char * response = "{\"type\":\"BrowseFlag\", \"BrowseFlag\": \"BrowseDirectChildren\", \"Filter\": \"*\", \"ObjectID\": \"0\", \"RequestedCount\": \"16\", \"SortCriteria\": \"\", \"StartingIndex\": \"0\"}";

    upnp::UpnpCommand _command = upnp::Server::parse( request );
    std::stringstream out;
    out << _command;
    EXPECT_EQ( std::string( response ), out.str() );
}
TEST( UpnpTest, ParseRequest3 ) {

    const char * request = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?><s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><u:Browse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\"><ObjectID>0</ObjectID><BrowseFlag>BrowseDirectChildren</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>50</RequestedCount><SortCriteria></SortCriteria></u:Browse></s:Body></s:Envelope>";
    const char * response = "{\"type\":\"BrowseFlag\", \"BrowseFlag\": \"BrowseDirectChildren\", \"Filter\": \"*\", \"ObjectID\": \"0\", \"RequestedCount\": \"50\", \"SortCriteria\": \"\", \"StartingIndex\": \"0\"}";

    upnp::UpnpCommand _command = upnp::Server::parse( request );
    std::stringstream out;
    out << _command;
    EXPECT_EQ( std::string( response ), out.str() );
}
#define DESCRIPTION "<?xml version=\"1.0\"?>"\
    "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"\
    "<specVersion>"\
    "<major>1</major>"\
    "<minor>0</minor>"\
    "</specVersion>"\
    "<device>"\
    "<deviceType>urn:schemas-upnp-org:device:MediaServer:1</deviceType>"\
    "<presentationURL>http://192.168.0.8:49152/</presentationURL>"\
    "<friendlyName>MediaTomb</friendlyName>"\
    "<manufacturer>(c) 2005-2008 Gena Batyan &lt;bgeradz@mediatomb.cc&gt;, Sergey Bostandzhyan &lt;jin@mediatomb.cc&gt;, Leonhard Wimmer &lt;leo@mediatomb.cc&gt;</manufacturer>"\
    "<manufacturerURL>http://mediatomb.cc/</manufacturerURL>"\
    "<modelDescription>Free UPnP AV MediaServer, GNU GPL</modelDescription>"\
    "<modelName>MediaTomb</modelName>"\
    "<modelNumber>0.12.1</modelNumber>"\
    "<serialNumber>1</serialNumber>"\
    "<UDN>uuid:77c7acd6-5257-4c82-971d-03f9e4e2c91a</UDN>"\
    "<iconList>"\
    "<icon>"\
    "<mimetype>image/png</mimetype>"\
    "<width>120</width>"\
    "<height>120</height>"\
    "<depth>24</depth>"\
    "<url>/icons/mt-icon120.png</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/x-ms-bmp</mimetype>"\
    "<width>120</width>"\
    "<height>120</height>"\
    "<depth>24</depth>"\
    "<url>/icons/mt-icon120.bmp</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/jpeg</mimetype>"\
    "<width>120</width>"\
    "<height>120</height>"\
    "<depth>24</depth>"\
    "<url>/icons/mt-icon120.jpg</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/png</mimetype>"\
    "<width>48</width>"\
    "<height>48</height>"\
    "<depth>24</depth>"\
    "<url>/icons/mt-icon48.png</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/x-ms-bmp</mimetype>"\
    "<width>48</width>"\
    "<height>48</height>"\
    "<depth>24</depth>"\
    "<url>/icons/mt-icon48.bmp</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/jpeg</mimetype>"\
    "<width>48</width>"\
    "<height>48</height>"\
    "<depth>24</depth>"\
    "<url>/icons/mt-icon48.jpg</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/png</mimetype>"\
    "<width>32</width>"\
    "<height>32</height>"\
    "<depth>8</depth>"\
    "<url>/icons/mt-icon32.png</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/x-ms-bmp</mimetype>"\
    "<width>32</width>"\
    "<height>32</height>"\
    "<depth>8</depth>"\
    "<url>/icons/mt-icon32.bmp</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/jpeg</mimetype>"\
    "<width>32</width>"\
    "<height>32</height>"\
    "<depth>8</depth>"\
    "<url>/icons/mt-icon32.jpg</url>"\
    "</icon>"\
    "</iconList>"\
    "<serviceList>"\
    "<service>"\
    "<serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>"\
    "<serviceId>urn:upnp-org:serviceId:ConnectionManager</serviceId>"\
    "<SCPDURL>cm.xml</SCPDURL>"\
    "<controlURL>/upnp/control/cm</controlURL>"\
    "<eventSubURL>/upnp/event/cm</eventSubURL>"\
    "</service>"\
    "<service>"\
    "<serviceType>urn:schemas-upnp-org:service:ContentDirectory:1</serviceType>"\
    "<serviceId>urn:upnp-org:serviceId:ContentDirectory</serviceId>"\
    "<SCPDURL>cds.xml</SCPDURL>"\
    "<controlURL>/upnp/control/cds</controlURL>"\
    "<eventSubURL>/upnp/event/cds</eventSubURL>"\
    "</service>"\
    "</serviceList>"\
    "</device>"\
    "<URLBase>http://192.168.0.8:49152/</URLBase>"\
    "</root>"

TEST( UpnpTest, UpnpDeviceParse ) {

    upnp::Device device;
    upnp::parse( DESCRIPTION, device );

    EXPECT_EQ( device.versionMajor(), 1 );
    EXPECT_EQ( device.versionMinor(), 0 );

    EXPECT_EQ( device.deviceType(), "urn:schemas-upnp-org:device:MediaServer:1" );
    EXPECT_EQ( device.presentationUrl(), "http://192.168.0.8:49152/" );
    EXPECT_EQ( device.friendlyName(), "MediaTomb" );
    EXPECT_EQ( device.manufacturer(), "(c) 2005-2008 Gena Batyan <bgeradz@mediatomb.cc>, Sergey Bostandzhyan <jin@mediatomb.cc>, Leonhard Wimmer <leo@mediatomb.cc>" );
    EXPECT_EQ( device.manufacturerUrl(), "http://mediatomb.cc/" );
    EXPECT_EQ( device.modelDescription(), "Free UPnP AV MediaServer, GNU GPL" );
    EXPECT_EQ( device.modelName(), "MediaTomb" );
    EXPECT_EQ( device.modelNumber(), "0.12.1" );
    EXPECT_EQ( device.modelUrl(), "" );
    EXPECT_EQ( device.serialNumber(), "1" );
    EXPECT_EQ( device.udn(), "uuid:77c7acd6-5257-4c82-971d-03f9e4e2c91a" );
    EXPECT_EQ( device.upc(), "" );
    EXPECT_EQ( device.urlBase(), "http://192.168.0.8:49152/" );

    EXPECT_EQ( device.iconList().size(), 9U );
    EXPECT_EQ( device.iconList()[0].mimeType(), "image/png" );
    EXPECT_EQ( device.iconList()[0].width(), 120 );
    EXPECT_EQ( device.iconList()[0].height(), 120 );
    EXPECT_EQ( device.iconList()[0].depth(), 24 );
    EXPECT_EQ( device.iconList()[0].url(), "/icons/mt-icon120.png" );

    EXPECT_EQ( device.serviceList().size(), 2U );
    EXPECT_EQ( device.serviceList()[1].serviceType(), "urn:schemas-upnp-org:service:ContentDirectory:1" );
    EXPECT_EQ( device.serviceList()[1].serviceId(), "urn:upnp-org:serviceId:ContentDirectory" );
    EXPECT_EQ( device.serviceList()[1].scpdUrl(), "cds.xml" );
    EXPECT_EQ( device.serviceList()[1].controlUrl(), "/upnp/control/cds" );
    EXPECT_EQ( device.serviceList()[1].eventSubUrl(), "/upnp/event/cds" );
}
