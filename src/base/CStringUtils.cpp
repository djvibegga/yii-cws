/*
 * CStringUtils.cpp
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#include "base/CStringUtils.h"
#include <curl/curl.h>

string CStringUtils::ltrim(const string & needle, const string & what)
{
	string ret = needle;
	ret.erase(0, ret.find_first_not_of(what));
	return ret;
}

string CStringUtils::rtrim(const string & needle, const string & what)
{
	string ret = needle;
	ret.erase(ret.find_last_not_of(what) + 1, ret.max_size());
	return ret;
}

string CStringUtils::trim(const string & needle, const string & what)
{
	return rtrim(ltrim(needle, what), what);
}

string CStringUtils::implode(const string &glue, const vector<string> &pieces)
{
	string ret = *pieces.begin();
	for (vector<string>::const_iterator iter = ++pieces.begin(); iter != pieces.end(); ++iter) {
		ret += glue + *iter;
	}
	return ret;
}

string CStringUtils::urlEncode(const string & src)
{
	char * encoded = curl_escape(src.c_str(), 0);
	string ret(encoded);
	delete [] encoded;
	return ret;
}

string CStringUtils::urlDecode(const string & src)
{
	char * decoded = curl_unescape(src.c_str(), 0);
	string ret(decoded);
	delete [] decoded;
	return ret;
}
