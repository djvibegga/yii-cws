/*
 * MyUrlRule.h
 *
 *  Created on: Oct 22, 2013
 *      Author: djvibegga
 */

#ifndef MYURLRULE_H_
#define MYURLRULE_H_

#include <web/CUrlManager.h>

class MyUrlRule: public CUrlRule
{
public:
	MyUrlRule();
	virtual ~MyUrlRule();
	virtual string createUrl(
		const CUrlManager * const manager,
		TRouteStruct &route, const string & ampersand = "&") const;
};

#endif /* MYURLRULE_H_ */
