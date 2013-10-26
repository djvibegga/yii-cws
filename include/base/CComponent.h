#ifndef __C_COMPONENT_H__
#define __C_COMPONENT_H__

using namespace std;

#include <string>
#include <map>
#include <vector>
#include "interfaces.h"
#include "base/CBehavior.h"

struct SEventHandler
{
	IEventHandler * object;
	TEventHandler method;
};

typedef vector<SEventHandler> TEventHandlerList;
typedef map<string, TEventHandlerList> TEventHandlersMap;

class CComponent: public IEventHandler
{
private:
	TEventHandlersMap _e;

public:
	CComponent();
	virtual ~CComponent();
	CComponent & attachEventHandler(const string & event, IEventHandler * object, TEventHandler method);
	CComponent & attachEventHandler(const string & event, const SEventHandler & callee);
	virtual void raiseEvent(const string &name, CEvent &event);
	void attachBehavior(CBehavior * behavior);
	bool hasEventHandler(const string & event);

};

#endif
