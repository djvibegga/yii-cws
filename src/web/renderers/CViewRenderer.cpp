/*
 * CViewRenderer.cpp
 *
 *  Created on: 26 ����. 2013 �.
 *      Author: djvibegga
 */

#include "web/renderers/CViewRenderer.h"
#include "base/CException.h"
#include "base/Jvibetto.h"
#include <boost/filesystem.hpp>
#include <sys/stat.h>
#include <ctpp2/CTPP2VMFileLoader.hpp>
#include <ctpp2/CTPP2StreamOutputCollector.hpp>
#include <ctpp2/CTPP2VMMemoryCore.hpp>

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

_string CViewRenderer::renderFile(const IRenderingContext * context, const string & sourceFile, CDT & data, bool ret) throw (CException)
{
    boost::filesystem::path sourcePath(sourceFile);
    if (!boost::filesystem::exists(sourcePath)) {
        throw CException("View file \"" + sourceFile + "\" does not exist.");
    }
    string viewFile = getViewFile(sourceFile);
    boost::filesystem::path viewPath(viewFile);
    if (!boost::filesystem::exists(viewPath) || boost::filesystem::last_write_time(sourcePath) > boost::filesystem::last_write_time(viewPath)) {
        if (generateViewFile(sourceFile, viewFile)) {
            chmod(viewFile.c_str(), filePermission);
        } else {
            throw CException("Can't generate view file \"" + viewFile + "\" from source file \"" + sourceFile + "\".");
        }
    }

    if (context != 0) {
    	return context->renderInternal(viewFile, data, ret);
    }

	stringstream os;
	StreamOutputCollector outputCollector(os);
    TDynamicTemplateCacheMap::const_iterator found = _templateCache.find(viewFile);

	VMLoader * oLoader = 0;
	if (found == _templateCache.end()) {
		oLoader = new VMFileLoader(viewFile.c_str());
		_templateCache[viewFile.c_str()] = boost::shared_ptr<VMLoader>(oLoader);
	} else {
		oLoader = found->second.get();
	}

	PROFILE_BEGIN("CViewRenderer::rendering template bytecode of \"" + viewFile + "\"")
	UINT_32 iIP = 0;
	VM * vm = Jvibetto::app()->getTemplateEngine()->getVM();
	const VMMemoryCore * pVMMemoryCore = oLoader->GetCore();
	vm->Init(pVMMemoryCore, &outputCollector, 0);
	vm->Run(pVMMemoryCore, &outputCollector, iIP, data, 0);
	PROFILE_END()

	if (ret) {
		return utf8_to_(os.str());
	} else {
		Jvibetto::app()->getOutputStack().top()->echo(os.str());
		return _("");
	}
}

string CViewRenderer::getViewFile(const string & file)
{
	boost::filesystem::path sourcePath(file);
	return sourcePath.parent_path().string() + "/" + boost::filesystem::basename(sourcePath) + ".ct2";
}
