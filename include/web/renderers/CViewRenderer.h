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

class CViewRenderer: public CApplicationComponent, public IViewRenderer
{
public:
    bool useRuntimePath;
    unsigned int filePermission;
    CViewRenderer(CModule * module);
    CViewRenderer(const string &id, CModule * module);
    virtual ~CViewRenderer();
    _string renderFile(const IRenderingContext * context, const string & sourceFile, const cpptempl::data_map & data, bool ret) throw (CException);

protected:
    virtual bool generateViewFile(const string & sourceFile, const string & viewFile) = 0;
    string getViewFile(const string & file);
};

#endif /* CVIEWRENDERER_H_ */
