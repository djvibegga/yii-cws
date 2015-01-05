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
	const CBaseController * _owner;
	IOutputBuffer * _outputBuffer;
	TViewPtr _layout;

public:
	CView(const CBaseController * owner = 0);
	virtual ~CView();
	const CBaseController * getOwner() const;
	const CController * getController() const;
	void setOutputBuffer(IOutputBuffer * buffer);
	IOutputBuffer * getOutputBuffer() const;
	virtual void init();

	virtual boost::filesystem::path getLocalViewPath() const;
	virtual boost::filesystem::path getViewPath(bool checkTheme = false) const;
	virtual string getViewFile(const string & viewName) const throw (CException);
	virtual string render(const string & view, CDT & data, bool ret = false) throw (CException);
	virtual string getContent() const;
	virtual void echo(const wstring & content);
	virtual void echo(const string & content);
	virtual IOutputBuffer & operator<< (const wstring &right);
	virtual IOutputBuffer & operator<< (const string &right);
	void setLayout(const string & layout);
	void setLayout(TViewPtr layout);
	TViewPtr getLayout() const;

protected:
	virtual TViewPtr resolveLayout() const;
};

#endif /* CVIEW_H_ */
