/*
 * CTemplateView.h
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#ifndef CTEMPLATEVIEW_H_
#define CTEMPLATEVIEW_H_

#include "web/CView.h"

typedef map<string, cpptempl::token_vector> TTemplateCacheMap;

class CTemplateView: public CView
{
private:
	string _viewFile;
	cpptempl::data_map & _data;
	static TTemplateCacheMap _templateCache;

public:
	CTemplateView(const string & viewFile, cpptempl::data_map & data, const CBaseController * owner = 0);
	virtual ~CTemplateView();
	cpptempl::data_map & getData() const;
	virtual void init();
	virtual void run() throw (CException);
};

#endif /* CTEMPLATEVIEW_H_ */
