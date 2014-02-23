/*
 * CHtml.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: djvibegga
 */

#include "web/helpers/CHtml.h"
#include "base/cpptempl.h"
#include <boost/assign.hpp>

const TTagAttributesMap CHtml::_specialAttributes = boost::assign::map_list_of
(_("async"), _("1"))
(_("autofocus"), _("1"))
(_("autoplay"), _("1"))
(_("checked"), _("1"))
(_("controls"), _("1"))
(_("declare"), _("1"))
(_("default"), _("1"))
(_("defer"), _("1"))
(_("disabled"), _("1"))
(_("formnovalidate"), _("1"))
(_("hidden"), _("1"))
(_("ismap"), _("1"))
(_("loop"), _("1"))
(_("multiple"), _("1"))
(_("muted"), _("1"))
(_("nohref"), _("1"))
(_("noresize"), _("1"))
(_("novalidate"), _("1"))
(_("open"), _("1"))
(_("readonly"), _("1"))
(_("required"), _("1"))
(_("reversed"), _("1"))
(_("scoped"), _("1"))
(_("seamless"), _("1"))
(_("selected"), _("1"))
(_("typemustmatch"), _("1"));

_string CHtml::tag(
	const _string & tag,
	const TTagAttributesMap & htmlOptions,
	const _string & content,
	bool closeTag)
{
	_string html = _("<") + tag + renderAttributes(htmlOptions) + _(">");
	return closeTag
		  ? html + content + _("</") + tag + _(">")
		  : html + content;
}

_string CHtml::tag(
	const _string & tag,
	const TTagAttributesMap & htmlOptions,
	bool closeTag)
{
	_string html = _("<") + tag + renderAttributes(htmlOptions);
	return closeTag ? html + _(" />") : html + _(">");
}

_string CHtml::metaTag(
	const _string & content,
	const _string name,
	const _string httpEquiv,
	const TTagAttributesMap & options)
{
	TTagAttributesMap opts = options;
	if (!name.empty()) {
		opts[_("name")] = name;
	}
	if (!httpEquiv.empty()) {
		opts[_("http-equiv")] = httpEquiv;
	}
	opts[_("content")] = content;
	return tag(_("meta"), opts);
}

_string CHtml::linkTag(
	const _string & relation,
	const _string & type,
	const _string & href,
	const _string & media,
	const TTagAttributesMap & options)
{
	TTagAttributesMap opts = options;
	if (!relation.empty()) {
		opts[_("rel")] = relation;
	}
	if (!type.empty()) {
		opts[_("type")] = type;
	}
	if (!href.empty()) {
		opts[_("href")] = href;
	}
	if (!media.empty()) {
		opts[_("media")] = media;
	}
	return tag(_("link"), opts);
}

_string CHtml::css(const _string & text, const _string & media)
{
	_string mediaAttr;
	if (!media.empty()) {
		mediaAttr = _(" media=\"") + media + _("\"");
	}
	return _("<style type=\"text/css\"")
		  + mediaAttr
		  + _(">\n/*<![CDATA[*/\n") + text + _("\n/*]]>*/\n</style>");
}

_string CHtml::cssFile(const string & url, const _string & media)
{
	return linkTag(_("stylesheet"), _("text/css"), utf8_to_(url), media);
}

_string CHtml::script(const _string & text, const _string & type)
{
	TTagAttributesMap htmlOptions;
	htmlOptions[_("type")] = type;
	return tag(
		_("script"), htmlOptions,
		_("\n/*<![CDATA[*/\n") + text + _("\n/*]]>*/\n"),
		true
	);
}

_string CHtml::scriptFile(const string & url, const _string & type)
{
	TTagAttributesMap htmlOptions;
	htmlOptions[_("type")] = type;
	htmlOptions[_("src")] = utf8_to_(url);
	return tag(_("script"), htmlOptions, _(""), true);
}

_string CHtml::renderAttributes(const TTagAttributesMap & htmlOptions)
{
	TTagAttributesMap options = htmlOptions;
	if (options.empty()) {
		return _("");
	}
	_string html;
	bool raw = false;
	TTagAttributesMap::const_iterator encodeFound = options.find(_("encode"));
	if (encodeFound != options.end()) {
		raw = options[_("encode")] != _("1");
		options.erase(_("encode"));
	}

	TTagAttributesMap::const_iterator foundSpecial;
	for (TTagAttributesMap::const_iterator iter = options.begin(); iter != options.end(); ++iter) {
		TTagAttributesMap::const_iterator specialFound = _specialAttributes.find(iter->first);
		if (specialFound == _specialAttributes.end()) {
			html += _(" ") + iter->first
				 + _("=\"") + (raw ? iter->second : encode(iter->second))
				 + _("\"");
		} else {
			html += _(" ") + iter->first
				 + _("=\"") + iter->first + _("\"");
		}
	}

	return html;
}

_string CHtml::encode(const _string & text)
{
	 _string buffer;
	 buffer.reserve(text.size());
	 for(size_t pos = 0; pos != text.size(); ++pos) {
		switch(text[pos]) {
		case _('&'):  buffer.append(_("&amp;"));
			break;
		case _('\"'): buffer.append(_("&quot;"));
			break;
		case _('\''): buffer.append(_("&apos;"));
			break;
		case _('<'):  buffer.append(_("&lt;"));
			break;
		case _('>'):  buffer.append(_("&gt;"));
			break;
		default:
			buffer.append(&text[pos], 1);
			break;
		}
	}
	return buffer;
}
