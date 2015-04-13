/*
 * CStatePersister.cpp
 *
 *  Created on: Mar 19, 2015
 *      Author: djvibegga
 */

#include "base/CStatePersister.h"
#include "utils/CFile.h"
#include "base/Jvibetto.h"
#include "utils/CArchiver.h"

CStatePersister::CStatePersister()
: CApplicationComponent(0),
 cacheID("cache")
{
}

string CStatePersister::getClassName() const
{
	return "CStatePersister";
}

void CStatePersister::init()
{
	if (stateFile.empty()) {
		stateFile = Jvibetto::app()->getRuntimePath().string() + "/state.bin";
	}
	boost::filesystem::path dir = boost::filesystem::path(stateFile).parent_path();
	if (!boost::filesystem::is_directory(dir) || access(dir.string().c_str(), W_OK) == -1) {
		throw CException("Unable to create application state file \"" + stateFile + "{file}\". Make sure the directory containing the file exists and is writable by the Web server process.");
	}
}

CStatesMap CStatePersister::load()
{
	/*if($this->cacheID!==false && ($cache=Yii::app()->getComponent($this->cacheID))!==null)
	{
		$cacheKey='Yii.CStatePersister.'.$stateFile;
		if(($value=$cache->get($cacheKey))!==false)
			return unserialize($value);
		elseif(($content=@file_get_contents($stateFile))!==false)
		{
			$cache->set($cacheKey,$content,0,new CFileCacheDependency($stateFile));
			return unserialize($content);
		}
		else
			return null;
	}*/
	string content;
	try {
		content = CFile::getContents(stateFile);
	} catch (CException & e) {
	}
	CStatesMap states;
	if (!content.empty()) {
		try {
			CArchiver<CStatesMap>::load(content, states);
		} catch (boost::archive::archive_exception & e) {
			throw CException("Unable to restore states map from file: \"" + stateFile + "\".");
		}
	}
	return states;
}

void CStatePersister::save(CStatesMap & states)
{
	string content = CArchiver<CStatesMap>::save(states);
	CFile::putContents(stateFile, content);
}
