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

using namespace std;

class CStringUtils
{
public:
	static string ltrim(const string & needle, const string & what);
	static string rtrim(const string & needle, const string & what);
	static string trim(const string & needle, const string & what);
	static string implode(const string &glue, const vector<string> &pieces);
	static string urlEncode(const string & src);
	static string urlDecode(const string & src);
};

#endif /* CSTRINGUTILS_H_ */
