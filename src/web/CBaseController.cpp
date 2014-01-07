#include <web/CBaseController.h>
#include <web/CHttpResponse.h>
#include <web/CWebApplication.h>
#include <base/Jvibetto.h>
#include <base/CMemoryOutputBuffer.h>
#include <base/CStringUtils.h>
#include <sys/time.h>
#include "config.h"

#ifdef JV_DEBUG
#include <iostream>
#endif

string CBaseController::renderFile(const string & viewFile, cpptempl::data_map & data, bool ret)
{
    unsigned int widgetCount = _widgetStack.size();
    IViewRenderer * renderer = dynamic_cast<IViewRenderer*>(Jvibetto::app()->getComponent("viewRenderer"));
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

string CBaseController::renderInternal(const string & viewFile, cpptempl::data_map & data, bool ret) const
{
	wstring text = cpptempl::utf8_to_wide(CStringUtils::fileGetContents(viewFile));
    if (!data.empty()) {
    	cpptempl::token_vector tokens;
    	cpptempl::tokenize(text, tokens);
    	cpptempl::token_vector tree;
    	parse_tree(tokens, tree);
    	std::wostringstream stream;
#ifdef JV_DEBUG
    timeval time;
    gettimeofday(&time, 0);
    long microsecBegin = ((unsigned long long)time.tv_sec * 1000000) + time.tv_usec;
    stringstream ss;
    ss << "Begin processing view render. View file: \"" << viewFile << "\", Microtime: " << microsecBegin;
    Jvibetto::trace(ss.str());
#endif
        cpptempl::render(stream, tree, data);
#ifdef JV_DEBUG
    gettimeofday(&time, 0);
    long microsecEnd = ((unsigned long long)time.tv_sec * 1000000) + time.tv_usec;
    ss.str("");
    ss << "End processing view render. View file: \"" << viewFile << "\", Microtime: " << microsecEnd << ", Diff ms: " << (microsecEnd - microsecBegin) / 1000;
    Jvibetto::trace(ss.str());
#endif
        text = stream.str();
    }
    if (ret) {
        return cpptempl::wide_to_utf8(text);
    } else {
    	IOutputBuffer * outputBuffer = Jvibetto::app()->getOutputStack().top();
		outputBuffer->echo(cpptempl::wide_to_utf8(text).c_str());
    	return "";
    }
}

string CBaseController::renderInternal(IView & viewInstance, bool ret) const
{
	CApplication * app = Jvibetto::app();
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
