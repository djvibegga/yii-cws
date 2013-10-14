/*
 * interfaces.h
 *
 *  Created on: Oct 14, 2013
 *      Author: djvibegga
 */

#ifndef INTERFACES_H_
#define INTERFACES_H_

#include <string>

using namespace std;

class CComponent;
class CEvent;

class IEventHandler
{
public:
	virtual ~IEventHandler() {}
	virtual void handleEvent(const string &name, CEvent &event) = 0;
};

class IBehavior
{
public:
	virtual ~IBehavior() {};
	virtual void attach(CComponent * const component) = 0;
	virtual void detach(CComponent * const component) = 0;
	bool getEnabled();
	void setEnabled(bool value);
};

#include "pugiconfig.hpp"
#include "pugixml.hpp"

using namespace pugi;

class IConfigureable
{
public:
	virtual ~IConfigureable() {}
	virtual void applyConfig(const xml_node & config) = 0;
};

#endif /* INTERFACES_H_ */
