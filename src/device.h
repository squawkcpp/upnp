#ifndef DEVICE_H
#define DEVICE_H

#include <chrono>
#include <string>
#include <vector>

#include "rapidxml_ns.hpp"

#include "_utils.h"

namespace upnp {
/**
 * @brief The Icon struct
 */
struct Icon {
public:
    Icon() : width_(0), height_(0), depth_(0) {}
    Icon ( const Icon& ) = default;
    Icon ( Icon&& ) = default;
    Icon& operator= ( const Icon& ) = default;
    Icon& operator= ( Icon&& ) = default;
    ~Icon() {}

    /**
     * @brief width
     * @return
     */
    int width() const { return width_; }
    /**
     * @brief width
     * @param width
     */
    void width( const int & width ) { width_ = width; }
    /**
     * @brief height
     * @return
     */
    int height() const { return height_; }
    /**
     * @brief height
     * @param height
     */
    void height( const int & height ) { height_ = height; }
    /**
     * @brief depth
     * @return
     */
    int depth() const { return depth_; }
    /**
     * @brief depth
     * @param depth
     */
    void depth( const int & depth ) { depth_ = depth; }
    /**
     * @brief mimeType
     * @return
     */
    std::string mimeType() const { return mimeType_; }
    /**
     * @brief mimeType
     * @param mimeType
     */
    void mimeType( const std::string & mimeType ) { mimeType_ = mimeType; }
    /**
     * @brief url
     * @return
     */
    std::string url() const { return url_; }
    /**
     * @brief url
     * @param url
     */
    void url( const std::string & url ) { url_ = url; }

private:
    int width_, height_, depth_;
    std::string mimeType_, url_;
};

struct Service {
public:
    Service() {}
    Service ( const Service& ) = default;
    Service ( Service&& ) = default;
    Service& operator= ( const Service& ) = default;
    Service& operator= ( Service&& ) = default;
    ~Service() {}

    std::string serviceType() const { return service_type_; }
    void serviceType( const std::string & service_type ) { service_type_ = service_type; }
    std::string serviceId() const { return service_id_; }
    void serviceId( const std::string & service_id ) { service_id_ = service_id; }
    std::string scpdUrl() const { return scpd_url_; }
    void scpdUrl( const std::string & scpd_url ) { scpd_url_ = scpd_url; }
    std::string controlUrl() const { return control_url_; }
    void controlUrl( const std::string & control_url ) { control_url_ = control_url; }
    std::string eventSubUrl() const { return event_sub_url_; }
    void eventSubUrl( const std::string & event_sub_url ) { event_sub_url_ = event_sub_url; }

private:
    std::string service_type_, service_id_, scpd_url_, control_url_, event_sub_url_;
};
/**
 * @brief The UpnpDevice item
 */
struct Device {
public:
    Device() : timestamp_( std::chrono::high_resolution_clock::now() ) {}
    Device ( const Device& ) = default;
    Device ( Device&& ) = default;
    Device& operator= ( const Device& ) = default;
    Device& operator= ( Device&& ) = default;
    ~Device() {}

    /**
     * Required. Major version of the UPnP Device Architecture. Shall be 2 for devices implemented
     * on a UPnP 2. 0 architecture.
     * @brief set version Major
     * @return
     */
    int versionMajor() const { return versionMajor_; }
    /**
     * @brief get version Major
     * @param versionMajor
     */
    void versionMajor( const int & versionMajor) { versionMajor_ = versionMajor; }
    /**
     * Required. Minor version of the UPnP Device Architecture. Shall be 0 for devices implemented
     * on a UPnP 2.0 architecture. Shall accurately reflect the version number of  the UPnP Device
     * Architecture supported by the device.
     * @brief get version Minor
     * @return
     */
    int versionMinor() const { return versionMinor_; }
    /**
     * @brief set version Minor
     * @param versionMinor
     */
    void versionMinor( const int & versionMinor) { versionMinor_ = versionMinor; }
    /**
     * Required. UPnP device type. Single URI.
     * <ul><li>For standard devices  defined by a UPnP Forum working committee, shall begin with
     * "urn :schemas -upnp-org:device:"  followed  by the standardized device type suffix, a colon,
     * and an integer device version i.e. urn:schemas-upnp-org:device:deviceType:ver. The
     * highest supported version of  the device type shall be specified.
     * </li><li>For non-standard devices  specified by UPnP vendors ,  shall begin with “urn:” , followed by
     * a  Vendor Doma in  Name, followed by “:device:”, followed by a  device type suffix, colon,
     * and an integer version, i.e., “urn:domain-name:device:deviceType:ver”. Period
     * characters in the Vendor Domain Name shall be replaced with hyphens in accordance
     * with RFC 2141. The highest supported version of the device type shall be specified.</li></ul>
     * The device type suffix defined by a UPnP Forum working committee or specif ied by a UPnP
     * vendor shall be <= 64 chars, not counting the version suffix and separating colon.
     * @brief device Type
     * @return
     */
    std::string deviceType() const { return deviceType_; }
    /**
     * @brief device Type
     * @param deviceType
     */
    void deviceType( const std::string & deviceType) { deviceType_ = deviceType; }
    /**
     * Required. Short description for end user. Specified by UPnP vendor. String.
     * Should be < 64 characters.
     * @brief friendlyName
     * @return
     */
    std::string friendlyName() const { return friendlyName_; }
    /**
     * @brief friendlyName
     * @param friendlyName
     */
    void friendlyName( const std::string & friendlyName) { friendlyName_ = friendlyName; }
    /**
     * Required.  Manufacturer's name. Specified by UPnP vendor. String. Should be < 64
     * characters.
     * @brief manufacturer
     * @return
     */
    std::string manufacturer() const { return manufacturer_; }
    /**
     * @brief manufacturer
     * @param manufacturer
     */
    void manufacturer( const std::string & manufacturer) { manufacturer_ = manufacturer; }
    /**
     * Allowed. Web site for Manufacturer. Single URL.
     * @brief manufacturerUrl
     * @return
     */
    std::string manufacturerUrl() const { return manufacturerUrl_; }
    /**
     * @brief manufacturerUrl
     * @param manufacturerUrl
     */
    void manufacturerUrl( const std::string & manufacturerUrl) { manufacturerUrl_ = manufacturerUrl; }
    /**
     * Recommended. Long description for end user. Specified by UPnP vendor. String.
     * Should be < 128 characters.
     * @brief modelDescription
     * @return
     */
    std::string modelDescription() const { return modelDescription_; }
    /**
     * @brief modelDescription
     * @param modelDescription
     */
    void modelDescription( const std::string & modelDescription) { modelDescription_ = modelDescription; }
    /**
     * Required. Model name. Specified by UPnP vendor. String. Should be < 32 characters.
     * @brief modelName
     * @return
     */
    std::string modelName() const { return modelName_; }
    /**
     * @brief modelName
     * @param modelName
     */
    void modelName( const std::string & modelName) { modelName_ = modelName; }
    /**
     * Recommended. Model number. Specified by UPnP vendor. String. Should be < 32 characters.
     * @brief modelNumber
     * @return
     */
    std::string modelNumber() const { return modelNumber_; }
    /**
     * @brief modelNumber
     * @param modelNumber
     */
    void modelNumber( const std::string & modelNumber) { modelNumber_ = modelNumber; }
    /**
     * Allowed. Web site for model. Specified by UPnP vendor. Single URL.
     * @brief modelUrl
     * @return
     */
    std::string modelUrl() const { return modelUrl_; }
    /**
     * @brief modelUrl
     * @param modelUrl
     */
    void modelUrl( const std::string & modelUrl) { modelUrl_ = modelUrl; }
    /**
     * Recommended. Serial number.  Specified by UPnP vendor. String. Should be < 64 characters.
     * @brief serialNumber
     * @return
     */
    std::string serialNumber() const { return serialNumber_; }
    /**
     * @brief serialNumber
     * @param serialNumber
     */
    void serialNumber( const std::string & serialNumber) { serialNumber_ = serialNumber; }
    /**
     * Required.  Unique Device Name. Universally- unique identifier for the device, whether  root or
     * embedded. shall be the same over time for a specific device instance ( i. e. , shall  survive
     * reboots) . shall match the field value of the NT header field in device discovery messages.
     * shall match the prefix of the USN header field in all disc overy messages. shall begin with
     * “uuid:” followed by a UUID suffix specified by a UPnP vendor.
     * @brief udn
     * @return
     */
    std::string udn() const { return udn_; }
    /**
     * @brief udn
     * @param udn
     */
    void udn( const std::string & udn) { udn_ = udn; }
    /**
     * Allowed. Universal Product Code.  12-digit,  all- numericc ode that  identifies the consumer
     * package. Managed by the Uniform Code Council. Specified by UPnP vendor. Single UPC.
     * @brief upc
     * @return
     */
    std::string upc() const { return upc_; }
    /**
     * @brief upc
     * @param upc
     */
    void upc( const std::string & upc) { upc_ = upc; }
    /**
     * Recommended. URL to presentation for device. shall be
     * relative to the URL at which the device description is located in accordance with the rules
     * spec ified in clause 5 of  RFC 3986. Specified by UPnP vendor. Single URL.
     * @brief presentationUrl
     * @return
     */
    std::string presentationUrl() const { return presentationUrl_; }
    /**
     * @brief presentationUrl
     * @param presentationUrl
     */
    void presentationUrl( const std::string & presentationUrl) { presentationUrl_ = presentationUrl; }
    /**
     * @brief device base url.
     * @return
     */
    std::string urlBase() const { return url_base_; }
    /**
     * @brief device base url.
     * @param url_base
     */
    void urlBase( const std::string & url_base) { url_base_ = url_base; }

    /**
     * Required if  and only if  device has one or more icons. Specified by  UPnP vendor.
     * @brief iconList
     * @return
     */
    std::vector< Icon > iconList() const { return images_; }
    /**
     * @brief addIcon
     * @param icon
     */
    void addIcon( Icon icon ) { images_.push_back( icon ); }

    /**
     * @brief serviceList
     * @return
     */
    std::vector< Service > serviceList() const { return services_; }
    /**
     * @brief addService
     * @param service
     */
    void addService( Service service ) { services_.push_back( service ); }

    std::chrono::high_resolution_clock::time_point timestamp() {
        return timestamp_;
    }
    void touch() {
        timestamp_ = std::chrono::high_resolution_clock::now();
    }
    bool timeout() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto dur = end_time - timestamp_;
        auto f_secs = std::chrono::duration_cast<std::chrono::duration<unsigned int>> ( dur );
        return( f_secs.count() >= timeout_ );
    }
    void timeout(time_t timeout ) {
        timeout_ = timeout;
    }

private:
    int versionMajor_, versionMinor_;
    std::string deviceType_, friendlyName_, manufacturer_, manufacturerUrl_, modelDescription_, modelName_,
                modelNumber_, modelUrl_, serialNumber_, udn_, upc_, presentationUrl_, url_base_;
    std::vector< Icon > images_;
    std::vector< Service > services_;
    std::vector< Device > devices_;
    std::chrono::high_resolution_clock::time_point timestamp_;
    time_t timeout_ = 1800;
};
inline void parse( const std::string & xml, Device & device ) {
    rapidxml_ns::xml_document<> doc;
    doc.parse<0>( const_cast< char* >(  xml.c_str() ) );
    auto root_node = doc.first_node();
    FOR( "*", root_node, [&device]( rapidxml_ns::xml_node<>* body_node ) {
        std::cout << body_node->local_name() << std::endl;
        if( strcmp( "specVersion", body_node->local_name() ) == 0 ) {
            FOR( "*", body_node, [&device]( rapidxml_ns::xml_node<>* version_node ) {
                if( strcmp( "major", version_node->local_name() ) == 0 ) {
                    device.versionMajor( std::atoi( version_node->value() ) );
                } else if( strcmp( "minor", version_node->local_name() ) == 0 ) {
                    device.versionMinor( std::atoi( version_node->value() ) );
                }
            });
        } else if( strcmp( "device", body_node->local_name() ) == 0 ) {
            FOR( "*", body_node, [&device]( rapidxml_ns::xml_node<>* device_node ) {
                if( strcmp( "deviceType", device_node->local_name() ) == 0 ) {
                    device.deviceType( device_node->value() );
                } else if( strcmp( "presentationURL", device_node->local_name() ) == 0 ) {
                    device.presentationUrl( device_node->value() );
                } else if( strcmp( "friendlyName", device_node->local_name() ) == 0 ) {
                    device.friendlyName( device_node->value() );
                } else if( strcmp( "manufacturer", device_node->local_name() ) == 0 ) {
                    device.manufacturer( device_node->value() );
                } else if( strcmp( "manufacturerURL", device_node->local_name() ) == 0 ) {
                    device.manufacturerUrl( device_node->value() );
                } else if( strcmp( "modelNumber", device_node->local_name() ) == 0 ) {
                    device.modelNumber( device_node->value() );
                } else if( strcmp( "modelDescription", device_node->local_name() ) == 0 ) {
                    device.modelDescription( device_node->value() );
                } else if( strcmp( "modelName", device_node->local_name() ) == 0 ) {
                    device.modelName( device_node->value() );
                } else if( strcmp( "UDN", device_node->local_name() ) == 0 ) {
                    device.udn( device_node->value() );
                } else if( strcmp( "urlBase", device_node->local_name() ) == 0 ) {
                    device.urlBase( device_node->value() );
                } else if( strcmp( "serialNumber", device_node->local_name() ) == 0 ) {
                    device.serialNumber( device_node->value() );
                } else if( strcmp( "iconList", device_node->local_name() ) == 0 ) {
                    FOR( "icon", device_node, [&device]( rapidxml_ns::xml_node<>* iconlist_node ) {
                        Icon _icon;
                        FOR( "*", iconlist_node, [&_icon]( rapidxml_ns::xml_node<>* icon_node ) {
                            if( strcmp( "mimetype", icon_node->local_name() ) == 0 ) {
                                _icon.mimeType( icon_node->value() );
                            } else if( strcmp( "width", icon_node->local_name() ) == 0 ) {
                                _icon.width( std::atoi( icon_node->value() ) );
                            } else if( strcmp( "height", icon_node->local_name() ) == 0 ) {
                                _icon.height( std::atoi( icon_node->value() ) );
                            } else if( strcmp( "depth", icon_node->local_name() ) == 0 ) {
                                _icon.depth( std::atoi( icon_node->value() ) );
                            } else if( strcmp( "url", icon_node->local_name() ) == 0 ) {
                                _icon.url( icon_node->value() );
                            }
                        });
                        device.addIcon( _icon );
                    });
                } else if( strcmp( "serviceList", device_node->local_name() ) == 0 ) {
                    FOR( "service", device_node, [&device]( rapidxml_ns::xml_node<>* servicelist_node ) {
                        Service _service;
                        FOR( "*", servicelist_node, [&_service]( rapidxml_ns::xml_node<>* service_node ) {
                            if( strcmp( "serviceType", service_node->local_name() ) == 0 ) {
                                _service.serviceType( service_node->value() );
                            } else if( strcmp( "serviceId", service_node->local_name() ) == 0 ) {
                                _service.serviceId( service_node->value() );
                            } else if( strcmp( "SCPDURL", service_node->local_name() ) == 0 ) {
                                _service.scpdUrl( service_node->value() );
                            } else if( strcmp( "controlURL", service_node->local_name() ) == 0 ) {
                                _service.controlUrl( service_node->value() );
                            } else if( strcmp( "eventSubURL", service_node->local_name() ) == 0 ) {
                                _service.eventSubUrl( service_node->value() );
                            }
                        });
                        device.addService( _service );
                    });
                }
            });

        } else if( strcmp( "URLBase", body_node->local_name() ) == 0 ) {
            device.urlBase( body_node->value() );
        }
    });
}
}//namespace upnp
#endif // DEVICE_H
