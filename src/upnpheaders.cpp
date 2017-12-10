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
#include "upnpheaders.h"

#include "fmt/format.h"

#include "_utils.h"

namespace upnp {

http::http_status UpnpHeaders::execute ( http::Request& request, http::Response& response ) {

    //set the status line
    response.version_major ( request.version_major() );
    response.version_minor ( request.version_minor() );
    response.protocol ( request.protocol() );

    //set content length
    if ( ! response.contains_parameter ( http::header::CONTENT_LENGTH ) ) {
        response.parameter ( http::header::CONTENT_LENGTH,  std::to_string ( response.tellp() ) );
    }

    //add expiration date
    if ( response.expires() ) {
        time_t now = time ( nullptr );
        struct tm then_tm = *gmtime ( &now );
        then_tm.tm_sec += response.expires();
        mktime ( &then_tm );
        response.parameter ( http::header::EXPIRES, http::utils::time_to_string ( &then_tm ) );
    }

    //add now
    time_t now = time ( nullptr );
    struct tm now_tm = *gmtime ( &now );
    mktime ( &now_tm );
    response.parameter ( http::header::DATE, http::utils::time_to_string ( &now_tm ) );

    //add rest
    response.parameter ( http::header::SERVER, fmt::format ( "{} DLNADOC/1.50 UPnP/1.0 SSDP/1.0.0", uname() ) );
    response.parameter ( "SID", fmt::format ("uuid:{}", config_->uuid ) );
    response.parameter ( "EXT", http::utils::time_to_string ( &now_tm ) );

    return http::http_status::OK;
}
}//namespace upnp
