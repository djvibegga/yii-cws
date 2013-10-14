#ifndef __C_EVENT_H__
#define __C_EVENT_H__

using namespace std;

#include <map>
#include "base/CComponent.h"

class CEvent: public CComponent
{
public:
	void * sender;
	bool handled;
	map<string, void*> params;

	CEvent(void * sender);
	CEvent(void * sender, const map<string, void*> &params);
};

#endif
