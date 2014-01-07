/*
 * CDbCriteria.cpp
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#include "db/CDbCriteria.h"
#include "base/CStringUtils.h"
#include <sstream>
#include <boost/assign.hpp>

const string CDbCriteria::PARAM_PREFIX = "jnp";
int CDbCriteria::paramCount = 0;

CDbCriteria::CDbCriteria()
: select("*"),
  distinct(false),
  limit(-1),
  offset(-1),
  together(false),
  index(0)
{

}

CDbCriteria::~CDbCriteria()
{
}

CDbCriteria & CDbCriteria::addCondition(const string & condition, const string & op)
{
	if (this->condition.empty()) {
		this->condition = condition;
	} else {
		this->condition = "(" + this->condition + ") " + op + " (" + condition + ")";
	}
	return *this;
}

CDbCriteria & CDbCriteria::addSearchCondition(
	const string & column, string & keyword,
	bool escape, const string & op, const string & like
)
{
	if (keyword.empty()) {
		return *this;
	}
	if (escape) {
		TReplacementMap replacement = boost::assign::map_list_of("%", "\\%") ("_", "\\_") ("\\", "\\\\");
		keyword = "%" + CStringUtils::strtr(keyword, replacement) + "%";
	}
	stringstream cond;
	stringstream paramName;
	paramName << PARAM_PREFIX << paramCount;
	cond << column << " " << like << " (:" << paramName.str() + ")";
	++paramCount;
	params[":" + paramName.str()] = CDbCommandParameter(paramName.str(), SQL_STRING, keyword.c_str());
	return addCondition(cond.str(), op);
}

CDbCriteria & CDbCriteria::addInCondition(const string & column, const vector<string> & values, const string op)
{
	stringstream cond;
	size_t n  = values.size();
	if (n < 1) {
		cond << "0=1"; // 0=1 is used because in MSSQL value alone can't be used in WHERE
	} else if(n == 1) {
		string value = values.front();
		if (value.empty()) {
			cond << column << " IS NULL OR " << column << " = ''";
		} else {
			stringstream paramName;
			paramName << PARAM_PREFIX << paramCount;
			cond << column << "=" << ":" << paramName.str();
			++paramCount;
			params[":" + paramName.str()] = CDbCommandParameter(paramName.str(), SQL_STRING, value.c_str());
		}
	} else {
		vector<string> params;
		for (vector<string>::const_iterator iter = values.begin(); iter != values.end(); ++iter) {
			stringstream paramName;
			paramName << PARAM_PREFIX << paramCount;
			params.push_back(paramName.str());
			++paramCount;
			this->params[":" + paramName.str()] = CDbCommandParameter(paramName.str(), SQL_STRING, iter->c_str());
		}
		cond << column << " IN (" << CStringUtils::implode(", ", params) << ")";
	}
	return addCondition(cond.str(), op);
}

CDbCriteria & CDbCriteria::addNotInCondition(const string & column, const vector<string> & values, const string op)
{
	stringstream cond;
	if (values.size() == 1) {
		string value = values.front();
		if (value.empty()) {
			cond << column << " IS NOT NULL";
		} else {
			stringstream paramName;
			paramName << PARAM_PREFIX << paramCount;
			cond << column << "=" << ":" << paramName.str();
			++paramCount;
			params[":" + paramName.str()] = CDbCommandParameter(paramName.str(), SQL_STRING, value.c_str());
		}
	} else {
		vector<string> params;
		for (vector<string>::const_iterator iter = values.begin(); iter != values.end(); ++iter) {
			stringstream paramName;
			paramName << PARAM_PREFIX << paramCount;
			params.push_back(paramName.str());
			++paramCount;
			this->params[":" + paramName.str()] = CDbCommandParameter(paramName.str(), SQL_STRING, iter->c_str());
		}
		cond << column << " NOT IN (" << CStringUtils::implode(", ", params) << ")";
	}
	return addCondition(cond.str(), op);
}


CDbCriteria & CDbCriteria::compare(
	const string & column, CDbCommandParameter value, const string & comparison,
	bool partialMatch, const string & op, bool escape
)
{
	if (partialMatch && value.type == SQL_STRING) {
		if (comparison.empty()) {
			string keyword = *((string*)value.value);
			return addSearchCondition(column, keyword, escape, op);
		}
		if (comparison == "<>") {
			string keyword = *((string*)value.value);
			return addSearchCondition(column, keyword, escape, op, "NOT LIKE");
		}
	}

	stringstream paramName;
	paramName << PARAM_PREFIX << paramCount;
	addCondition(column + comparison + paramName.str(), op);
	++paramCount;
	value.name = paramName.str();
	params[":" + paramName.str()] = value;
	return *this;
}

CDbCriteria & CDbCriteria::addBetweenCondition(const string & column, const long & valueStart, const long & valueEnd, const string & op)
{
	stringstream paramStart, paramEnd, cond;
	paramStart << PARAM_PREFIX << paramCount;
	paramEnd << PARAM_PREFIX << paramCount + 1;
	paramCount += 2;
	params[":" + paramStart.str()] = CDbCommandParameter(paramStart.str(), SQL_INT, &valueStart);
	params[":" + paramEnd.str()] = CDbCommandParameter(paramEnd.str(), SQL_INT, &valueEnd);
	cond << column << " BETWEEN :" << paramStart.str() << " AND :" << paramEnd.str();

	return addCondition(cond.str(), op);
}
