/*
 * CViewRenderer.h
 *
 *  Created on: 26 ����. 2013 �.
 *      Author: djvibegga
 */

#ifndef CVIEWRENDERER_H_
#define CVIEWRENDERER_H_

#include <base/CApplicationComponent.h>
#include <base/CException.h>
#include <ctpp2/CTPP2VMLoader.hpp>

typedef boost::shared_ptr<VMLoader> VMLoaderPtr;
typedef map<string, VMLoaderPtr> TDynamicTemplateCacheMap;

class CViewRenderer: public CApplicationComponent, public IViewRenderer
{
private:
	TDynamicTemplateCacheMap _templateCache;

public:
    bool useRuntimePath;
    unsigned int filePermission;
    CViewRenderer(CModule * module);
    CViewRenderer(const string &id, CModule * module);
    virtual ~CViewRenderer();
    string renderFile(const IRenderingContext * context, const string & sourceFile, CDT & data, bool ret) throw (CException);

protected:
    virtual bool generateViewFile(const string & sourceFile, const string & viewFile) = 0;
    string getViewFile(const string & file);
};

#endif /* CVIEWRENDERER_H_ */
