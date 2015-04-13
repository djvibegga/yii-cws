/*
 * CAssetManager.cpp
 *
 *  Created on: Feb 12, 2014
 *      Author: djvibegga
 */

#include "web/CAssetManager.h"
#include "web/CWebApplication.h"
#include "base/CStringUtils.h"
#include "base/Jvibetto.h"
#include "utils/CHash.h"
#include "sys/stat.h"

const string CAssetManager::DEFAULT_BASEPATH = "public/assets";

CAssetManager::CAssetManager(CModule * module)
: CApplicationComponent("assetManager", module),
  linkAssets(false),
  newFileMode(0666),
  newDirMode(0777),
  forceCopy(false)
{
	excludeFiles.push_back(".svn");
	excludeFiles.push_back(".gitignore");
}

CAssetManager::CAssetManager(const string &id, CModule * module)
: CApplicationComponent(id, module),
  linkAssets(false),
  newFileMode(0666),
  newDirMode(0777),
  forceCopy(false)
{
	excludeFiles.push_back(".svn");
	excludeFiles.push_back(".gitignore");
}

string CAssetManager::getClassName() const
{
	return "CAssetManager";
}

void CAssetManager::init()
{
	CApplicationComponent::init();
	_basePath = resolveBasePath();
	_baseUrl = resolveBaseUrl();
}

boost::filesystem::path CAssetManager::getBasePath() const
{
	return _basePath;
}

void CAssetManager::setBasePath(const boost::filesystem::path & path) throw (CException)
{
	if (!path.empty() && boost::filesystem::is_directory(path) && access(path.string().c_str(), W_OK)) {
		_basePath = path;
	} else {
		throw CException(
			"CAssetManager.basePath \"" + path.string() + "\" is invalid. Please make sure the directory exists and is writable by the Web server process."
		);
	}
}

boost::filesystem::path CAssetManager::resolveBasePath() const
{
	return boost::filesystem::path(Jvibetto::app()->getBasePath().string() + "/" + DEFAULT_BASEPATH);
}


string CAssetManager::getBaseUrl() const
{
	return _baseUrl;
}

void CAssetManager::setBaseUrl(const string & url)
{
	_baseUrl = CStringUtils::rtrim(url, "/");
}

string CAssetManager::resolveBaseUrl() const
{
	return "/" + DEFAULT_BASEPATH;
}

string CAssetManager::publish(const boost::filesystem::path & path, bool hashByName, int level) throw (CException)
{
	return publish(path, hashByName, level, forceCopy);
}

string CAssetManager::publish(const boost::filesystem::path & path, bool hashByName, int level, bool forceCopy) throw (CException)
{
	if (forceCopy && linkAssets) {
		throw CException("The \"forceCopy\" and \"linkAssets\" cannot be both true.", 0, __FILE__, __LINE__);
	}
	boost::filesystem::path src = boost::filesystem::canonical(path);
	TAssetsPathMap::const_iterator found = _published.find(src.string());
	if (found == _published.end()) {
		if (!boost::filesystem::exists(src)) {
			throw CException("The asset \"" + path.string() + "\" to be published does not exist.");
		} else {
			string dir = generatePath(src, hashByName);
			boost::filesystem::path dstDir(getBasePath().string() + "/" + dir);
			bool isDirDest = boost::filesystem::is_directory(dstDir);
			if (boost::filesystem::is_directory(src)) {
				if (linkAssets && !isDirDest) {
					boost::filesystem::create_symlink(src, dstDir);
				} else if (!isDirDest || forceCopy) {
					CFile::copyDirectory(
						src,
						dstDir,
						TExcludeList(),
						excludeFiles,
						newDirMode,
						newFileMode,
						level
					);
				}
				return _published[src.string()] = getBaseUrl() + "/" + dir;
			} else {
				string fileName = src.filename().string();
				boost::filesystem::path dstFile(dstDir.string() + "/" + fileName);
				if (!isDirDest) {
					boost::filesystem::create_directories(dstDir);
					chmod(dstDir.string().c_str(), newDirMode);
				}
				bool isFileExists = boost::filesystem::exists(dstFile);
				if (linkAssets && !isFileExists) {
					boost::filesystem::create_symlink(src, dstFile);
				} else if (!isFileExists || boost::filesystem::last_write_time(dstFile) < boost::filesystem::last_write_time(src)) {
					boost::filesystem::copy_file(src, dstFile);
					chmod(dstFile.string().c_str(), newFileMode);
				}
				return _published[src.string()] = getBaseUrl() + "/" + dir + "/" + fileName;
			}
		}
	} else {
		return found->second;
	}
}

boost::filesystem::path CAssetManager::getPublishedPath(const boost::filesystem::path & path, bool hashByName) const
{
	boost::filesystem::path canonical = boost::filesystem::canonical(path);
	if (!boost::filesystem::exists(canonical)) {
		return boost::filesystem::path("");
	} else {
		boost::filesystem::path basePath = boost::filesystem::path(
			getBasePath().string() + "/" + generatePath(canonical, hashByName)
		);
		return boost::filesystem::is_directory(canonical)
			  ? basePath
			  : boost::filesystem::path(basePath.string() + "/" + canonical.filename().string());
	}
}

string CAssetManager::getPublishedUrl(const boost::filesystem::path & path, bool hashByName) const
{
	TAssetsPathMap::const_iterator found = _published.find(path.string());
	boost::filesystem::path canonical = boost::filesystem::canonical(path);
	if (found != _published.end()) {
		return found->second;
	} else if (!boost::filesystem::exists(canonical)) {
		return "";
	} else {
		string baseUrl = getBaseUrl() + "/" + generatePath(canonical, hashByName);
		return boost::filesystem::is_directory(canonical)
			  ? baseUrl
			  : baseUrl + "/" + canonical.filename().string();
	}
}

string CAssetManager::hash(const boost::filesystem::path & path) const
{
	stringstream ss;
	ss << CHash::crc32(path.string());
	return ss.str();
}

string CAssetManager::generatePath(const boost::filesystem::path & file, bool hashByName) const
{
	string forHashing;
	string dirname = boost::filesystem::is_directory(file)
				   ? file.string()
				   : file.parent_path().string();
	if (hashByName) {
		forHashing = dirname;
	} else {
		stringstream ss;
		ss << boost::filesystem::last_write_time(file);
		forHashing = dirname + ss.str();
	}
	return hash(forHashing);
}

void CAssetManager::applyConfig(const xml_node & config)
{
	PARSE_XML_CONF_BOOL_PROPERTY(config, linkAssets, "linkAssets");
}
