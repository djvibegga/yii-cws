/*
 * CBehavior.h
 *
 *  Created on: Oct 14, 2013
 *      Author: djvibegga
 */

#ifndef CBEHAVIOR_H_
#define CBEHAVIOR_H_

class CComponent;
class CEvent;

#include <string>
#include "interfaces.h"

class CComponent;
class CEvent;

using namespace std;

class CBehavior: public IBehavior, public IEventHandler
{
private:
	bool _enabled;

public:
	CBehavior();
	virtual ~CBehavior();
	virtual void attach(CComponent * const component);
	virtual void detach(CComponent * const component);
	bool getEnabled();
	void setEnabled(bool value);
	virtual void handleEvent(const string &name, CEvent &event);
};


#endif /* CBEHAVIOR_H_ */
