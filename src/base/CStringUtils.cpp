/*
 * CStringUtils.cpp
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#include "base/CStringUtils.h"
#include <curl/curl.h>
#include <algorithm>
#include <cryptopp/base64.h>

using namespace std;
using namespace CryptoPP;

const unsigned int CStringUtils::_TIME_STR_BUFFER_LENGTH = 80;

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

string CStringUtils::upperCase(const string & needle)
{
	string ret = needle;
	transform(needle.begin(), needle.end(), ret.begin(), ::toupper);
	return ret;
}

string CStringUtils::lowerCase(const string & needle)
{
	string ret = needle;
	transform(needle.begin(), needle.end(), ret.begin(), ::tolower);
	return ret;
}

bool CStringUtils::lengthComparator(const string & a, const string & b)
{
	return a.length() >= b.length();
}

string CStringUtils::strtr(const string & needle, const TReplacementMap & replacement)
{
	vector<string> order;
	for (TReplacementMap::const_iterator iter = replacement.begin(); iter != replacement.end(); ++iter) {
		if (iter->first != iter->second) {
			order.push_back(iter->first);
		}
	}
	sort(order.begin(), order.end(), CStringUtils::lengthComparator);
	string ret = needle;
	std::string::size_type pos = 0;
	for (vector<string>::iterator iter = order.begin(); iter != order.end(); ++iter) {
		pos = ret.find(*iter);
		while (pos != ::string::npos) {
			ret = ret.substr(0, pos) + replacement.find(*iter)->second + ret.substr(pos + iter->length());
			pos = ret.find(*iter);
		}
	}
	return ret;
}

string CStringUtils::implode(const string &glue, const vector<string> &pieces)
{
	if (pieces.empty()) {
		return "";
	}
	string ret = *pieces.begin();
	for (vector<string>::const_iterator iter = ++pieces.begin(); iter != pieces.end(); ++iter) {
		ret += glue + *iter;
	}
	return ret;
}

wstring CStringUtils::implode(const wstring &glue, const vector<wstring> &pieces)
{
	if (pieces.empty()) {
		return L"";
	}
	wstring ret = *pieces.begin();
	for (vector<wstring>::const_iterator iter = ++pieces.begin(); iter != pieces.end(); ++iter) {
		ret += glue + *iter;
	}
	return ret;
}

vector<string> CStringUtils::explode(const string &separator, const string & needle, unsigned int limit)
{
    std::string::size_type pos = needle.find(separator);
	vector<string> ret;
	string buff = needle;
	while (pos != ::string::npos && (ret.size() < limit - 1 || limit == 0)) {
		ret.push_back(buff.substr(0, pos));
		buff = buff.substr(pos + 1);
		pos = buff.find(separator);
	}
	ret.push_back(buff);
	return ret;
}

vector<boost::smatch> CStringUtils::regexMatchAll(const string & needle, const boost::regex matcher)
{
	vector<boost::smatch> ret;
	boost::smatch matches;
	string::const_iterator start = needle.begin();
	string::const_iterator end = needle.end();
	while (boost::regex_search(start, end, matches, matcher)) {
		ret.push_back(matches);
		start = matches[0].second;
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

string CStringUtils::fromTimestamp(const string & format, time_t timestamp, bool isGmt)
{
	struct tm * timeinfo;
	if (isGmt) {
		timeinfo = gmtime(&timestamp);
	} else {
		timeinfo = localtime(&timestamp);
	}
	char buffer[_TIME_STR_BUFFER_LENGTH];
	string ret;
	strftime(buffer, _TIME_STR_BUFFER_LENGTH, format.c_str(), timeinfo);
	return string(buffer);
}

int CStringUtils::toInt(const string & str)
{
	return atoi(str.c_str());
}

string CStringUtils::fromInt(int from)
{
	stringstream ss;
	ss << from;
	return ss.str();
}

string CStringUtils::fromBool(bool from)
{
	return from ? "true" : "false";
}

string CStringUtils::fromLong(long from)
{
	stringstream ss;
	ss << from;
	return ss.str();
}

string CStringUtils::fromULong(unsigned long from)
{
	stringstream ss;
	ss << from;
	return ss.str();
}

string CStringUtils::fromDouble(double from)
{
	stringstream ss;
	ss << from;
	return ss.str();
}

string CStringUtils::base64Encode(const string & str)
{
	string ret;
	StringSource ss(str, true,
		new CryptoPP::Base64Encoder(
			new StringSink(ret),
			false // do not append a newline
		)
	);
	return ret;
}

string CStringUtils::base64Decode(const string & str)
{
	string ret;
	StringSource ss(str, true,
		new CryptoPP::Base64Decoder(
			new StringSink(ret)
		)
	);
	return ret;
}
