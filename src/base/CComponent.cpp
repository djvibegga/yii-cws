#include "base/CComponent.h"
#include "base/CEvent.h"
#include "interfaces.h"

CComponent::CComponent()
: _e(TEventHandlersMap())
{
}

CComponent::~CComponent()
{
}

CComponent & CComponent::attachEventHandler(const string & event, IEventHandler *handler)
{
	TEventHandlerList list;
	if (_e.find(event) == _e.end()) {
		list = TEventHandlerList();
	} else {
		list = _e[event];
	}
	list.insert(list.begin(), handler);
	_e[event] = list;
	return *this;
}

bool CComponent::hasEventHandler(const string & event)
{
	return _e.find(event) != _e.end();
}

void CComponent::raiseEvent(const string &name, CEvent &event)
{
	for (TEventHandlersMap::iterator iter = _e.begin(); iter != _e.end(); ++iter) {
		TEventHandlerList handlers = iter->second;
		for (TEventHandlerList::iterator handler = handlers.begin(); handler != handlers.end(); ++handler) {
			(*handler)->handleEvent(name, event);
		}
	}
}

void CComponent::attachBehavior(CBehavior * behavior)
{
	TEventNameList events = behavior->events();
	for (TEventNameList::iterator i = events.begin(); i != events.end(); ++i) {
		attachEventHandler(*i, behavior);
	}
	behavior->attach(this);
}
