
#include <gtest/gtest.h>

#include "../src/_utils.h"

namespace upnp {

#define XML_SOAP_ENVELOPE "<?xml version=\"1.0\"?>\n<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:BrowseResponse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\"><Result xsi:type=\"xsd:string\"/><NumberReturned xsi:type=\"xsd:int\">0</NumberReturned><TotalMatches xsi:type=\"xsd:int\">0</TotalMatches><UpdateID xsi:type=\"xsd:int\">1</UpdateID></u:BrowseResponse></s:Body></s:Envelope>"

TEST(UtilsTest, soap_envelope) {
    EXPECT_EQ( XML_SOAP_ENVELOPE, soap_envelope( "", 0, 0 ) );
}


}//namespace upnp
