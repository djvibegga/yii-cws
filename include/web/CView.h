/*
 * CView.h
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#ifndef CVIEW_H_
#define CVIEW_H_

#include <boost/filesystem.hpp>
#include "web/CController.h"

typedef map<string, boost::filesystem::path> TViewPathMap;

class CView: public CBaseController, public IOutputBuffer, public IView
{
private:
	static TViewPathMap _viewPaths;
	CBaseController * _owner;
	IOutputBuffer * _outputBuffer;

public:
	CView(CBaseController * owner = 0);
	virtual ~CView();
	CBaseController * getOwner() const;
	const CController * getController() const;
	void setOutputBuffer(IOutputBuffer * buffer);
	IOutputBuffer * getOutputBuffer() const;

	virtual boost::filesystem::path getLocalViewPath() const;
	virtual boost::filesystem::path getViewPath(bool checkTheme = false) const;
	virtual string getViewFile(const string & viewName) const throw (CException);
	virtual string render(const string & view, cpptempl::data_map & data, bool ret = false) throw (CException);
	virtual string getContent() const;
	virtual void echo(const string & content);
	virtual IOutputBuffer & operator<< (const string &right);
};

#endif /* CVIEW_H_ */
