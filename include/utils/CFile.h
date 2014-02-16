/*
 * CFileHelper.h
 *
 *  Created on: Feb 16, 2014
 *      Author: djvibegga
 */

#ifndef CFILE_H_
#define CFILE_H_

#include "base/CComponent.h"

typedef list<string> TFileList;
typedef list<string> TExtensionList;
typedef list<string> TExcludeList;

class CFile: public CComponent
{
public:
	static TFileList find(
		const boost::filesystem::path & where,
		const TExtensionList & fileTypes,
		const TExcludeList & exclude,
		int level = -1,
		bool absolutePaths = true);
	static void copyDirectory(
		const boost::filesystem::path & src,
		const boost::filesystem::path & dst,
		const TExtensionList & fileTypes,
		const TExcludeList & exclude,
		unsigned int newDirMode,
		unsigned int newFileMode,
		int level = -1) throw (boost::filesystem3::filesystem_error);

protected:
	static TFileList findRecursive(
		const boost::filesystem::path & where,
		const string & base,
		const TExtensionList & fileTypes,
		const TExcludeList & exclude,
		int level,
		bool absolutePaths);
	static bool validatePath(
		const boost::filesystem::path & file,
		bool isFile,
		const TExtensionList & fileTypes,
		const TExcludeList & exclude);
	static void copyDirectoryRecursive(
		const boost::filesystem::path & src,
		const boost::filesystem::path & dst,
		const string & base,
		const TExtensionList & fileTypes,
		const TExcludeList & exclude,
		unsigned int newDirMode,
		unsigned int newFileMode,
		int level = -1) throw (boost::filesystem3::filesystem_error);
};

#endif /* CFILE_H_ */
