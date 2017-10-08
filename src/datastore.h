/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef DATASTORE_H
#define DATASTORE_H

#include <functional>
#include <map>
#include <regex>
#include <string>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/functional/hash.hpp>

#include "fmt/format.h"
#include <redox.hpp>

#include "gtest/gtest_prod.h"

/** the redis commands */
namespace redis {
static const std::string SADD       = "SADD";
static const std::string DEL        = "DEL";
static const std::string EXISTS     = "EXISTS";
static const std::string HGET       = "HGET";
static const std::string HGETALL    = "HGETALL";
static const std::string HMSET      = "HMSET";
static const std::string LRANGE     = "LRANGE";
static const std::string SCARD      = "SCARD";
static const std::string SMEMBERS   = "SMEMBERS";
static const std::string SUNION     = "SUNION";
static const std::string SREM       = "SREM";
static const std::string ZADD       = "ZADD";
static const std::string ZCARD      = "ZCARD";
static const std::string ZRANGE     = "ZRANGE";
static const std::string ZREVRANGE  = "ZREVRANGE";
static const std::string ZREM       = "ZREM";
}

namespace param {
static const std::string ALBUM          = "album";
static const std::string ARTIST         = "artist";
static const std::string AUTHOR         = "author";
static const std::string AUDIO          = "audio";
static const std::string BACKDROP_PATH  = "backdrop_path";
static const std::string BITRATE        = "bitrate";
static const std::string BPS            = "bps";
static const std::string CHANNELS       = "channels";
static const std::string CLASS          = "cls";
static const std::string CLEAN_STRING   = "clean_string";
static const std::string COMMENT        = "comment";
static const std::string COMPOSER       = "composer";
static const std::string COVER          = "cover";
static const std::string DATE           = "date";
static const std::string DISC           = "disc";
static const std::string EBOOK          = "ebook";
static const std::string EPISODE        = "episode";
static const std::string EXT            = "ext";
static const std::string EXTENSION      = "ext";
static const std::string FILE           = "file";
static const std::string FOLDER         = "folder";
static const std::string GENRE          = "genre";
static const std::string HEIGHT         = "height";
static const std::string HOMEPAGE       = "homepage";
static const std::string IMDB_ID        = "imdb_id";
static const std::string IMAGE          = "image";
static const std::string ISBN           = "isbn";
static const std::string LANGUAGE       = "language";
static const std::string MAKE           = "Make";
static const std::string MIME_TYPE      = "mimeType";
static const std::string MOVIE          = "movie";
static const std::string NAME           = "name";
static const std::string PARENT         = "parent";
static const std::string PATH           = "path";
static const std::string PERFORMER      = "performer";
static const std::string PLAYTIME       = "playlength";
static const std::string POSTER_PATH    = "poster_path";
static const std::string PUBLISHER      = "publisher";
static const std::string ROOT           = "root";
static const std::string SAMPLERATE     = "samplerate";
static const std::string SEASON         = "season";
static const std::string SERIE          = "serie";
static const std::string SIZE           = "size";
static const std::string STILL_IMAGE    = "still_image";
static const std::string THUMB          = "thumb";
static const std::string TIMESTAMP      = "timestamp";
static const std::string TITLE          = "title";
static const std::string TMDB_ID        = "tmdb_id";
static const std::string TRACK          = "track";
static const std::string WIDTH          = "width";
static const std::string YEAR           = "year";
}

namespace key {
static const std::string CONFIG = "config:cds";
static const std::string FS     = "fs";
static const std::string GENRES = "genres";
static const std::string KEY    = "key";
static const std::string LIST   = "list";
static const std::string MIME   = "fs:mime";
static const std::string NEW    = "new";
static const std::string TAG    = "tag";
static const std::string TYPE   = "type";
}

///@cond DOC_INTERNAL
namespace _internal {
static const std::vector< std::string > __NAMES = std::vector< std::string> (
{ param::FOLDER, param::AUDIO, param::MOVIE, param::SERIE, param::IMAGE, param::EBOOK,
  param::FILE, param::ALBUM, param::COVER, param::EPISODE, param::ARTIST } );
static const std::array< std::map< std::string, std::string >, 7 > menu ( {{
        { {param::NAME, "Music Albums"},  {key::TYPE, param::ALBUM} },
        { {param::NAME, "Music Artists"}, {key::TYPE, param::ARTIST} },
        { {param::NAME, "Photos"},        {key::TYPE, param::IMAGE} },
        { {param::NAME, "Movies"},        {key::TYPE, param::MOVIE} },
        { {param::NAME, "TV Series"},     {key::TYPE, param::SERIE} },
        { {param::NAME, "eBooks"},        {key::TYPE, param::EBOOK} },
        { {param::NAME, "Storage"},       {key::TYPE, param::FILE } }
}});
}
///@endcond DOC_INTERNAL

/** @brief Read and write CDS data in redis. */
struct data {

typedef std::shared_ptr< redox::Redox > redis_ptr;
typedef std::map< std::string, std::string > node_t;
typedef std::set< std::string > nodes_t;
typedef std::function< void(const std::string&) > async_fn;
typedef std::vector< std::string > command_t;

/** @brief current timestamp in millis. */
static unsigned long time_millis() {
    return
        std::chrono::system_clock::now().time_since_epoch() /
        std::chrono::milliseconds(1);
}
/** @brief hash create a hash of the input string. */
static std::string hash ( const std::string& in /** @param in string to hash. */ ) {
    static boost::hash<std::string> _hash;
    //TODO why slash or fs?
    if ( in == "/" || in == key::FS || is_mod ( in ) ) {
        return in;
    } else {
        return std::to_string ( _hash ( in ) );
    }
}

class NodeType {
public:
    enum Enum { folder = 0, audio = 1, movie = 2, serie = 3, image = 4, ebook = 5, file = 6, album = 7, cover = 8, episode = 9, artist = 10 };

    /** @brief node type as string.  */
    static std::string str ( Enum type ) {
        return _internal::__NAMES.at ( type );
    }

    /** @brief get type from string.  */
    static Enum parse ( const std::string& type ) {
        if ( type == param::FOLDER )
        { return folder; }

        if ( type == param::AUDIO )
        { return audio; }

        if ( type == param::MOVIE )
        { return movie; }

        if ( type == param::SERIE )
        { return serie; }

        if ( type == param::IMAGE )
        { return image; }

        if ( type == param::EBOOK )
        { return ebook; }

        if ( type == param::ALBUM )
        { return album; }

        if ( type == param::COVER )
        { return cover; }

        if ( type == param::ARTIST )
        { return artist; }

        if ( type == param::EPISODE )
        { return episode; }

        return file;
    }

    /** @brief is the type string valid.  */
    static bool valid ( const std::string& type ) {
        return std::find ( _internal::__NAMES.begin(), _internal::__NAMES.end(), type ) != _internal::__NAMES.end();
    }
};

// -----------------------------------------------------------------------------------------------------------
// --------------------------                      key utils                        --------------------------
// -----------------------------------------------------------------------------------------------------------

/** @brief check if path is a mod path */
static bool is_mod ( const std::string& path ) {
    for ( auto& __mod : _internal::menu ) {
        if ( __mod.at( key::TYPE ) == path ) {
            return true;
        }
    }
    return false;
}

template< typename... ARGS >
/** @brief make_key */
static std::string make_key (
    const std::string& prefix, /** @param prefix the prefix of the string*/
    const std::string& value, /** @param value the first value to add */
    ARGS... args /** @param args the following tokens */ ) {
    std::string _res = prefix;
    __iterate_key ( _res, value, args... );
    return _res;
}

/** @brief make node key */
static std::string make_key_node ( const std::string& key /** @param key node key. */ ) {
    return make_key ( key::FS, key ); /** TODO add node */
}
/** @brief make type list key (fs:KEY:TYPE (SET:KEY) ) */
static std::string make_key_list ( const std::string& key /** @param key node key. */ ) {
    return make_key( key::FS, key, key::LIST );
}
/** @brief make node list key (fs:TYPE:list (SET:KEY) ) */
static std::string make_key_list ( const NodeType::Enum& type /** @param type node type. */ ) {
    return make_key( key::FS, NodeType::str ( type ), key::LIST );
}

// -----------------------------------------------------------------------------------------------------------
// --------------------------                    database utils                     --------------------------
// -----------------------------------------------------------------------------------------------------------

/** @brief create new database connection. */
static redis_ptr make_connection ( const std::string db /** @param db database host */,
                                   int port /** @param port database port */ ) {
    redis_ptr _redis = redis_ptr ( new redox::Redox() );

    if ( !_redis->connect ( db, port ) ) {
        throw std::system_error ( std::error_code ( 300, std::generic_category() ),
                                  "unable to connect to database on localhost." );
    }
    return _redis;
}

/** @brief get the node attribute value. */
static std::string get( redis_ptr redis /** @param redis the database pointer. */,
                        const std::string& key /** @param key redis key. */,
                        const std::string param /** @param param parameter name. */ ) {

    auto& _item = redis->commandSync< std::string > ( {redis::HGET, data::make_key_node ( key ), param} );
    /** @return the the value or empty string if not found. */
    return( _item.ok() ? _item.reply() : "" );
}

/** @brief set the node attribute value. */
static void set( redis_ptr redis /** @param redis the database pointer. */,
                        const std::string& key /** @param key redis key. */,
                        const std::string param /** @param param parameter name. */,
                        const std::string value /** @param value parameter value. */ ) {

    redis->command ( { redis::HMSET, data::make_key_node ( key ), param, value } );
}

/** @brief store the node in redis. */
static void save( redis_ptr redis /** @param redis pointer to redis database. */,
                  const std::string& key /** @param key key of the object to store. */,
                  const node_t& node /** @param node the node values to store. */ ) {
    auto _command = std::vector< std::string >( { redis::HMSET, make_key_node ( key ) } );
    for ( auto __i : node ) {
        _command.push_back ( __i.first );
        _command.push_back ( __i.second );
    }
    redis->command ( _command );
}

/** @brief get the node by key */
static node_t node ( redis_ptr redis /** @param redis redis database pointer. */,
                     const std::string& key /** @param key the node key. */ ) {

    auto& _item = redis->commandSync< std::vector< std::string > > ( { redis::HGETALL, make_key_node ( key ) } );
    /** @return the the node or empty map if not found. */
    return( _item.ok() ? to_map( _item.reply() ) : node_t() );
}

/** @brief get the node children */
static void children( redis_ptr redis /** @param redis redis database pointer. */,
                      const std::string& key /** @param key the node key. */,
                      const int& index /** @param index start index. */,
                      const int& count /** @param count result size. */,
                      const std::string& sort /** @param sort sort results [alpha, timestamp]. */,
                      const std::string& order /** @param order order the results [asc, desc]. */,
                      const std::string& filter /** @param filter filter results by keyword. */,
                      async_fn fn /** @param fn the callback function. */ ) {

    //TODO sort and filter
    command_t _redis_command;
    if( sort == "default" ) {
        _redis_command = { (order=="desc"?redis::ZREVRANGE:redis::ZRANGE), make_key_list( key ), std::to_string( index ), std::to_string( index + count ) };
    } else {
        _redis_command ={ redis::LRANGE, make_key( make_key_list( key ), sort, order ), std::to_string( index ), std::to_string( index + count ) };
    }
    redox::Command< std::vector< std::string > >& _c = redis->commandSync< std::vector< std::string > >( _redis_command );
    if( _c.ok() ) {
        for( const std::string& __c : _c.reply() ) {
            fn( __c );
        }
    }
}

/** @brief get the node files by type*/
static void files( redis_ptr redis /** @param redis redis database pointer. */,
                      const std::string& key /** @param key the file key. */,
                      const NodeType::Enum type /** @param type the file type. */,
                      const int& index /** @param index start index. */,
                      const int& count /** @param count result size. */,
                      async_fn fn /** @param fn the callback function. */ ) {

    redox::Command< std::vector< std::string > >& _c = redis->commandSync< std::vector< std::string > >(
        { redis::ZRANGE, data::make_key( key::FS, key, "types" /** TODO */, NodeType::str( type ) ), std::to_string( index ), std::to_string( index + count ) }
    );
    if( _c.ok() ) {
        for( const std::string& __c : _c.reply() ) {
            fn( __c );
        }
    }
}

/** @brief get the node children count */ //TODO add params sort,order,tags
static int children_count( redis_ptr redis, const std::string& key ) {
    redox::Command< int >& _c = redis->commandSync< int >(
        { redis::ZCARD, make_key_list( key ) }
    );
    return( _c.ok() ? _c.reply() : 0 );
}

/** @brief get the node children count */ //TODO add params sort,order,tags
static int files_count( redis_ptr redis, const std::string& key, const NodeType::Enum type ) {
    redox::Command< int >& _c = redis->commandSync< int >(
        { redis::ZCARD, make_key( key::FS, key, "types", NodeType::str( type ) ) }
    );
    return( _c.ok() ? _c.reply() : 0 );
}

/** @brief create a new tag. */
static void add_tag( redis_ptr redis /** @param redis redis database pointer. */,
                            const std::string& name /** @param name name of the tag */,
                            const std::string& keyword /** @param keyword the tag */,
                            const NodeType::Enum& type /** TODO remove @param type the type for the tag */,
                            const std::string& node /** @param node the node key */,
                            float score /** @param score the score of the node index */ ) {

    redis->command( { redis::ZADD, make_key( key::FS, key::TAG, param::NAME ), std::to_string( score ), name } );
    redis->command( { redis::ZADD, make_key( key::FS, key::TAG, name ), std::to_string( score ),  keyword } );
    redis->command( { redis::ZADD, make_key( key::FS, key::TAG, keyword ), std::to_string( score ), node } );
}

/** @brief evaluate the lua script. */
template< class... ARGS>
static void eval(  redis_ptr redis /** @param redis the database pointer. */,
                   const std::string& script,
                   int argc,
                   ARGS... argv ) {
    redis->command ( { "EVAL", script, std::to_string( argc ), argv... } );
}

// -----------------------------------------------------------------------------------------------------------
// --------------------------                  content relations                    --------------------------
// -----------------------------------------------------------------------------------------------------------

/** @brief add node to parents type list */
static void add_types( redis_ptr redis, const std::string& path, const std::string& key, const unsigned long& score )
{ redis->command( {redis::ZADD, data::make_key_list( path ), std::to_string( score ), key } ); }
/** @brief remove node from parents type list */
static void rem_types( redis_ptr redis, const std::string& parent, const std::string& key )
{ redis->command( {redis::ZREM, data::make_key_list( parent ), hash( key ) } ); }

/** @brief add node to global nodes list */
static void add_nodes( redis_ptr redis, NodeType::Enum type, const std::string& key, unsigned long score )
{ redis->command( {redis::ZADD,  data::make_key_list( type ), std::to_string( score ), key } ); }
/** @brief remove node from global nodes list */
static void rem_nodes( redis_ptr redis, NodeType::Enum type, const std::string& key )
{ redis->command( {redis::ZREM,  data::make_key_list( type ), key } ); }

/** @brief add node to global nodes list */
static void add_nodes( redis_ptr redis, const std::string& parent, NodeType::Enum type, const std::string& key, unsigned long score )
{ redis->command( {redis::ZADD,  data::make_key( key::FS, parent, "types", NodeType::str( type ) ), std::to_string( score ), key } ); }

// -----------------------------------------------------------------------------------------------------------
// --------------------------                      mime type                        --------------------------
// -----------------------------------------------------------------------------------------------------------

/** @brief add mime type to list and increment mime counter. */
static void incr_mime ( data::redis_ptr redis /** @param redis redis database pointer. */,
                        const std::string& mime_type /** @param mime_type the mime type.*/ ) {
    //reference in mime-type set
    redis->command ( { redis::SADD, key::MIME, mime_type } );
    //incr the mime_type counter
    redis->command ( { "INCR", data::make_key( key::MIME, mime_type ) } );
}

// -----------------------------------------------------------------------------------------------------------
// --------------------------                    configuration                      --------------------------
// -----------------------------------------------------------------------------------------------------------

/** @brief get the configuration as json string from the database. */
static std::string config ( redis_ptr redis /** @param redis redox database. */ ) {
    return redis->get ( key::CONFIG );
}
/** @brief store the configuration as json string in the database. */
static void config ( redis_ptr redis, /** @param redis redox database. */
                     const std::string& json /** @param c the configuration as json string. */ ) {
    redis->set ( key::CONFIG, json );
}
/** @brief check if a configuration exists in the database. */
static bool config_exists ( redis_ptr redis /** @param redis redox database pointer. */ ) {
    redox::Command<int>& c = redis->commandSync<int> ( { redis::EXISTS, key::CONFIG} );
    return ( c.ok() && c.reply() );
}

private:
static void __iterate_key (
    std::string& key,
    const std::string& value ) {
    key.append ( ":" );
    key.append ( value );
}
template< class... ARGS >
static void __iterate_key (
    std::string& key,
    const std::string& value,
    ARGS... args ) {
    key.append ( ":" );
    key.append ( value );
    __iterate_key ( key, args... );
}
FRIEND_TEST(DatastoreTest, to_map );
static std::map< std::string, std::string > to_map ( command_t in ) {
    std::map< std::string, std::string > _map;
    assert ( in.size() % 2 == 0 );
    for ( size_t i = 0; i < in.size(); ++++i ) {
        _map[ in.at ( i ) ] = in.at ( i + 1 );
    }
    return _map;
}
};//struct data
#endif // DATASTORE_H
