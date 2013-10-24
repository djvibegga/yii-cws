#ifndef __C_EVENT_H__
#define __C_EVENT_H__

using namespace std;

#include <map>
#include "base/CComponent.h"

typedef map<string, void*> TEventParams;

class CEvent: public CComponent
{
public:
	const CComponent * sender;
	bool handled;
	TEventParams params;

	CEvent(CComponent * const sender);
	CEvent(CComponent * const sender, const TEventParams &params);
};

#endif
