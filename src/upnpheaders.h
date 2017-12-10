/*
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef UPNPHEADERS_H
#define UPNPHEADERS_H

#include <memory>
#include <string>

#include "http/constant.h"
#include "http/request.h"
#include "http/response.h"

#include "config.h"

namespace upnp {

class UpnpHeaders {
public:
    UpnpHeaders( config_t config ) : config_( config ) {};
    UpnpHeaders ( const UpnpHeaders& ) = default; //TODO
    UpnpHeaders ( UpnpHeaders&& ) = default;
    UpnpHeaders& operator= ( const UpnpHeaders& ) = default; //TODO
    UpnpHeaders& operator= ( UpnpHeaders&& ) = default;
    ~UpnpHeaders() {}

    http::http_status execute ( http::Request& request, http::Response& response );

private:
    config_t config_;
};
}//namespace upnp
#endif // UPNPHEADERS_H
