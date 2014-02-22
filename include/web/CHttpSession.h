/*
 * CHttpSesion.h
 *
 *  Created on: Feb 22, 2014
 *      Author: djvibegga
 */

#ifndef CHTTPSESION_H_
#define CHTTPSESION_H_

#include "base/CApplicationComponent.h"
#include "base/cpptempl.h"
#include "base/CException.h"

using namespace cpptempl;

class CHttpSession: public CApplicationComponent
{
private:
	static const string SESSION_ID_KEY;
	static const long int DEFAULT_GC_SESSIONS_TIMEOUT;
	string _sessionId;
	data_map _sessionData;
	static bool _isGCRunned;
	void _runGarbageCollector(long int & interval);

public:
	long int gcTimeout;
	bool autoOpen;

	CHttpSession(CModule * module);
	CHttpSession(const string &id, CModule * module);
	void setSessionId(const string & sessionId);
	string getSessionId() const;
	data_map & getData();
	virtual void reset();
	virtual bool open() throw (CException);
	virtual void close() throw (CException);
	virtual _string read(const string & sessionId) const throw (CException);
	virtual bool write(const string & sessionId, const _string & data) const throw (CException);
	virtual void destroy(const string & sessionId) const throw (CException);
	virtual void gcSessions() const throw (CException);

protected:
	virtual void applyConfig(const xml_node & config);
	virtual string resolveSessionId() const;
	virtual string generateUniqueSessionId() const;
	virtual bool serializeData(_string & dest);
	virtual bool unserializeData(const _string & src);
};

#endif /* CHTTPSESION_H_ */
