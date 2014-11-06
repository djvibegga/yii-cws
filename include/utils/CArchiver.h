/*
 * CArchiver.h
 *
 *  Created on: Nov 4, 2014
 *      Author: djvibegga
 */

#ifndef CARCHIVER_H_
#define CARCHIVER_H_

#include <string>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

using namespace std;

template<class T>
class CArchiver
{
public:
	static string save(T & data)
	{
		stringstream os;
		boost::archive::text_oarchive oa(os);
		oa << data;
		return os.str();
	}

	static void load(const string & src, T & output)
	{
		stringstream is;
		is << src;
		boost::archive::text_iarchive ia(is);
		ia >> output;
	}
};

#endif /* CARCHIVER_H_ */
