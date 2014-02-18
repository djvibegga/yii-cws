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

typedef map<_string, _string> TTagAttributesMap;

class CHtml
{
private:
	static const TTagAttributesMap _specialAttributes;

public:
	static _string tag(
			const _string & tag,
			const TTagAttributesMap & htmlOptions,
			const _string & content,
			bool closeTag = true);
	static _string tag(
		const _string & tag,
		const TTagAttributesMap & htmlOptions = TTagAttributesMap(),
		bool closeTag = true);
	static _string metaTag(
		const _string & content,
		const _string name = _(""),
		const _string httpEquiv = _(""),
		const TTagAttributesMap & options = TTagAttributesMap());
	static _string linkTag(
		const _string & relation = _(""),
		const _string & type = _(""),
		const _string & href = _(""),
		const _string & media = _(""),
		const TTagAttributesMap & options = TTagAttributesMap());
	static _string css(const _string & text, const _string & media = _(""));
	static _string cssFile(const string & url, const _string & media = _(""));
	static _string script(const _string & text, const _string & type = _("text/javascript"));
	static _string scriptFile(const string & url, const _string & type = _("text/javascript"));
	static _string renderAttributes(const TTagAttributesMap & htmlOptions);
	static _string encode(const _string & text);
};

#endif /* CHTML_H_ */
