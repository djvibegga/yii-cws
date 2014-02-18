/*
 * CMap.h
 *
 *  Created on: 27 ����. 2013 �.
 *      Author: djvibegga
 */

#ifndef CMAP_H_
#define CMAP_H_

#include <base/CComponent.h>

typedef map<string, string> TKeyValueMap;

class CMap: public CComponent
{
public:
    static vector<TKeyValueMap> fromKeyValueMapToArray(const TKeyValueMap & from, const string & key = "key", const string & value = "value");
};

#endif /* CMAP_H_ */
