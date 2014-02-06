/*
 * Goal.cpp
 *
 *  Created on: Feb 5, 2014
 *      Author: djvibegga
 */

#include "Goal.h"

IMPLEMENT_AR_CLASS(Goal)

Goal::Goal(const string & scenario, CDbConnection * connection)
: CActiveRecord(scenario, connection),
  id(0)
{
}

Goal::Goal(const Goal & other)
: CActiveRecord(other.getScenario(), other.getDbConnection()),
  id(other.id)
{

}

Goal::~Goal()
{
}

CActiveRecord * Goal::instantiate(const TDbRow & attributes) const
{
	return new Goal("", getDbConnection());
}

string Goal::tableName() const
{
	return "goals";
}

void Goal::populateProperty(const string & name, const SAField * value)
{
	if (name == "id") {
		id = value->asULong();
	} else if (name == "name") {
		this->name = value->asString().GetMultiByteChars();
	}
}
