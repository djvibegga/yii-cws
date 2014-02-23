/*
 * CFile.cpp
 *
 *  Created on: Feb 16, 2014
 *      Author: djvibegga
 */

#include "utils/CFile.h"
#include <sys/stat.h>
#include <boost/regex.hpp>
#include <fstream>
#include <stdio.h>

TFileList CFile::find(
	const boost::filesystem::path & where,
	const TExtensionList & fileTypes,
	const TExcludeList & exclude,
	int level,
	bool absolutePaths)
{
	TFileList list = findRecursive(where, where.string(), fileTypes, exclude, level, absolutePaths);
//	sort($list);
	return list;
}

TFileList CFile::findRecursive(
	const boost::filesystem::path & where,
	const string & base,
	const TExtensionList & fileTypes,
	const TExcludeList & exclude,
	int level,
	bool absolutePaths)
{
	TFileList list;
	try {
		boost::filesystem::directory_iterator iter(where.string());
		boost::filesystem::path current;
		string currentStr;
		::string::size_type pos;
		bool isFile;
		while (iter != boost::filesystem::directory_iterator()) {
			current = iter->path();
			isFile = !boost::filesystem::is_directory(current);
			if (validatePath(current, isFile, fileTypes, exclude)) {
				currentStr = current.string();
				if (isFile) {
					if (absolutePaths) {
						list.push_back(currentStr);
					} else {
						pos = currentStr.find(base);
						if (pos == ::string::npos) {
							list.push_back(currentStr);
						} else {
							list.push_back(currentStr.substr(pos + base.length() + 1));
						}
					}
				} else if (level) {
					TFileList sublist = findRecursive(current, base, fileTypes, exclude, level - 1, absolutePaths);
					list.merge(sublist);
				}
			}
			++iter;
		}
		return list;
	} catch (boost::filesystem::filesystem_error & e) {
		return list;
	}
}


bool CFile::validatePath(
	const boost::filesystem::path & file,
	bool isFile,
	const TExtensionList & fileTypes,
	const TExcludeList & exclude)
{
	for (TExcludeList::const_iterator iter = exclude.begin(); iter != exclude.end(); ++iter) {
		if (boost::regex_match(file.string(), boost::regex(*iter))) {
			return false;
		}
	}
	if (!isFile || fileTypes.empty()) {
		return true;
	}
	string extension = file.extension().string();
	for (TExtensionList::const_iterator iter = fileTypes.begin(); iter != fileTypes.end(); ++iter) {
		if (*iter == extension) {
			return true;
		}
	}
	return false;
}

void CFile::copyDirectory(
	const boost::filesystem::path & src,
	const boost::filesystem::path & dst,
	const TExtensionList & fileTypes,
	const TExcludeList & exclude,
	unsigned int newDirMode,
	unsigned int newFileMode,
	int level) throw (boost::filesystem3::filesystem_error)
{
	if (!boost::filesystem::is_directory(dst)) {
		boost::filesystem::create_directories(dst);
		chmod(dst.string().c_str(), newDirMode);
	}
	copyDirectoryRecursive(src, dst, "", fileTypes, exclude, newDirMode, newFileMode, level);
}

void CFile::copyDirectoryRecursive(
	const boost::filesystem::path & src,
	const boost::filesystem::path & dst,
	const string & base,
	const TExtensionList & fileTypes,
	const TExcludeList & exclude,
	unsigned int newDirMode,
	unsigned int newFileMode,
	int level) throw (boost::filesystem3::filesystem_error)
{
	if (!boost::filesystem::is_directory(dst)) {
		boost::filesystem::create_directories(dst);
		chmod(dst.string().c_str(), newDirMode);
	}

	boost::filesystem::directory_iterator iter(src.string());
	boost::filesystem::path current;
	string target;
	bool isFile;
	while (iter != boost::filesystem::directory_iterator()) {
		current = iter->path();
		isFile = !boost::filesystem::is_directory(current);
		if (validatePath(current, isFile, fileTypes, exclude)) {
			target = dst.string() + "/" + current.filename().string();
			if (isFile) {
				try {
					boost::filesystem::copy_file(
						current,
						boost::filesystem::path(target)
					);
					if (newFileMode) {
						chmod(target.c_str(), newFileMode);
					}
				} catch (boost::filesystem3::filesystem_error & e) {

				}
			} else if (level) {
				copyDirectoryRecursive(
					current,
					boost::filesystem::path(target),
					base + "/" + current.filename().string(),
					fileTypes,
					exclude,
					newDirMode,
					newFileMode,
					level - 1);
			}
		}
		++iter;
	}
}

string CFile::dirName(const string & path)
{
    std::string::size_type pos = path.find_last_of("/");
	if (pos == ::string::npos) {
		return "";
	}
	return path.substr(0, pos);
}

string CFile::getContents(const string & path) throw (CException)
{
    std::ifstream in(path.c_str());
    if (!in.is_open()) {
    	throw CException("File \"" + path + "\" has not found.", 0, __FILE__, __LINE__);
    }
    in.seekg(0, std::ios::end);
    size_t size = in.tellg();
    std::string buffer(size, ' ');
    in.seekg(0);
    in.read(&buffer[0], size);
    return buffer;
}

void CFile::putContents(const string & path, const string & content) throw (CException)
{
	_IO_FILE * file = fopen(path.c_str(), "w");
	if (!file) {
		throw CException("Can\'t open file for writing: '" + path + "'.");
	}
	flockfile(file);
	fprintf(file, "%s", content.c_str());
	funlockfile(file);
	fclose(file);
}
