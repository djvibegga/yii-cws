/*
 * CHttpSesion.h
 *
 *  Created on: Feb 22, 2014
 *      Author: djvibegga
 */

#ifndef CHTTPSESION_H_
#define CHTTPSESION_H_

#include "base/CApplicationComponent.h"
#include "base/CException.h"
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>

typedef map<string, string> TSessionDataMap;

class CHttpSession: public CApplicationComponent
{
	friend class boost::serialization::access;

private:
	static const string SESSION_ID_KEY;
	static const long int DEFAULT_GC_SESSIONS_TIMEOUT;
	string _sessionId;
	TSessionDataMap _sessionData;
	static bool _isGCRunned;
	static boost::mutex _gcMutexLocker;

public:
	long int gcTimeout;
	bool autoOpen;

	CHttpSession();
	CHttpSession(CWebApplication * app);
	CHttpSession(const string &id, CWebApplication * app);
	CHttpSession(const CHttpSession & other);
	virtual void init();
	void setSessionId(const string & sessionId);
	string getSessionId() const;
	TSessionDataMap & getData();
	string & operator[](const string & key);
	virtual void reset();
	virtual bool open() throw (CException);
	virtual void close() throw (CException);
	virtual _string read(const string & sessionId) const;
	virtual bool write(const string & sessionId, const _string & data) const;
	virtual void destroy(const string & sessionId) const throw (CException);
	virtual void gcSessions() const throw (CException);

protected:
	boost::filesystem::path sessionsPath;

	void ensureGCRunned() throw (CException);
	virtual boost::filesystem::path resolveSessionFilePath() const;
	virtual void applyConfig(const xml_node & config);
	virtual string resolveSessionId() const;
	virtual string generateUniqueSessionId() const;
	virtual bool serializeData(string & dest);
	virtual bool unserializeData(const string & src);

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & _sessionData;
	}
};


class CHttpSessionGCRunner: public IRunable
{
private:
	CWebApplication * _instance;
	long int _timeout;

public:
	CHttpSessionGCRunner(CWebApplication * app, long int timeout);
	virtual ~CHttpSessionGCRunner();
	virtual void init() throw (CException);
	virtual void run() throw (CException);
};

#endif /* CHTTPSESION_H_ */