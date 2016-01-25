/*
 * CFileLogRoute.cpp
 *
 *  Created on: Oct 23, 2013
 *      Author: djvibegga
 */

#include "logging/CFileLogRoute.h"
#include "base/Cws.h"
#include <boost/filesystem.hpp>
#include <stdio.h>

using namespace boost::filesystem;

CFileLogRoute::CFileLogRoute(const string & logFile)
: CLogRoute(),
 _logFile(logFile)
{
}

string CFileLogRoute::getClassName() const
{
	return "CFileLogRoute";
}

CFileLogRoute::~CFileLogRoute()
{
}

void CFileLogRoute::init() throw (CException)
{
	if (getLogPath().empty()) {
		setLogPath(string(Cws::app()->getRuntimePath().c_str()));
	}
}

boost::filesystem::path CFileLogRoute::getLogPath() const
{
	return _logPath;
}

void CFileLogRoute::setLogPath(const string & value) throw (CException)
{
	setLogPath(boost::filesystem::path(value));
}

void CFileLogRoute::setLogPath(const boost::filesystem::path & value) throw (CException)
{
	_logPath = value;
	if (_logPath.empty() || !is_directory(_logPath) || access(_logPath.c_str(), W_OK) == -1) {
		throw CException("CFileLogRoute.logPath '" + string(value.c_str())
			+ "' does not point to a valid directory. Make sure the directory exists and is writable by the Web server process.");
	}
}

string CFileLogRoute::getLogFile() const
{
	return _logFile;
}

void CFileLogRoute::setLogFile(const string & value)
{
	_logFile = value;
}

void CFileLogRoute::processLogs(TLogElementList & logs)
{
	string logFile = string(getLogPath().c_str()) + "/" + getLogFile();
	/*if (@filesize($logFile)>$this->getMaxFileSize()*1024)
		$this->rotateFiles();*/
	_IO_FILE * file = fopen(logFile.c_str(), "a");
	if (!file) {
		throw CException("Can\'t open log file for writing: '" + string(logFile.c_str()) + "'.");
	}
	flockfile(file);
	for (TLogElementList::const_iterator iter = logs.begin(); iter != logs.end(); ++iter) {
		fprintf(file, "%s\n", formatLogMessage(*iter).c_str());
	}
	funlockfile(file);
	fclose(file);
}
