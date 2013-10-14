#include "base/CComponent.h"
#include "base/CEvent.h"
#include "interfaces.h"

CComponent::CComponent()
: _e(TEventHandlerList())
{
}

CComponent::~CComponent()
{
}

void CComponent::attachEventHandler(IEventHandler *handler)
{
	_e.insert(_e.begin(), handler);
}

void CComponent::raiseEvent(const string &name, CEvent &event)
{
	for (TEventHandlerList::iterator iter = _e.begin(); iter != _e.end(); ++iter) {
		(*iter)->handleEvent(name, event);
	}
}

void CComponent::attachBehavior(CBehavior * behavior)
{
	this->attachEventHandler(behavior);
	behavior->attach(this);
}
