/*
 * CStatePersister.h
 *
 *  Created on: Mar 19, 2015
 *      Author: djvibegga
 */

#ifndef CSTATEPERSISTER_H_
#define CSTATEPERSISTER_H_

#include "base/CApplicationComponent.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>

class CStatesMap
{
public:
	typedef map<string, _string>::const_iterator iterator;

private:
	friend class boost::serialization::access;
	map<string, _string> _data;

public:
	_string & operator[](const string & key)
	{
		return _data[key];
	}

	void erase(const string & key)
	{
		_data.erase(key);
	}

	const map<string, _string>::const_iterator begin()
	{
		return _data.begin();
	}

	const map<string, _string>::const_iterator end()
	{
		return _data.end();
	}

	const map<string, _string>::const_iterator find(const string & key)
	{
		return _data.find(key);
	}

	map<string, _string> getData() const
	{
		return _data;
	}


protected:
	template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & _data;
		}
};

class CStatePersister: public CApplicationComponent
{
public:
	string stateFile;
	string cacheID;

	CStatePersister();
	virtual string getClassName() const;
	virtual void init();
	virtual CStatesMap load();
	virtual void save(CStatesMap & states);
};

#endif /* CSTATEPERSISTER_H_ */
