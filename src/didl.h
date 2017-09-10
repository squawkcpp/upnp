#ifndef DIDL_H
#define DIDL_H

#include <string>
#include <map>

#include "rapidxml_ns.hpp"
#include "rapidxml_ns_print.hpp"
#include "rapidxml_ns_utils.hpp"

#include "config.h"
#include "datastore.h"

namespace upnp {
/** @brief write didl metadata to xml. */
struct Didl {
public:
    /** @brief Didl CTOR. */
    Didl( data::redis_ptr redis, config_t config );
    /** @brief Didl DTOR. */
    ~Didl() {}
    void write( const std::string& key, const int& child_count, const std::map< std::string, std::string >& values );

    /** @brief write xml to string. */
    std::string str() {
        std::string s = "<?xml version=\"1.0\"?>\n";
        rapidxml_ns::print( std::back_inserter(s), doc_, rapidxml_ns::print_no_indenting );
        return s;
    }
    /** @brief number of objects written to xml */
    int count() const { return result_; }
private:
    data::redis_ptr redis_;
    config_t config_;
    int result_;
    rapidxml_ns::xml_document<> doc_;
    rapidxml_ns::xml_node<>* root_node_;
};
}//namespace upnp
#endif // DIDL_H
