/*
 * CFileLogRoute.h
 *
 *  Created on: Oct 23, 2013
 *      Author: djvibegga
 */

#ifndef CFILELOGROUTE_H_
#define CFILELOGROUTE_H_

#include "logging/CLogRoute.h"
#include "base/CException.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

class CFileLogRoute: public CLogRoute
{
private:
	boost::filesystem::path _logPath;
	string _logFile;

public:
	CFileLogRoute(const string & logFile = "application.log");
	virtual ~CFileLogRoute();
	boost::filesystem::path getLogPath() const;
	void setLogPath(const string & value) throw (CException);
	void setLogPath(const boost::filesystem::path & value) throw (CException);
	string getLogFile() const;
	void setLogFile(const string & value);
	virtual void init() throw (CException);

protected:
	virtual void processLogs(TLogElementList & logs);
};

#endif /* CFILELOGROUTE_H_ */
