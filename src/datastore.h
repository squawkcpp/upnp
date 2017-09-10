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
#include <string>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/functional/hash.hpp>

#include "fmt/format.h"
#include <redox.hpp>

/** @brief Read and write CDS data in redis. */
namespace data {

// -----------------------------------------------------------------------------------------------------------
// --------------------------                 constants and types                   --------------------------
// -----------------------------------------------------------------------------------------------------------

static const std::string REDIS_ADD = "SADD";
static const std::string REDIS_EXISTS = "EXISTS";
static const std::string REDIS_HGET = "HGET";
static const std::string REDIS_HGETALL = "HGETALL";
static const std::string REDIS_HMSET = "HMSET";
static const std::string REDIS_SCARD = "SCARD";
static const std::string REDIS_MEMBERS = "SMEMBERS";
static const std::string REDIS_SUNION = "SUNION";
static const std::string REDIS_REM = "SREM";
static const std::string REDIS_ZADD = "ZADD";
static const std::string REDIS_ZCARD = "ZCARD";
static const std::string REDIS_ZRANGE = "ZRANGE";
static const std::string REDIS_ZREM = "ZREM";

static const std::string KEY_CLASS = "cls";
static const std::string KEY_CONFIG = "config:cds";
static const std::string KEY_FS = "fs";
static const std::string KEY_KEY = "key";
static const std::string KEY_LIST = "list";
static const std::string KEY_MIME_LIST = "fs:mime";
static const std::string KEY_MIME = "mime";
static const std::string KEY_NAME = "name";
static const std::string KEY_NEW = "new";
static const std::string KEY_PARENT = "parent";
static const std::string KEY_PATH = "path";
static const std::string KEY_TAG = "tag";
static const std::string KEY_TIMESTAMP = "timestamp";
static const std::string KEY_TYPE = "type";
static const std::string TYPE_ARTIST = "artist";
static const std::string TYPE_FOLDER = "folder";
static const std::string TYPE_AUDIO = "audio";
static const std::string TYPE_ALBUM = "album";
static const std::string TYPE_MOVIE = "movie";
static const std::string TYPE_SERIE = "serie";
static const std::string TYPE_IMAGE = "image";
static const std::string TYPE_FILE = "file";
static const std::string TYPE_EBOOK = "ebook";
static const std::string TYPE_COVER = "cover";
static const std::string TYPE_EPISODE = "episode";

typedef std::shared_ptr< redox::Redox > redis_ptr;
typedef std::map< std::string, std::string > node_t;
typedef std::set< std::string > nodes_t;
typedef std::function< void(const std::string&) > async_fn;

///@cond DOC_INTERNAL
static const std::vector< std::string > __NAMES = std::vector< std::string> (
{ TYPE_FOLDER, TYPE_AUDIO, TYPE_MOVIE, TYPE_SERIE, TYPE_IMAGE, TYPE_EBOOK,
  TYPE_FILE, TYPE_ALBUM, TYPE_COVER, TYPE_EPISODE, TYPE_ARTIST } );
///@endcond DOC_INTERNAL

class NodeType {
public:
    enum Enum { folder = 0, audio = 1, movie = 2, serie = 3, image = 4, ebook = 5, file = 6, album = 7, cover = 8, episode = 9, artist = 10 };

    /** @brief node type as string.  */
    static std::string str ( Enum type ) {
        return __NAMES.at ( type );
    }

    /** @brief get type from string.  */
    static Enum parse ( const std::string& type ) {
        if ( type == TYPE_FOLDER )
        { return folder; }

        if ( type == TYPE_AUDIO )
        { return audio; }

        if ( type == TYPE_MOVIE )
        { return movie; }

        if ( type == TYPE_SERIE )
        { return serie; }

        if ( type == TYPE_IMAGE )
        { return image; }

        if ( type == TYPE_EBOOK )
        { return ebook; }

        if ( type == TYPE_ALBUM )
        { return album; }

        if ( type == TYPE_COVER )
        { return cover; }

        if ( type == TYPE_ARTIST )
        { return artist; }

        if ( type == TYPE_EPISODE )
        { return episode; }

        return file;
    }

    /** @brief is the type string valid.  */
    static bool valid ( const std::string& type ) {
        return std::find ( __NAMES.begin(), __NAMES.end(), type ) != __NAMES.end();
    }
};

// -----------------------------------------------------------------------------------------------------------
// --------------------------                      key utils                        --------------------------
// -----------------------------------------------------------------------------------------------------------

static std::array< std::map< std::string, std::string >, 7 > menu ( {{
        { {KEY_NAME, "Music Albums"},  {KEY_TYPE, TYPE_ALBUM} },
        { {KEY_NAME, "Music Artists"}, {KEY_TYPE, TYPE_ARTIST} },
        { {KEY_NAME, "Photos"},        {KEY_TYPE, TYPE_IMAGE} },
        { {KEY_NAME, "Movies"},        {KEY_TYPE, TYPE_MOVIE} },
        { {KEY_NAME, "TV Series"},     {KEY_TYPE, TYPE_SERIE} },
        { {KEY_NAME, "eBooks"},        {KEY_TYPE, TYPE_EBOOK} },
        { {KEY_NAME, "Storage"},       {KEY_TYPE, TYPE_FILE } }
}});

/** @brief check if path is a mod path */
static bool is_mod ( const std::string& path ) {
    for ( auto& __mod : menu ) {
        if ( __mod[KEY_TYPE] == path ) {
            return true;
        }
    }
    return false;
}

/** @brief hash create a hash of the input string. */
inline std::string hash ( const std::string& in /** @param in string to hash. */ ) {
    static boost::hash<std::string> _hash;

    if ( in == "/" || in == KEY_FS || is_mod ( in ) ) {
        return in;
    } else {
        return std::to_string ( _hash ( in ) );
    }
}

///@cond DOC_INTERNAL
inline void __iterate_key (
    std::string& key,
    const std::string& value ) {
    key.append ( ":" );
    key.append ( value );
}
template< class... ARGS >
inline void __iterate_key (
    std::string& key,
    const std::string& value,
    ARGS... args ) {
    key.append ( ":" );
    key.append ( value );
    __iterate_key ( key, args... );
}
///@endcond DOC_INTERNAL

template< typename... ARGS >
/** @brief make_key */
inline std::string make_key (
    const std::string& prefix, /** @param prefix the prefix of the string*/
    const std::string& value, /** @param value the first value to add */
    ARGS... args /** @param args the following parameters */ ) {
    std::string _res = prefix;
    __iterate_key ( _res, value, args... );
    return _res;
}

/** @brief make node key */
inline std::string make_key_node ( const std::string& key /** @param key node key. */ ) {
    return make_key ( KEY_FS, key );
}
/** @brief make type list key (fs:KEY:TYPE (SET:KEY) ) */
inline std::string make_key_list ( const std::string& key /** @param key node key. */ ) {
    return make_key( KEY_FS, key, KEY_LIST );
}
/** @brief make node list key (fs:TYPE:list (SET:KEY) ) */
inline std::string make_key_list ( const NodeType::Enum& type /** @param type node type. */ ) {
    return make_key( KEY_FS, NodeType::str ( type ), KEY_LIST );
}

// -----------------------------------------------------------------------------------------------------------
// --------------------------                    database utils                     --------------------------
// -----------------------------------------------------------------------------------------------------------

/** @brief Make redis connection. */

static std::map< std::string, std::string > to_map ( std::vector< std::string > in ) {
    std::map< std::string, std::string > _map;
    assert ( in.size() % 2 == 0 );

    for ( size_t i = 0; i < in.size(); ++++i ) {
        _map[ in.at ( i ) ] = in.at ( i + 1 );
    }

    return _map;
}

static redis_ptr make_connection ( const std::string db /** @param db database host */,
                                   int port /** @param port database port */ ) {
    redis_ptr _redis = redis_ptr ( new redox::Redox() );

    if ( !_redis->connect ( db, port ) ) {
        throw std::system_error ( std::error_code ( 300, std::generic_category() ),
                                  "unable to connect to database on localhost." );
    }
    return _redis;
}

/** @brief flush content with the given prefix. */ //TODO key with wildcard ':*'
static void flush(  redis_ptr redis /** @param redis the database pointer. */, const std::string& prefix /** @param prefix the prefix. */ )
{ redis->command ( { "EVAL", "local keys = redis.call('keys', ARGV[1]) \n for i=1,#keys,5000 do \n redis.call('del', unpack(keys, i, math.min(i+4999, #keys))) \n end \n return keys", "0", fmt::format( "{}:*", prefix ) } ); }

/** @brief get the node attribute value. */
static std::string get( redis_ptr redis /** @param redis the database pointer. */,
                        const std::string& key /** @param key redis key. */,
                        const std::string param /** @param param parameter name. */ ) {

    auto& _item = redis->commandSync< std::string > ( {REDIS_HGET, data::make_key_node ( key ), param} );
    /** @return the the value or empty string if not found. */
    return( _item.ok() ? _item.reply() : "" );
}

// -----------------------------------------------------------------------------------------------------------
// --------------------------                  content relations                    --------------------------
// -----------------------------------------------------------------------------------------------------------

/** @brief add node to parents type list */
inline void add_types( redis_ptr redis, const std::string& path, const std::string& key, const int& score )
{ redis->command( {REDIS_ZADD, data::make_key_list( path ), std::to_string( score ), key } ); }
/** @brief remove node from parents type list */
inline void rem_types( redis_ptr redis, NodeType::Enum type /* TODO remove */, const std::string& parent, const std::string& key )
{ redis->command( {REDIS_ZREM, data::make_key_list( parent ), hash( key ) } ); }

/** @brief add node to global nodes list */
inline void add_nodes( redis_ptr redis, NodeType::Enum type, const std::string& key ) //TODO add score
{ redis->command( {REDIS_ZADD,  data::make_key_list( type ), "0", key } ); }
/** @brief remove node from global nodes list */
inline void rem_nodes( redis_ptr redis, NodeType::Enum type, const std::string& key )
{ redis->command( {REDIS_ZREM,  data::make_key_list( type ), key } ); }

// -----------------------------------------------------------------------------------------------------------
// --------------------------                  directory content                    --------------------------
// -----------------------------------------------------------------------------------------------------------

/** @brief get the node by key */
static node_t node ( redis_ptr redis /** @param redis redis database pointer. */,
                     const std::string& key /** @param key the node key. */ ) {

    auto& _item = redis->commandSync< std::vector< std::string > > ( { REDIS_HGETALL, make_key_node ( key ) } );
    /** @return the the node or empty map if not found. */
    return( _item.ok() ? to_map( _item.reply() ) : node_t() );
}

/** @brief get the node children */ //TODO add params sort,order,tags,index,count
static void children( redis_ptr redis /** @param redis redis database pointer. */,
                      const std::string& key /** @param key the node key. */,
                      const int& index /** @param index start index. */,
                      const int& count /** @param count result size. */,
                      async_fn fn /** @param fn the callback function. */ ) {

    //TODO use defined type
    redox::Command< std::vector< std::string > >& _c = redis->commandSync< std::vector< std::string > >
            ( {REDIS_ZRANGE, make_key_list( key ), std::to_string( index ), std::to_string( index + count ) } );
    if( _c.ok() ) {
        for( const std::string& __c : _c.reply() ) {
            fn( __c );
        }
    }
}

/** @brief get the node children count */ //TODO add params sort,order,tags
static int children_count( redis_ptr redis, const std::string& key ) {
    redox::Command< int >& _c = redis->commandSync< int >(
        { REDIS_ZCARD, make_key_list( key ) }
    );
    return( _c.ok() ? _c.reply() : 0 );
}

/** @brief store the node in redis. */
static void save( redis_ptr redis, const std::string& key, node_t& node ) {
    auto _command = std::vector< std::string >( { REDIS_HMSET, make_key_node ( key) } );
    for ( auto __i : node ) {
        _command.push_back ( __i.first );
        _command.push_back ( __i.second );
    }
    redis->command ( _command );
    add_types( redis, node[KEY_PARENT], key, 0 /*TODO score*/ );
}

/** @brief create a new tag. */
static void add_tag( redis_ptr redis /** @param redis redis database pointer. */,
                            const std::string& name /** @param name name of the tag */,
                            const std::string& keyword /** @param keyword the tag */,
                            const NodeType::Enum& type /** @param type the type for the tag */,
                            const std::string& node /** @param node the node key */,
                            float score /** @param score the score of the node index */ ) {

    redis->command( { REDIS_ZADD, make_key( KEY_FS, NodeType::str( type ), KEY_TAG, KEY_LIST ), std::to_string( score ), name } );
    redis->command( { REDIS_ZADD, make_key( KEY_FS, NodeType::str( type ), KEY_TAG, name ), std::to_string( score ), keyword } );
    redis->command( { REDIS_HMSET, make_key( KEY_FS, NodeType::str( type ), KEY_TAG ), keyword, node } );
}

/** @brief check or update the timestamp of the node. */
static bool timestamp( redis_ptr redis /** @param redis redis database pointer. */,
                       const std::string& key /** @param key key of the node. */,
                       size_t timestamp /** @param timestamp the last write timestamp. */ ) {

    auto& _exist = redis->commandSync< int > ( { REDIS_EXISTS, make_key ( KEY_FS, key, KEY_TIMESTAMP ) } );
    if( _exist.ok() && _exist.reply() == 1 ) {
        size_t _timestamp = std::stoul( redis->get( make_key ( KEY_FS, key, KEY_TIMESTAMP ) ) );
        if(  _timestamp == timestamp )
        { return true; }
    }
    redis->set( make_key ( KEY_FS, key, KEY_TIMESTAMP ), std::to_string( timestamp ) );
    return false;
}

/** @brief add new item in datastore. */
static void new_item( redis_ptr redis /** @param redis redis database pointer. */,
                      const std::string& parent /** @param parent parent key for audiofiles. */,
                      const std::string& key /** @param key key of the node. */,
                      const NodeType::Enum type /** @param type type of the node. */ ) {
    if( type == NodeType::audio ) {
        redis->command( {REDIS_ADD,  make_key( KEY_FS, KEY_NEW, NodeType::str( type ) ), parent} );
    } else {
        redis->command( {REDIS_ADD,  make_key( KEY_FS, KEY_NEW, NodeType::str( type ) ), key} );
    }
}

/** @brief iterate new items by type. */
static void new_items( redis_ptr redis, const NodeType::Enum type, async_fn fn ) {

    auto& _c = redis->commandSync< nodes_t >(
        {REDIS_MEMBERS, make_key( KEY_FS, KEY_NEW, NodeType::str( type ) ) } );

    if( _c.ok() ) {
        for( const std::string& __c : _c.reply() ) {
            fn( __c );
            redis->command( {REDIS_REM, make_key( KEY_FS, KEY_NEW, NodeType::str( type ) ), __c } );
        }
    }
}

/** @brief add mime type to list and increment mime counter. */
inline void incr_mime ( data::redis_ptr redis /** @param redis redis database pointer. */,
                        const std::string& mime_type /** @param mime_type the mime type.*/ ) {
    //reference in mime-type set
    redis->command ( { REDIS_ADD, KEY_MIME_LIST, mime_type } );
    //incr the mime_type counter
    redis->command ( { "INCR", make_key ( KEY_FS, KEY_MIME, mime_type ) } );
}

// -----------------------------------------------------------------------------------------------------------
// --------------------------                    configuration                      --------------------------
// -----------------------------------------------------------------------------------------------------------

/** @brief get the configuration as json string from the database. */
inline std::string config ( redis_ptr redis /** @param redis redox database. */ ) {
    return redis->get ( KEY_CONFIG );
}
/** @brief store the configuration as json string in the database. */
inline void config ( redis_ptr redis, /** @param redis redox database. */
                     const std::string& json /** @param c the configuration as json string. */ ) {
    redis->set ( KEY_CONFIG, json );
}
/** @brief check if a configuration exists in the database. */
inline bool config_exists ( redis_ptr redis /** @param redis redox database pointer. */ ) {
    redox::Command<int>& c = redis->commandSync<int> ( { REDIS_EXISTS, KEY_CONFIG} );
    return ( c.ok() && c.reply() );
}
}//namespace data
#endif // DATASTORE_H
