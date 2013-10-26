/*
 * CDbCommand.cpp
 *
 *  Created on: Oct 25, 2013
 *      Author: djvibegga
 */

#include "db/CDbCommand.h"
#include "base/CStringUtils.h"
#include "db/CDbDataReader.h"
#include "base/Jvibetto.h"
#include <mysql/mysql.h>

CDbCommandParameter::CDbCommandParameter(ECommandParameter type, TDbParameter value)
{
	this->type = type;
	this->value = value;
}

CDbCommandParameter::CDbCommandParameter()
{
}

string CDbCommandParameter::dump() const
{
	switch (type) {
	case PARAM_STRING:
		return string((const char*)value);
	}
	throw CException("Invalid data type. Can\'t dump db command parameter.");
}

MYSQL_BIND * CDbCommandParameter::constructBind() const
{
	MYSQL_BIND * param = new MYSQL_BIND;
	memset(param, 0, sizeof(param));
	//memset (result, 0, sizeof (result));
	switch (type) {
	case PARAM_STRING:
		param->buffer_type = MYSQL_TYPE_VARCHAR;
	case PARAM_INT:
		param->buffer_type = MYSQL_TYPE_LONG;
	case PARAM_FLOAT:
		param->buffer_type = MYSQL_TYPE_FLOAT;
	case PARAM_DOUBLE:
		param->buffer_type = MYSQL_TYPE_DOUBLE;
	}
	param->buffer      = value;
	param->is_unsigned = 0;
	param->is_null     = 0;
	param->length      = 0;
	return param;
}

CDbCommand::CDbCommand(const CDbConnection * const connection, const string & query)
: _connection(connection),
  _text(query),
  _statement(0)
{
}

CDbCommand::CDbCommand(const CDbConnection * const connection)
: _connection(connection),
  _text(""),
  _statement(0)
{
}

CDbCommand::~CDbCommand()
{
	for (vector<MYSQL_BIND*>::iterator iter = _binds.begin(); iter != _binds.end(); ++iter) {
		delete *iter;
	}
}

void CDbCommand::setText(const string & query)
{
	_text = query;
}

string CDbCommand::getText() const
{
	return _text;
}

MYSQL_STMT * CDbCommand::getStatement() const
{
	return _statement;
}

const CDbConnection * CDbCommand::getConnection() const
{
	return _connection;
}

CDbCommand & CDbCommand::bindParam(const string & name, const string & value)
{
	_params[name] = CDbCommandParameter(PARAM_STRING, (TDbParameter)value.c_str());
	return *this;
}

CDbCommand & CDbCommand::bindParam(const string & name, const int value)
{
	_params[name] = CDbCommandParameter(PARAM_INT, (TDbParameter)&value);
	return *this;
}

CDbCommand & CDbCommand::bindParam(const string & name, const float & value)
{
	_params[name] = CDbCommandParameter(PARAM_FLOAT, (TDbParameter)&value);
	return *this;
}

CDbCommand & CDbCommand::bindParam(const string & name, const double & value)
{
	_params[name] = CDbCommandParameter(PARAM_DOUBLE, (TDbParameter)&value);
	return *this;
}

CDbCommand & CDbCommand::mergeParametersWith(const TCommandParameterMap & other)
{
	for (TCommandParameterMap::const_iterator iter = other.begin(); iter != other.end(); ++iter) {
		_params[iter->first] = iter->second;
	}
	return *this;
}

int CDbCommand::execute() throw (CDbException)
{
	return execute(TCommandParameterMap());
}

int CDbCommand::execute(const TCommandParameterMap & params) throw (CDbException)
{
	mergeParametersWith(params);
	string parametersDump;
	if (_connection->enableParamLogging && !_params.empty()) {
		vector<string> parametersList;
		for (TCommandParameterMap::const_iterator iter = _params.begin(); iter != _params.end(); ++iter) {
			parametersList.push_back(iter->first + "=" + iter->second.dump());
		}
		parametersDump = "Bound with " + CStringUtils::implode(", ", parametersList);
	}
	Jvibetto::trace("Executing SQL: " + _text + parametersDump, "system.db.CDbCommand");
	try
	{
		MYSQL * connection = _connection->getConnection();
		_statement = mysql_stmt_init(connection);
		if (_statement == 0) {
			//throw CDbException()
		}

		if (mysql_stmt_prepare(_statement, _text.c_str(), _text.length())) {
			const char * error = mysql_stmt_error(_statement);
			throw CDbException(string(error), mysql_stmt_errno(_statement));
		}

		MYSQL_BIND * bind = 0;
		for (TCommandParameterMap::const_iterator iter = _params.begin(); iter != _params.end(); ++iter) {
			bind = iter->second.constructBind();
			mysql_stmt_bind_param(_statement, bind);
			_binds.push_back(bind);
		}

		mysql_stmt_execute(_statement);

		return mysql_stmt_affected_rows(_statement);

	} catch(const CException & e) {
		/*$errorInfo=$e instanceof PDOException ? $e->errorInfo : null;
		$message=$e->getMessage();
		Yii::log(Yii::t('yii','CDbCommand::execute() failed: {error}. The SQL statement executed was: {sql}.',
			array('{error}'=>$message, '{sql}'=>$this->getText().$par)),CLogger::LEVEL_ERROR,'system.db.CDbCommand');

		if(YII_DEBUG)
			$message.='. The SQL statement executed was: '.$this->getText().$par;

		throw new CDbException(Yii::t('yii','CDbCommand failed to execute the SQL statement: {error}',
			array('{error}'=>$message)),(int)$e->getCode(),$errorInfo);*/
	}
	return 0;
}


CDbDataReader CDbCommand::_queryInternal(const string & method, const TCommandParameterMap & params)
{
	/*$params=array_merge($this->params,$params);

	if($this->_connection->enableParamLogging && ($pars=array_merge($this->_paramLog,$params))!==array())
	{
		$p=array();
		foreach($pars as $name=>$value)
			$p[$name]=$name.'='.var_export($value,true);
		$par='. Bound with '.implode(', ',$p);
	}
	else
		$par='';

	Yii::trace('Querying SQL: '.$this->getText().$par,'system.db.CDbCommand');

	if($this->_connection->queryCachingCount>0 && $method!==''
			&& $this->_connection->queryCachingDuration>0
			&& $this->_connection->queryCacheID!==false
			&& ($cache=Yii::app()->getComponent($this->_connection->queryCacheID))!==null)
	{
		$this->_connection->queryCachingCount--;
		$cacheKey='yii:dbquery'.$this->_connection->connectionString.':'.$this->_connection->username;
		$cacheKey.=':'.$this->getText().':'.serialize(array_merge($this->_paramLog,$params));
		if(($result=$cache->get($cacheKey))!==false)
		{
			Yii::trace('Query result found in cache','system.db.CDbCommand');
			return $result[0];
		}
	}

	try
	{
		if($this->_connection->enableProfiling)
			Yii::beginProfile('system.db.CDbCommand.query('.$this->getText().$par.')','system.db.CDbCommand.query');

		$this->prepare();
		if($params===array())
			$this->_statement->execute();
		else
			$this->_statement->execute($params);

		if($method==='')
			$result=new CDbDataReader($this);
		else
		{
			$mode=(array)$mode;
			call_user_func_array(array($this->_statement, 'setFetchMode'), $mode);
			$result=$this->_statement->$method();
			$this->_statement->closeCursor();
		}

		if($this->_connection->enableProfiling)
			Yii::endProfile('system.db.CDbCommand.query('.$this->getText().$par.')','system.db.CDbCommand.query');

		if(isset($cache,$cacheKey))
			$cache->set($cacheKey, array($result), $this->_connection->queryCachingDuration, $this->_connection->queryCachingDependency);

		return $result;
	}
	catch(Exception $e)
	{
		if($this->_connection->enableProfiling)
			Yii::endProfile('system.db.CDbCommand.query('.$this->getText().$par.')','system.db.CDbCommand.query');

		$errorInfo=$e instanceof PDOException ? $e->errorInfo : null;
		$message=$e->getMessage();
		Yii::log(Yii::t('yii','CDbCommand::{method}() failed: {error}. The SQL statement executed was: {sql}.',
			array('{method}'=>$method, '{error}'=>$message, '{sql}'=>$this->getText().$par)),CLogger::LEVEL_ERROR,'system.db.CDbCommand');

		if(YII_DEBUG)
			$message.='. The SQL statement executed was: '.$this->getText().$par;

		throw new CDbException(Yii::t('yii','CDbCommand failed to execute the SQL statement: {error}',
			array('{error}'=>$message)),(int)$e->getCode(),$errorInfo);
	}*/
	return CDbDataReader(*this);
}
