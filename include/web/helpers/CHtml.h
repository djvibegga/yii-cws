/*
 * CHtml.h
 *
 *  Created on: Feb 17, 2014
 *      Author: djvibegga
 */

#ifndef CHTML_H_
#define CHTML_H_

#include <map>
#include "defines.h"

typedef map<string, string> TTagAttributesMap;

class CHtml
{
private:
	static const TTagAttributesMap _specialAttributes;

public:
	static string tag(
			const string & tag,
			const TTagAttributesMap & htmlOptions,
			const string & content,
			bool closeTag = true);
	static string tag(
		const string & tag,
		const TTagAttributesMap & htmlOptions = TTagAttributesMap(),
		bool closeTag = true);
	static string metaTag(
		const string & content,
		const string name = "",
		const string httpEquiv = "",
		const TTagAttributesMap & options = TTagAttributesMap());
	static string linkTag(
		const string & relation = "",
		const string & type = "",
		const string & href = "",
		const string & media = "",
		const TTagAttributesMap & options = TTagAttributesMap());
	static string css(const string & text, const string & media = "");
	static string cssFile(const string & url, const string & media = "");
	static string script(const string & text, const string & type = "text/javascript");
	static string scriptFile(const string & url, const string & type = "text/javascript");
	static string renderAttributes(const TTagAttributesMap & htmlOptions);
	static string encode(const string & text);
};

#endif /* CHTML_H_ */
