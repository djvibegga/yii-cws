#include "base/CEvent.h"
#include "base/CComponent.h"

CEvent::CEvent(void * sender)
{
	this->sender = sender;
	handled = false;
}

CEvent::CEvent(void * sender, const map<string, void*> &params)
{
	this->sender = sender;
	this->params = params;
	handled = false;
}
