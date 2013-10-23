/*
 * MyUrlRule.cpp
 *
 *  Created on: Oct 22, 2013
 *      Author: djvibegga
 */

#include "MyUrlRule.h"

MyUrlRule::MyUrlRule()
: CUrlRule("", "")
{
}

MyUrlRule::~MyUrlRule()
{
}

string MyUrlRule::createUrl(
	const CUrlManager * const manager,
	TRouteStruct &route, const string & ampersand) const
{
	if (route.path == "site/admin") {
		string ret = "admin";
		for (TRequestParams::iterator iter = route.params.begin(); iter != route.params.end(); ++iter) {
			ret += "/" + iter->second;
		}
		return ret;
	}
	return "";
}
