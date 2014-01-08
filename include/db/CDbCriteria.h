/*
 * CDbCriteria.h
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#ifndef CDBCRITERIA_H_
#define CDBCRITERIA_H_

#include "db/CDbCommand.h"
#include <string>
#include <vector>

using namespace std;

typedef vector<string> TDbCriteriaWithList;
typedef vector<string> TDbCriteriaScopeList;

class CDbCriteria
{
private:
	string _addColumnCondition(const string & column, const string & comparison, const string & op);

public:

	const static string PARAM_PREFIX;
	static int paramCount;
	string select;
	bool distinct;
	string condition;
	TCommandParameterMap params;
    int limit;
	int offset;
	string order;
	string $group;
	string join;
	string having;
	TDbCriteriaWithList with;
	string $alias;
	bool together;
	int index;
	TDbCriteriaScopeList scopes;

	CDbCriteria();
	virtual ~CDbCriteria();

	CDbCriteria & addCondition(const string & condition, const string & op = "AND");
	CDbCriteria & addSearchCondition(
		const string & column, string & keyword,
		bool escape = true, const string & op = "AND", const string & like = "LIKE"
	);
	CDbCriteria & addInCondition(const string & column, const vector<string> & values, const string op = "AND");
	CDbCriteria & addNotInCondition(const string & column, const vector<string> & values, const string op = "AND");
	CDbCriteria & compare(
		const string & column, string & value, const string & comparison = "",
		bool partialMatch = false, const string & op = "AND", bool escape = true
	);
	CDbCriteria & compare(
		const string & column, long & value,
		const string & comparison = "", const string & op = "AND"
	);
	CDbCriteria & compare(
		const string & column, unsigned long & value,
		const string & comparison = "", const string & op = "AND"
	);
	CDbCriteria & compare(
		const string & column, double & value,
		const string & comparison = "", const string & op = "AND"
	);
	CDbCriteria & addBetweenCondition(const string & column, const long & valueStart, const long & valueEnd, const string & op = "AND");
};

#endif /* CDBCRITERIA_H_ */
