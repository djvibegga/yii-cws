/*
 * CMap.cpp
 *
 *  Created on: 27 םמ‗ב. 2013 ד.
 *      Author: djvibegga
 */

#include <utils/CMap.h>
#include <boost/assign.hpp>

vector<TKeyValueMap> CMap::fromKeyValueMapToArray(const TKeyValueMap & from, const string & key, const string & value)
{
    vector<TKeyValueMap> ret;
    for (TKeyValueMap::const_iterator iter = from.begin(); iter != from.end(); ++iter) {
        ret.push_back(boost::assign::map_list_of(key, iter->first) (value, iter->second));
    }
    return ret;
}
