#include <web/CBaseController.h>
#include <web/CHttpResponse.h>
#include <base/Jvibetto.h>
#include <base/CStringUtils.h>
#include <sys/time.h>
#include "config.h"

#ifdef JV_DEBUG
#include <iostream>
#endif;

string CBaseController::renderFile(const string & viewFile, cpptempl::data_map * data, bool ret)
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

string CBaseController::renderInternal(const string & viewFile, cpptempl::data_map * data, bool ret) const
{
#ifdef JV_DEBUG
    timeval time;
    gettimeofday(&time, 0);
    long microsecBegin = ((unsigned long long)time.tv_sec * 1000000) + time.tv_usec;
    stringstream ss;
    ss << "Begin processing view render. View file: \"" << viewFile << "\", Microtime: " << microsecBegin;
    Jvibetto::trace(ss.str());
#endif
    string text = CStringUtils::fileGetContents(viewFile);
    if (data != 0) {
        text = cpptempl::parse(text, *data);
    }
#ifdef JV_DEBUG
    gettimeofday(&time, 0);
    long microsecEnd = ((unsigned long long)time.tv_sec * 1000000) + time.tv_usec;
    ss.str("");
    ss << "End processing view render. View file: \"" << viewFile << "\", Microtime: " << microsecEnd << ", Diff: " << (microsecEnd - microsecBegin);
    Jvibetto::trace(ss.str());
#endif
    if (ret) {
        return text;
    } else {
        CHttpResponse * response = dynamic_cast<CHttpResponse*>(Jvibetto::app()->getComponent("response"));
        response->echo(text);
        return "";
    }
}

