/*
 * CDbDataReader.h
 *
 *  Created on: Oct 26, 2013
 *      Author: djvibegga
 */

#ifndef CDBDATAREADER_H_
#define CDBDATAREADER_H_

#include "base/CComponent.h"
#include "db/CDbCommand.h"
#include <map>
#include <vector>

using namespace std;

class CDbDataReader: public CComponent
{
private:
	bool _closed;
	int _index;
	SACommand * _saCommand;

public:
	CDbDataReader(SACommand * command);
	virtual string getClassName() const;
	~CDbDataReader();
	bool nextResult();
	SAField & readColumn(int columnNum);
	SAField & readColumn(const string & columnName);
	TDbRow readRow();
	int getRowCount() const;
};

#endif /* CDBDATAREADER_H_ */
