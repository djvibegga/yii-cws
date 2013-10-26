#include "base/CComponent.h"
#include "base/CEvent.h"
#include "interfaces.h"
#include "logging/CLogRouter.h"

CComponent::CComponent()
: _e(TEventHandlersMap())
{
}

CComponent::~CComponent()
{
}

CComponent & CComponent::attachEventHandler(const string & event, IEventHandler * object, TEventHandler method)
{
	SEventHandler callee;
	callee.object = object;
	callee.method = method;
	return attachEventHandler(event, callee);
}

CComponent & CComponent::attachEventHandler(const string & event, const SEventHandler & callee)
{
	TEventHandlerList list;
	if (_e.find(event) == _e.end()) {
		list = TEventHandlerList();
	} else {
		list = _e[event];
	}
	list.insert(list.begin(), callee);
	_e[event] = list;
	return *this;
}

bool CComponent::hasEventHandler(const string & event)
{
	return _e.find(event) != _e.end();
}

void CComponent::raiseEvent(const string &name, CEvent &event)
{
	TEventHandlerList handlers = _e[name];
	for (TEventHandlerList::iterator handler = handlers.begin(); handler != handlers.end(); ++handler) {
		(handler->object->*(handler->method))(event);
	}
}

void CComponent::attachBehavior(CBehavior * behavior)
{
	TEventsMap events = behavior->events();
	SEventHandler callee;
	for (TEventsMap::const_iterator i = events.begin(); i != events.end(); ++i) {
		callee.object = behavior;
		callee.method = i->second;
		attachEventHandler(i->first, callee);
	}
	behavior->attach(this);
}
