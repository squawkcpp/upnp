#ifndef DATASTORE_H
#define DATASTORE_H

#include <redox.hpp>

namespace data {

typedef std::shared_ptr< redox::Redox > redox_ptr;

inline redox_ptr make_connection( const std::string db, int port ) {
    redox_ptr rdx = redox_ptr( new redox::Redox() );
    if( !rdx->connect( db, port ) )
    { throw std::system_error( std::error_code(300, std::generic_category() ), "unable to connect to database on localhost." ); }
    return std::move( rdx );
}

/** @brief get the configuration as json string from the database. */
inline std::string config( redox_ptr rdx /** @param rdx redox database. */ ) {
    return rdx->get( "upnp:config" );
}
/** @brief store the configuration as json string in the database. */
inline void config( redox_ptr rdx, /** @param rdx redox database. */
             const std::string& json /** @param c the configuration as json string. */ ) {
    rdx->set( "upnp:config", json );
}
/** @brief check if a configuration exists in the database. */
inline bool config_exists( redox_ptr rdx /** @param rdx redox database. */ ) {
    redox::Command<int>& c = rdx->commandSync<int>({"EXISTS", "upnp:config"} );
    return( c.ok() && c.reply() );
}
}//namespace data
#endif // DATASTORE_H
