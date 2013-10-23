/*
 * CStringUtils.h
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#ifndef CSTRINGUTILS_H_
#define CSTRINGUTILS_H_

#include <string>
#include <vector>
#include <map>
#include <boost/regex.hpp>

using namespace std;

typedef map<string, string> TReplacementMap;

class CStringUtils
{
public:
	static string ltrim(const string & needle, const string & what);
	static string rtrim(const string & needle, const string & what);
	static string trim(const string & needle, const string & what);
	static string upperCase(const string & needle);
	static string lowerCase(const string & needle);
	static string strtr(const string & needle, const TReplacementMap & replacement);
	static string implode(const string &glue, const vector<string> &pieces);
	static vector<string> explode(const string &separator, const string & needle);
	static vector<boost::smatch> regexMatchAll(const string & needle, const boost::regex matcher);
	static string urlEncode(const string & src);
	static string urlDecode(const string & src);
	static string dirName(const string & path);

	static bool lengthComparator(const string & a, const string & b);
};

#endif /* CSTRINGUTILS_H_ */
