#ifndef __C_COMPONENT_H__
#define __C_COMPONENT_H__

using namespace std;

#include <string>
#include <map>
#include <vector>
#include "interfaces.h"
#include "base/CBehavior.h"

typedef vector<IEventHandler*> TEventHandlerList;

class CComponent
{
private:
	TEventHandlerList _e;

public:
	CComponent();
	virtual ~CComponent();
	void attachEventHandler(IEventHandler *handler);
	virtual void raiseEvent(const string &name, CEvent &event);
	void attachBehavior(CBehavior * behavior);
};

#endif
