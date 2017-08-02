#ifndef SSDPCLIENTCONNECTION_H
#define SSDPCLIENTCONNECTION_H

#include <string>

#include <asio.hpp>

#include "http/utils/httpparser.h"

namespace upnp {
inline namespace asio_impl {

class SSDPClientConnection {
public:
    SSDPClientConnection ( const std::string & multicast_address, const int & multicast_port,
                           std::function< void ( http::Response& ) > handler ) :
        io_service_(), strand_ ( io_service_ ), socket ( io_service_ ),
        multicast_address ( multicast_address ), multicast_port ( multicast_port ),
        _handler ( handler ) {}

        ~SSDPClientConnection() {
                io_service_.stop();
                ssdp_runner->join();
        }

        void send ( const std::string & request_line, const std::map< std::string, std::string > & headers );
        void handle_receive_from ( const asio::error_code&, size_t bytes_recvd );

private:
        asio::io_service io_service_;
        asio::io_service::strand strand_;
        asio::ip::udp::socket socket;
        std::string multicast_address;
        int multicast_port;
        asio::ip::udp::endpoint sender_endpoint;

    std::function< void ( http::Response& ) > _handler;

    enum { max_length = http::BUFFER_SIZE };
        std::array< char, max_length > data;

    /* the runner thread */
        std::unique_ptr<std::thread> ssdp_runner;
    http::utils::HttpParser http_parser;
};
}//namespace asio_impl
}//namespace upnp
#endif // SSDPCLIENTCONNECTION_H
