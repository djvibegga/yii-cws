/*
 * CController.cpp
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#include <web/CController.h>
#include <web/CWebModule.h>
#include <base/CException.h>
#include <base/Jvibetto.h>

CController::CController(const string & id, CModule * module)
: CBaseController(),
  CApplicationComponent(id, module)
{
	if (module != 0) {
		module->setController(id, this);
	}
}

CController::~CController()
{

}

void CController::init()
{
    setViewPath(resolveViewPath());
    CApplicationComponent::init();
}

void CController::registerAction(const string &name, TAction action)
{
	_actions[name] = action;
}

void CController::run(const string &actionId, CHttpRequest * const request, CHttpResponse * const response) throw (CHttpException)
{
	TActionsMap::const_iterator iter = _actions.find(actionId);
	if (iter == _actions.end()) {
		throw CHttpException(404, "Route not found");
	}
	TAction action = iter->second;
	(this->*action)(request, response);
}

string CController::getViewFile(const string & viewName) const throw (CException)
{
    /*if (($theme=Yii::app()->getTheme())!==null && ($viewFile=$theme->getViewFile($this,$viewName))!==false)
        return $viewFile;*/
    boost::filesystem::path basePath = Jvibetto::app()->getViewPath();
    boost::filesystem::path moduleViewPath = basePath;
    CModule * module = getModule();
    if (module != 0) {
        moduleViewPath = module->getViewPath();
    }
    return resolveViewFile(viewName, getViewPath(), basePath, moduleViewPath);
}

boost::filesystem::path CController::resolveViewPath() const
{
    CModule * module = getModule();
    if (module == 0) {
        module = Jvibetto::app();
    }
    return boost::filesystem::path(module->getViewPath().string() + "/" + getId());
}

boost::filesystem::path CController::getViewPath() const
{
    return _viewPath;
}

void CController::setViewPath(const string & path)
{
    setViewPath(boost::filesystem::path(path));
}

void CController::setViewPath(const boost::filesystem::path & path)
{
    _viewPath = path;
}

string CController::resolveViewFile(
        const string & viewName,
        const boost::filesystem::path & viewPath,
        const boost::filesystem::path & basePath,
        const boost::filesystem::path & moduleViewPath) const throw (CException)
{
    if (viewName.empty()) {
        throw CException("Empty view name was passed");
    }
    boost::filesystem::path modViewPath;
    if (moduleViewPath.empty()) {
        modViewPath = basePath;
    } else {
        modViewPath = moduleViewPath;
    }
    IViewRenderer * renderer = Jvibetto::app()->getViewRenderer();
    if (renderer == 0) {
        throw CException("Sorry. View renderer component is not attached.");
    }
    string viewFile;
    if (viewName[0] == '/') {
        if (strncmp(viewName.c_str(), "//", 2) == 0) {
            viewFile = basePath.string() + viewName;
        } else {
            viewFile = modViewPath.string() + viewName;
        }
    } else if (viewName.find(".") != ::string::npos) {
        viewFile = Jvibetto::getPathOfAlias(viewName).string();
    } else {
        viewFile = viewPath.string() + "/" + viewName;
    }
    viewFile += renderer->fileExtension;
    return boost::filesystem::exists(boost::filesystem::path(viewFile)) ? viewFile : "";
}

string CController::renderPartial(const string & view, cpptempl::data_map & data, bool ret, bool processOutput) throw (CException)
{
    string viewFile = getViewFile(view);
    if (!viewFile.empty()) {
        try {
            string output = renderFile(viewFile, data, true);
            if (processOutput) {
                output = this->processOutput(output);
            }
            if (ret) {
                return output;
            } else {
                CHttpResponse * response = dynamic_cast<CHttpResponse*>(Jvibetto::app()->getComponent("response"));
                response->echo(output);
            }
        } catch (cpptempl::TemplateException & e) {
            throw CException(string("View render exception occured: ") + e.what());
        }
    } else {
        throw CException("Controller cannot find the requested view \"{" + view + "\".");
    }
    return "";
}

string CController::getLayoutFile(const string & layoutName) throw (CException)
{
	if (layoutName.empty()) {
		return "";
	}
	/*if (($theme=Yii::app()->getTheme())!==null && ($layoutFile=$theme->getLayoutFile($this,$layoutName))!==false)
		return $layoutFile;*/

	CModule * module = 0;
	string layout = layoutName;
	if (layoutName.empty()) {
		module = getModule();
		while (module != 0) {
			if (dynamic_cast<CWebModule*>(module) == 0) {
				throw CException("Invalid module instance. Any module of an web application must be a web module.");
			} else if (!(dynamic_cast<CWebModule*>(module))->layout.empty()) {
				break;
			}
			module = dynamic_cast<CModule*>(module->getParent());
		}
		if (module == 0) {
			module = Jvibetto::app();
		}
		layout = dynamic_cast<CWebModule*>(module)->layout;
	} else if ((module = dynamic_cast<CWebModule*>(getModule())) == 0) {
		module = Jvibetto::app();
	}
	IHasLayout * moduleWithLayout = dynamic_cast<IHasLayout*>(module);
	return resolveViewFile(
		layout, moduleWithLayout->getLayoutPath(),
		Jvibetto::app()->getViewPath(), module->getViewPath()
	);
}

string CController::render(const string & view, cpptempl::data_map & data, bool ret) throw (CException)
{
	if (beforeRender(view)) {
		string output = renderPartial(view, data, true);
		string layoutFile = getLayoutFile(layout);
		if (!layoutFile.empty()) {
			cpptempl::data_map viewData;
			viewData["content"] = output;
			output = renderFile(layoutFile, viewData, true);
		}
		afterRender(view, output);
		output = processOutput(output);

		if (ret) {
			return output;
		} else {
			Jvibetto::app()->getOutputStack().top()->echo(output);
		}
	}
	return "";
}

bool CController::beforeRender(const string & view)
{
	return true;
}

void CController::afterRender(const string & view, string &output)
{
}

string CController::processOutput(const string & output)
{
	/*Yii::app()->getClientScript()->render($output);

	// if using page caching, we should delay dynamic output replacement
	if($this->_dynamicOutput!==null && $this->isCachingStackEmpty())
	{
		$output=$this->processDynamicOutput($output);
		$this->_dynamicOutput=null;
	}

	if($this->_pageStates===null)
		$this->_pageStates=$this->loadPageStates();
	if(!empty($this->_pageStates))
		$this->savePageStates($this->_pageStates,$output);*/

	return output;
}
