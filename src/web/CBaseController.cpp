#include <web/CBaseController.h>
#include <web/CHttpResponse.h>
#include <web/CWebApplication.h>
#include <base/Cws.h>
#include <base/CMemoryOutputBuffer.h>
#include <base/CStringUtils.h>
#include "base/CProfiler.h"
#include "web/CTemplateView.h"
#include <string>
#include "config.h"

string CBaseController::renderFile(const string & viewFile, CDT & data, bool ret)
{
    unsigned int widgetCount = _widgetStack.size();
    IViewRenderer * renderer = dynamic_cast<IViewRenderer*>(Cws::app()->getComponent("viewRenderer"));
    boost::filesystem::path viewPath(viewFile);
    string content = "";
    if (renderer != 0 && renderer->fileExtension == viewPath.extension().string()) {
        content = renderer->renderFile(this, viewFile, data, ret);
    } else {
        content = renderInternal(viewFile, data, ret);
    }
    if (_widgetStack.size() == widgetCount) {
        return content;
    } else {
        throw CException(
            "Controller contains improperly nested widget tags in its view \""
            + viewFile + "\". Some widget does not have an endWidget() call."
        );
    }
}

string CBaseController::renderInternal(const string & viewFile, CDT & data, bool ret) const
{
	CTemplateView view(viewFile, data, this);
	return renderInternal(view, ret);
}

string CBaseController::renderInternal(IView & viewInstance, bool ret) const
{
	CApplication * app = Cws::app();
	if (ret) {
		app->getOutputStack().push(new CMemoryOutputBuffer());
	}
	IOutputBuffer * outputBuffer = app->getOutputStack().top();
	viewInstance.setOutputBuffer(outputBuffer);
	viewInstance.init();
	viewInstance.run();
	if (ret) {
		string output = outputBuffer->getContent();
		app->getOutputStack().pop();
		delete outputBuffer;
		return output;
	}
	return "";
}
