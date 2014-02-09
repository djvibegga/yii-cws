/*
 * CTemplateView.h
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#ifndef CTEMPLATEVIEW_H_
#define CTEMPLATEVIEW_H_

#include "web/CView.h"

typedef map<string, cpptempl::token_vector> TDynamicTemplateCacheMap;
typedef map<string, wstring> TStaticTemplateCacheMap;

class CTemplateView: public CView
{
private:
	string _viewFile;
	cpptempl::data_map _data;
	static TDynamicTemplateCacheMap _dynamicTemplateCache;
	static TStaticTemplateCacheMap _staticTemplateCache;

public:
	CTemplateView(const string & viewFile, const cpptempl::data_map & data, const CBaseController * owner = 0);
	CTemplateView(const string & viewFile, const CBaseController * owner = 0);
	virtual ~CTemplateView();
	void setData(const cpptempl::data_map & data);
	cpptempl::data_map & getData();
	virtual void init();
	virtual void run() throw (CException);
};

#endif /* CTEMPLATEVIEW_H_ */
