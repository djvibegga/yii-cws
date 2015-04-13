/*
 * CAssetManager.h
 *
 *  Created on: Feb 12, 2014
 *      Author: djvibegga
 */

#ifndef CASSETMANAGER_H_
#define CASSETMANAGER_H_

#include "base/CApplicationComponent.h"
#include "utils/CFile.h"

typedef map<string, string> TAssetsPathMap;

class CAssetManager: public CApplicationComponent
{
private:
	boost::filesystem::path _basePath;
	string _baseUrl;
	TAssetsPathMap _published;

public:
	static const string DEFAULT_BASEPATH;
	bool linkAssets;
	TExcludeList excludeFiles;
	unsigned int newFileMode;
	unsigned int newDirMode;
	bool forceCopy;

	CAssetManager(CModule * module);
	CAssetManager(const string &id, CModule * module);
	virtual string getClassName() const;
	virtual void init();

	boost::filesystem::path getBasePath() const;
	void setBasePath(const boost::filesystem::path & path) throw (CException);
	virtual boost::filesystem::path resolveBasePath() const;
	string getBaseUrl() const;
	void setBaseUrl(const string & url);
	virtual string resolveBaseUrl() const;
	string publish(const boost::filesystem::path & path, bool hashByName = false, int level = -1) throw (CException);
	virtual string publish(const boost::filesystem::path & path, bool hashByName, int level, bool forceCopy) throw (CException);
	virtual boost::filesystem::path getPublishedPath(const boost::filesystem::path & path, bool hashByName = false) const;
	virtual string getPublishedUrl(const boost::filesystem::path & path, bool hashByName = false) const;

protected:
	virtual void applyConfig(const xml_node & config);
	virtual string hash(const boost::filesystem::path & path) const;
	string generatePath(const boost::filesystem::path & file, bool hashByName = false) const;
};

#endif /* CASSETMANAGER_H_ */
