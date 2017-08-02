#include "ssdpclientconnection.h"

#include "_utils.h"

namespace upnp {
inline namespace asio_impl {

void SSDPClientConnection::send ( const std::string & request_line, const std::map< std::string, std::string > & headers ) {
        std::string message = create_header ( request_line, headers );

    asio::ip::udp::endpoint endpoint ( asio::ip::address::from_string ( multicast_address.c_str() ), multicast_port );
    asio::ip::address _listen_address = asio::ip::address::from_string ( "0.0.0.0" );
    asio::ip::udp::endpoint listen_endpoint ( asio::ip::udp::v4() /*_listen_address*/, 0 );

    socket.open ( listen_endpoint.protocol() );
    socket.set_option ( asio::ip::udp::socket::reuse_address ( true ) );
    socket.bind ( listen_endpoint );

    using namespace std::placeholders;
    socket.async_receive_from ( asio::buffer ( data, max_length ), sender_endpoint,
                                strand_.wrap ( std::bind ( &SSDPClientConnection::handle_receive_from, this, _1, _2 ) ) );

    ssdp_runner = std::unique_ptr<std::thread> ( new std::thread (
                      std::bind ( static_cast<size_t ( asio::io_service::* ) () > ( &asio::io_service::run ), &io_service_ ) ) );
    socket.send_to ( asio::buffer ( message, message.length() ), endpoint );

    std::this_thread::sleep_for ( std::chrono::seconds ( 5 ) );
    socket.close();
}
void SSDPClientConnection::handle_receive_from ( const asio::error_code & error, size_t bytes_recvd ) {
    std::cout << "handle_receive_from: " << std::endl;

    if ( !error ) {
        http::Response response;
        response.remote_ip( sender_endpoint.address().to_string() );
        http_parser.parse_response ( response, data, 0, bytes_recvd );
        _handler ( response );
        using namespace std::placeholders;
        socket.async_receive_from ( asio::buffer ( data, max_length ), sender_endpoint,
                                    strand_.wrap ( std::bind ( &SSDPClientConnection::handle_receive_from, this, _1, _2 ) ) );
        }
}
}//namespace asio_impl
}//namespace upnp
