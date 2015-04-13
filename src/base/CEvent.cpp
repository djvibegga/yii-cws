#include "base/CEvent.h"
#include "base/CComponent.h"

CEvent::CEvent(CComponent * const sender)
{
	this->sender = sender;
	handled = false;
}

CEvent::CEvent(CComponent * const sender, const map<string, void*> &params)
{
	this->sender = sender;
	this->params = params;
	handled = false;
}

string CEvent::getClassName() const
{
	return "CEvent";
}
