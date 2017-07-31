#ifndef SERVER_H
#define SERVER_H

#include <memory>

#include "http/server.h"
#include "http/httpserver.h"

/** @brief UPNP Server implementation. */
namespace upnp {

/** @brief The UPNP Server class. */
class Server {
public:
    /** @brief The CDS Server CTOR. */
    Server(std::shared_ptr< http::Server< http::HttpServer > > web_server, /** @param web_server the server to attach the API uri's. */
           const std::string& redis, /** @param redis redis host */
           const short port /** @param port the redis port. */ );
};
}//namespace upnp
#endif // SERVER_H
