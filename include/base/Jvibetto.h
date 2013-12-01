/*
 * Jvibetto.h
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#ifndef YIIBASE_H_
#define YIIBASE_H_

#include <base/CApplication.h>

typedef map<string, boost::filesystem::path> TPathAliasMap;

class Jvibetto
{
private:
    static TPathAliasMap _aliases;

public:
	static CApplication * app();
	static CLogger & getLogger();
	static void log(
		const string & message,
		const string & level = CLogger::LEVEL_INFO,
		const string & category = "application");
	static void trace(const string & message, const string & category = "application");

	static void setPathOfAlias(const string & alias, const boost::filesystem::path & path);
	static boost::filesystem::path getPathOfAlias(const string & alias);
};

#endif /* YIIBASE_H_ */