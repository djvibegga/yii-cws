#include "TestBehavior.h"
#include <base/Jvibetto.h>

TEventsMap TestBehavior::events()
{
	TEventsMap map;
	map["onBeginRequest"] = EVENT_HANDLER(&TestBehavior::logRequest);
	return map;
}

void TestBehavior::logRequest(CEvent & event)
{
	Jvibetto::log("testing message from behavior");
}
