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
#include "defines.h"
#include "CException.h"

using namespace std;

typedef map<string, string> TReplacementMap;

class CStringUtils
{
private:
	static const unsigned int _TIME_STR_BUFFER_LENGTH;

public:
	static string ltrim(const string & needle, const string & what = " ");
	static string rtrim(const string & needle, const string & what = " ");
	static string trim(const string & needle, const string & what = " ");
	static string upperCase(const string & needle);
	static string lowerCase(const string & needle);
	static string strtr(const string & needle, const TReplacementMap & replacement);
	static string implode(const string &glue, const vector<string> &pieces);
	static wstring implode(const wstring &glue, const vector<wstring> &pieces);
	static vector<string> explode(const string &separator, const string & needle, unsigned int limit = 0);
	static vector<boost::smatch> regexMatchAll(const string & needle, const boost::regex matcher);
	static string urlEncode(const string & src);
	static string urlDecode(const string & src);
	static string fromTimestamp(const string & format, time_t timestamp, bool isGmt = false);
	static int toInt(const string & str);
	static string fromInt(int from);
	static string fromBool(bool from);
	static string fromLong(long from);
	static string fromULong(unsigned long from);
	static string fromDouble(double from);
	static string base64Encode(const string & str);
	static string base64Decode(const string & str);

	static bool lengthComparator(const string & a, const string & b);

	static inline std::wstring utf8_to_wide(const std::string & text) {
	#ifndef _MSC_VER
			return boost::locale::conv::to_utf<wchar_t>(text, "UTF-8");
	#else
			// Calculate the required length of the buffer
			const size_t len_needed = ::MultiByteToWideChar(CP_UTF8, 0, text.c_str(), (UINT)(text.length()) , NULL, 0 );
			boost::scoped_array<wchar_t> buff(new wchar_t[len_needed+1]) ;
			const size_t num_copied = ::MultiByteToWideChar(CP_UTF8, 0, text.c_str(), text.size(), buff.get(), len_needed+1) ;
			return std::wstring(buff.get(), num_copied) ;
	#endif
		}

	static inline std::string wide_to_utf8(const std::wstring& text) {
	#ifndef _MSC_VER
			return boost::locale::conv::from_utf<>(text, "UTF-8");
	#else
			const size_t len_needed = ::WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (UINT)(text.length()) , NULL, 0, NULL, NULL) ;
			boost::scoped_array<char> buff(new char[len_needed+1]) ;
			const size_t num_copied = ::WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (UINT)(text.length()) , buff.get(), len_needed+1, NULL, NULL) ;
			return std::string(buff.get(), num_copied) ;
	#endif
		}
};

#ifdef _UNICODE
#define _to_utf8(data) CStringUtils::wide_to_utf8(data)
#define utf8_to_(data) CStringUtils::utf8_to_wide(data)
#define wide_to_(data) data
#else
#define _to_utf8(data) data
#define utf8_to_(data) data
#define wide_to_(data) CStringUtils::wide_to_utf8(data)
#endif

#endif /* CSTRINGUTILS_H_ */
