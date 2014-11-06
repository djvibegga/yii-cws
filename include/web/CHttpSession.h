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
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>

typedef map<string, _string> TSessionDataMap;

class CHttpSession: public CApplicationComponent
{
	friend class boost::serialization::access;

private:
	static const string SESSION_ID_DEFAULT_KEY;
	static const long int DEFAULT_GC_SESSIONS_TIMEOUT;
	static const long int DEFAULT_SESSION_LIFE_TIME;
	static const string DEFAULT_SESSION_STATE_FILE_EXTENSION;
	string _sessionId;
	TSessionDataMap _sessionData;
	static bool _isGCRunned;
	static boost::uuids::basic_random_generator<boost::mt19937> _gen;
	static boost::mutex _gcMutexLocker;
	static boost::mutex _idGeneratorLocker;


public:
	static const string LOG_CATEGORY;

	long int gcTimeout;
	bool autoOpen;
	long int lifeTime;
	bool passSessionIdByRequestParam;
	bool passSessionIdByCookie;
	string sessionIdRequestParamName;
	string sessionIdCookieName;

	CHttpSession();
	CHttpSession(CWebApplication * app);
	CHttpSession(const string &id, CWebApplication * app);
	CHttpSession(const CHttpSession & other);
	virtual void init();
	void setSessionId(const string & sessionId);
	string getSessionId() const;
	string regenerateId(bool deleteOldSession = false);
	TSessionDataMap & getData();
	_string & operator[](const string & key);
	virtual void reset();
	virtual bool open() throw (CException);
	virtual void close() throw (CException);
	virtual _string read(const string & sessionId) const;
	virtual bool write(const string & sessionId, const _string & data) const;
	virtual void destroy(const string & sessionId = "") throw (CException);
	virtual void gcSessions() const throw (CException);

protected:
	boost::filesystem::path sessionsPath;

	void ensureGCRunned() throw (CException);
	virtual boost::filesystem::path resolveSessionFilePath(const string & sessionId) const;
	virtual void applyConfig(const xml_node & config);
	virtual string resolveSessionId() const;
	virtual string generateUniqueSessionId() const;
	virtual bool serializeData(string & dest);
	virtual bool unserializeData(const string & src);
	virtual void saveSessionIdIntoCookies(const string & sessionId);

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
