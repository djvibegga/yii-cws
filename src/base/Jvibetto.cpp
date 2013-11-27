/*
 * Yii.cpp
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#include <base/Jvibetto.h>
#include <base/CStringUtils.h>

TPathAliasMap Jvibetto::_aliases;

CApplication * Jvibetto::app()
{
	return CApplication::getInstance();
}

CLogger & Jvibetto::getLogger()
{
	return CApplication::getInstance()->getLogger();
}

void Jvibetto::log(
	const string & message,
	const string & level,
	const string & category)
{
	getLogger().log(message, level, category);
}

void Jvibetto::trace(const string & message, const string & category)
{
	log(message, CLogger::LEVEL_TRACE, category);
}

boost::filesystem::path Jvibetto::getPathOfAlias(const string & alias)
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
            vector<string> parts = CStringUtils::explode(".", alias.substr(pos + 1));
            string subAlias = alias.substr(pos);
            subAlias.replace(subAlias.begin(), subAlias.end(), ".", "/");
            return boost::filesystem::path(_aliases[rootAlias].string() + subAlias);
        }
    }
    return boost::filesystem::path("");
}

void Jvibetto::setPathOfAlias(const string & alias, const boost::filesystem::path & path)
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
