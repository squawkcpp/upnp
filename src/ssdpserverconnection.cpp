#include "ssdpserverconnection.h"

//#include <http.h>

#include <thread>
#include <future>

#include "spdlog/spdlog.h"

namespace upnp {
inline namespace asio_impl {

SSDPServerConnection::SSDPServerConnection ( const std::string & multicast_address,
        const int & multicast_port, const std::string & listen_address, std::function< void ( http::Request& ) > handler ) :
    io_service(), strand_ ( io_service ), socket ( io_service ), multicast_address ( multicast_address ),
    multicast_port ( multicast_port ), _handler ( handler ) {

        asio::ip::address _multicast_address = asio::ip::address::from_string ( multicast_address );
    asio::ip::address _listen_address = asio::ip::address::from_string ( listen_address );

        // Create the socket so that multiple may be bound to the same address.
    asio::ip::udp::endpoint listen_endpoint ( asio::ip::udp::v4(), multicast_port );
        socket.open ( listen_endpoint.protocol() );

        socket.set_option ( asio::ip::udp::socket::reuse_address ( true ) );
        socket.bind ( listen_endpoint );

        // Join the multicast group.
        socket.set_option (
        asio::ip::multicast::join_group ( _multicast_address.to_v4(), _listen_address.to_v4() ) );

    using namespace std::placeholders;
    socket.async_receive_from ( asio::buffer ( data, http::BUFFER_SIZE ), sender_endpoint,
                                strand_.wrap ( std::bind ( &SSDPServerConnection::handle_receive_from, this, _1,_2 ) ) );

    ssdp_runner = std::unique_ptr<std::thread> ( new std::thread (
        std::bind ( static_cast<size_t ( asio::io_service::* ) () > ( &asio::io_service::run ), &io_service ) ) );
}

SSDPServerConnection::~SSDPServerConnection() {
        io_service.stop();
        ssdp_runner->join();
}

void SSDPServerConnection::send ( std::string request_line, std::map< std::string, std::string > headers ) {

    std::string message = create_header ( request_line, headers );

        asio::io_service io_service_;
        asio::ip::udp::endpoint endpoint ( asio::ip::address::from_string ( multicast_address.c_str() ), multicast_port );
        asio::ip::udp::socket socket ( io_service_, endpoint.protocol() );
        socket.send_to (
                asio::buffer ( message, message.length() ), endpoint );
}

void SSDPServerConnection::send ( SsdpResponse response ) {
    try {
        std::string buffer = create_header ( response.request_line, response.headers );
            socket.send_to (
                    asio::buffer ( buffer, buffer.length() ), sender_endpoint );
    } catch( std::system_error& e ) {
        spdlog::get ( LOGGER )-> warn ( "error in send response: ({}:{})", e.code().value(), e.what() );
    }
}

void SSDPServerConnection::handle_receive_from ( const asio::error_code & error, size_t bytes_recvd ) {
        if ( !error ) {
        http::Request request;
        request.remote_ip ( sender_endpoint.address().to_string() );
        http_parser.parse_request( request, data, 0, bytes_recvd );
        _handler ( request );
        //TODO http_parser.reset();

        using namespace std::placeholders;
        socket.async_receive_from ( asio::buffer ( data, http::BUFFER_SIZE ), sender_endpoint,
            strand_.wrap ( std::bind ( &SSDPServerConnection::handle_receive_from, this, _1, _2 ) ) );

    } else { /*TODO http_parser.reset(); */ } //on error
}
}//namespace asio_impl
}//namespace upnp
