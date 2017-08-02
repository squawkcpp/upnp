#include <string>
#include <map>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

#include "../src/server.h"

TEST(UpnpParseTest, parse_request) {

    std::string request = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?><s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><u:GetProtocolInfo xmlns:u=\"urn:schemas-upnp-org:service:ConnectionManager:1\"/></s:Body></s:Envelope>";
    upnp::UpnpCommand upnp_command = upnp::Server::parse( request );
    EXPECT_EQ( upnp::UpnpCommand::GET_PROTOCOL_INFO, upnp_command.type);
}

TEST(UpnpParseTest, description ) {

    std::stringstream _ss;
    std::map< std::string, std::string > _request;
    upnp::Config _c;
//TODO    upnp::Server::description( _c, _ss );
    EXPECT_EQ( "", _ss.str() );
}

