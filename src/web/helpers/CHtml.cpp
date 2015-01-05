/*
 * CHtml.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: djvibegga
 */

#include "web/helpers/CHtml.h"
#include "base/CStringUtils.h"
#include <boost/assign.hpp>

const TTagAttributesMap CHtml::_specialAttributes = boost::assign::map_list_of
("async", "1")
("autofocus", "1")
("autoplay", "1")
("checked", "1")
("controls", "1")
("declare", "1")
("default", "1")
("defer", "1")
("disabled", "1")
("formnovalidate", "1")
("hidden", "1")
("ismap", "1")
("loop", "1")
("multiple", "1")
("muted", "1")
("nohref", "1")
("noresize", "1")
("novalidate", "1")
("open", "1")
("readonly", "1")
("required", "1")
("reversed", "1")
("scoped", "1")
("seamless", "1")
("selected", "1")
("typemustmatch", "1");

string CHtml::tag(
	const string & tag,
	const TTagAttributesMap & htmlOptions,
	const string & content,
	bool closeTag)
{
	string html = "<" + tag + renderAttributes(htmlOptions) + ">";
	return closeTag
		  ? html + content + "</" + tag + ">"
		  : html + content;
}

string CHtml::tag(
	const string & tag,
	const TTagAttributesMap & htmlOptions,
	bool closeTag)
{
	string html = "<" + tag + renderAttributes(htmlOptions);
	return closeTag ? html + " />" : html + ">";
}

string CHtml::metaTag(
	const string & content,
	const string name,
	const string httpEquiv,
	const TTagAttributesMap & options)
{
	TTagAttributesMap opts = options;
	if (!name.empty()) {
		opts["name"] = name;
	}
	if (!httpEquiv.empty()) {
		opts["http-equiv"] = httpEquiv;
	}
	opts["content"] = content;
	return tag("meta", opts);
}

string CHtml::linkTag(
	const string & relation,
	const string & type,
	const string & href,
	const string & media,
	const TTagAttributesMap & options)
{
	TTagAttributesMap opts = options;
	if (!relation.empty()) {
		opts["rel"] = relation;
	}
	if (!type.empty()) {
		opts["type"] = type;
	}
	if (!href.empty()) {
		opts["href"] = href;
	}
	if (!media.empty()) {
		opts["media"] = media;
	}
	return tag("link", opts);
}

string CHtml::css(const string & text, const string & media)
{
	string mediaAttr;
	if (!media.empty()) {
		mediaAttr = " media=\"" + media + "\"";
	}
	return "<style type=\"text/css\""
		  + mediaAttr
		  + ">\n/*<![CDATA[*/\n" + text + "\n/*]]>*/\n</style>";
}

string CHtml::cssFile(const string & url, const string & media)
{
	return linkTag("stylesheet", "text/css", url, media);
}

string CHtml::script(const string & text, const string & type)
{
	TTagAttributesMap htmlOptions;
	htmlOptions["type"] = type;
	return tag(
		"script", htmlOptions,
		"\n/*<![CDATA[*/\n" + text + "\n/*]]>*/\n",
		true
	);
}

string CHtml::scriptFile(const string & url, const string & type)
{
	TTagAttributesMap htmlOptions;
	htmlOptions["type"] = type;
	htmlOptions["src"] = url;
	return tag("script", htmlOptions, "", true);
}

string CHtml::renderAttributes(const TTagAttributesMap & htmlOptions)
{
	TTagAttributesMap options = htmlOptions;
	if (options.empty()) {
		return "";
	}
	string html;
	bool raw = false;
	TTagAttributesMap::const_iterator encodeFound = options.find("encode");
	if (encodeFound != options.end()) {
		raw = options["encode"] != "1";
		options.erase("encode");
	}

	TTagAttributesMap::const_iterator foundSpecial;
	for (TTagAttributesMap::const_iterator iter = options.begin(); iter != options.end(); ++iter) {
		TTagAttributesMap::const_iterator specialFound = _specialAttributes.find(iter->first);
		if (specialFound == _specialAttributes.end()) {
			html += " " + iter->first
				 + "=\"" + (raw ? iter->second : encode(iter->second))
				 + "\"";
		} else {
			html += " " + iter->first
				 + "=\"" + iter->first + "\"";
		}
	}

	return html;
}

string CHtml::encode(const string & text)
{
	 string buffer;
	 buffer.reserve(text.size());
	 for(size_t pos = 0; pos != text.size(); ++pos) {
		switch(text[pos]) {
		case '&':  buffer.append("&amp;");
			break;
		case '\"': buffer.append("&quot;");
			break;
		case '\'': buffer.append("&apos;");
			break;
		case '<':  buffer.append("&lt;");
			break;
		case '>':  buffer.append("&gt;");
			break;
		default:
			buffer.append(&text[pos], 1);
			break;
		}
	}
	return buffer;
}
