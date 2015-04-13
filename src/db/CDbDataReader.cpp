/*
 * CDbDataReader.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: djvibegga
 */

#include "db/CDbDataReader.h"
#include <sqlapi/myAPI.h>

CDbDataReader::CDbDataReader(SACommand * command)
: _closed(false),
  _index(-1)
{
	_saCommand = command;
}

string CDbDataReader::getClassName() const
{
	return "CDbDataReader";
}

CDbDataReader::~CDbDataReader()
{
}

bool CDbDataReader::nextResult()
{
	if (_closed) {
		return false;
	}
	if (_saCommand->FetchNext()) {
		++_index;
	} else {
		_closed = true;
		_saCommand->Close();
		return false;
	}
	return true;
}

SAField & CDbDataReader::readColumn(int columnNum)
{
	return _saCommand->Field(columnNum);
}

SAField & CDbDataReader::readColumn(const string & columnName)
{
	return _saCommand->Field(columnName.c_str());
}

TDbRow CDbDataReader::readRow()
{
	TDbRow row;
	int fieldsCount = _saCommand->FieldCount();
	for (int i = 1; i <= fieldsCount; ++i) {
		row[string(_saCommand->Field(i).Name().GetMultiByteChars())] = &_saCommand->Field(i);
	}
	return row;
}

int CDbDataReader::getRowCount() const
{
	return _saCommand->RowsAffected();
}
