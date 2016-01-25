/*
 * Yii.cpp
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#include <base/Cws.h>
#include <base/CStringUtils.h>
#include <stdarg.h>

TPathAliasMap Cws::_aliases;

CApplication * Cws::app()
{
	return CApplication::getInstance();
}

CLogger & Cws::getLogger()
{
	return CApplication::getInstance()->getLogger();
}

void Cws::log(
	const string & message,
	const string & level,
	const string & category)
{
	getLogger().log(message, level, category);
}

void Cws::trace(const string & message, const string & category)
{
	log(message, CLogger::LEVEL_TRACE, category);
}

boost::filesystem::path Cws::getPathOfAlias(const string & alias)
{
    TPathAliasMap::const_iterator iter = _aliases.find(alias);
    if (iter != _aliases.end()) {
        return _aliases[alias];
    } else {
        std::string::size_type pos = alias.find(".");
        if (pos == ::string::npos) {
            return boost::filesystem::path("");
        }
        string rootAlias = alias.substr(0, pos);
        iter = _aliases.find(rootAlias);
        if (iter != _aliases.end()) {
            string subAlias = alias.substr(pos);
            std::replace(subAlias.begin(), subAlias.end(), '.', '/');
            return boost::filesystem::path(_aliases[rootAlias].string() + subAlias);
        }
    }
    return boost::filesystem::path("");
}

void Cws::setPathOfAlias(const string & alias, const boost::filesystem::path & path)
{
    if (path.empty()) {
        TPathAliasMap::iterator iter = _aliases.find(alias);
        if (iter != _aliases.end()) {
            _aliases.erase(iter, ++iter);
        }
    } else {
        _aliases[alias] = boost::filesystem::canonical(path);
    }
}

_string Cws::t(const _string & message)
{
	CApplication * app = CApplication::getInstance();
	const char * category = app->name.c_str();
	return t(message, category);
}

_string Cws::t(const _string & message, char const * category)
{
	CApplication * app = CApplication::getInstance();
	std::locale loc = app->getLocaleByLanguage(app->getLanguage());
	return boost::locale::dgettext(category, message.c_str(), loc);
}
