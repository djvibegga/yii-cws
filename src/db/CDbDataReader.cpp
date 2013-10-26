/*
 * CDbDataReader.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: djvibegga
 */

#include "db/CDbDataReader.h"

CDbDataReader::CDbDataReader(const CDbCommand & command)
: _closed(false),
  _index(-1)
{
	_statement = command.getStatement();
}

CDbDataReader::~CDbDataReader()
{
}

/*
public function bindColumn($column, &$value, $dataType=null)
{
	if($dataType===null)
		$this->_statement->bindColumn($column,$value);
	else
		$this->_statement->bindColumn($column,$value,$dataType);
}

public function read()
{
	return $this->_statement->fetch();
}

public function readColumn($columnIndex)
{
	return $this->_statement->fetchColumn($columnIndex);
}

public function readObject($className,$fields)
{
	return $this->_statement->fetchObject($className,$fields);
}

public function readAll()
{
	return $this->_statement->fetchAll();
}

public function nextResult()
{
	if(($result=$this->_statement->nextRowset())!==false)
		$this->_index=-1;
	return $result;
}


public function close()
{
	$this->_statement->closeCursor();
	$this->_closed=true;
}


public function getIsClosed()
{
	return $this->_closed;
}

public function getRowCount()
{
	return $this->_statement->rowCount();
}

public function count()
{
	return $this->getRowCount();
}


public function getColumnCount()
{
	return $this->_statement->columnCount();
}

public function rewind()
{
	if($this->_index<0)
	{
		$this->_row=$this->_statement->fetch();
		$this->_index=0;
	}
	else
		throw new CDbException(Yii::t('yii','CDbDataReader cannot rewind. It is a forward-only reader.'));
}

public function key()
{
	return $this->_index;
}


public function current()
{
	return $this->_row;
}


public function next()
{
	$this->_row=$this->_statement->fetch();
	$this->_index++;
}

public function valid()
{
	return $this->_row!==false;
}
*/
