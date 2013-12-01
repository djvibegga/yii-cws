/*
 * CViewRenderer.cpp
 *
 *  Created on: 26 םמ‗ב. 2013 ד.
 *      Author: djvibegga
 */

#include <web/renderers/CViewRenderer.h>
#include <boost/filesystem.hpp>
#include <base/CException.h>
#include <sys/stat.h>

CViewRenderer::CViewRenderer(CModule * module)
: CApplicationComponent("viewRenderer", module),
  useRuntimePath(true),
  filePermission(0755)
{

}

CViewRenderer::CViewRenderer(const string &id, CModule * module)
: CApplicationComponent(id, module),
  useRuntimePath(true),
  filePermission(0755)
{

}

CViewRenderer::~CViewRenderer()
{
}

string CViewRenderer::renderFile(const IRenderingContext * context, const string & sourceFile, cpptempl::data_map * data, bool ret) throw (CException)
{
    boost::filesystem::path sourcePath(sourceFile);
    if (!boost::filesystem::exists(sourcePath)) {
        throw CException("View file \"" + sourceFile + "\" does not exist.");
    }
    string viewFile = getViewFile(sourceFile);
    boost::filesystem::path viewPath(viewFile);
    if (boost::filesystem::last_write_time(sourcePath) > boost::filesystem::last_write_time(viewPath)) {
        if (generateViewFile(sourceFile, viewFile)) {
            chmod(viewFile.c_str(), filePermission);
        } else {
            throw CException("Can't generate view file \"" + viewFile + "\" from source file \"" + sourceFile + "\".");
        }
    }
    return context->renderInternal(viewFile, data, ret);
}

string CViewRenderer::getViewFile(const string & file)
{
    //TODO: implement dynamic view compile
    return file;
}
