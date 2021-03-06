/*
 * CController.cpp
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#include "web/CController.h"
#include "base/CStringUtils.h"
#include "web/CWebModule.h"
#include "base/CException.h"
#include "base/Cws.h"
#include "web/CTemplateView.h"
#include "web/CLayoutView.h"
#include "web/CClientScript.h"

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

string CController::getClassName() const
{
	return "CController";
}

void CController::init()
{
    setViewPath(resolveViewPath());
    CApplicationComponent::init();
}

TViewPtr CController::getLayout() const
{
	return _layout;
}

void CController::setLayout(const string & layout)
{
	CTemplateView * instance = new CTemplateView(getLayoutFile(layout), this);
	_layout = TViewPtr(instance);
}

void CController::setLayout(TViewPtr layout)
{
	_layout = layout;
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
	_actionId = actionId;
	(this->*action)(request, response);
	_actionId = "";
}

string CController::getViewFile(const string & viewName) const throw (CException)
{
    /*if (($theme=Yii::app()->getTheme())!==null && ($viewFile=$theme->getViewFile($this,$viewName))!==false)
        return $viewFile;*/
    boost::filesystem::path basePath = Cws::app()->getViewPath();
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
        module = Cws::app();
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
    IViewRenderer * renderer = Cws::app()->getViewRenderer();
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
        viewFile = Cws::getPathOfAlias(viewName).string();
    } else {
        viewFile = viewPath.string() + "/" + viewName;
    }
    viewFile += renderer->fileExtension;
    return boost::filesystem::exists(boost::filesystem::path(viewFile)) ? viewFile : "";
}

string CController::renderPartial(const string & view, CDT & data, bool ret, bool processOutput) throw (CException)
{
    string viewFile = getViewFile(view);
    if (!viewFile.empty()) {
        try {
            string output = renderFile(viewFile, data, true);
            if (processOutput) {
                this->processOutput(output);
            }
            if (ret) {
                return output;
            } else {
                CHttpResponse * response = dynamic_cast<CHttpResponse*>(Cws::app()->getComponent("response"));
                response->echo(output);
            }
        } catch (CTPPException & e) {
            throw CException(string("View render exception occured: ") + e.what());
        }
    } else {
        throw CException("Controller cannot find the requested view \"{" + view + "\".");
    }
    return "";
}

string CController::renderPartial(IView & viewInstance, bool ret, bool processOutput) throw (CException)
{
	string output = renderInternal(viewInstance, true);
	if (processOutput) {
		this->processOutput(output);
	}
	if (ret) {
		return output;
	} else {
		Cws::app()->getOutputStack().top()->echo(output);
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
			module = Cws::app();
		}
		layout = dynamic_cast<CWebModule*>(module)->layout;
	} else if ((module = dynamic_cast<CWebModule*>(getModule())) == 0) {
		module = Cws::app();
	}
	IHasLayout * moduleWithLayout = dynamic_cast<IHasLayout*>(module);
	return resolveViewFile(
		layout, moduleWithLayout->getLayoutPath(),
		Cws::app()->getViewPath(), module->getViewPath()
	);
}

string CController::render(const string & view, CDT & data, bool ret) throw (CException)
{
	CTemplateView viewInstance(getViewFile(view), data, this);
	return render(viewInstance, ret);
}

string CController::render(IView & viewInstance, bool ret) throw (CException)
{
	if (beforeRender(viewInstance)) {
		string output = renderPartial(viewInstance, true, false);
		IView * layout = viewInstance.getLayout().get();
		if (layout != 0) {
			CTemplateView * templateLayout = dynamic_cast<CTemplateView*>(layout);
			if (templateLayout != 0) {
				CDT viewData;
				viewData["content"] = output;
				templateLayout->setData(viewData);
			} else {
				CLayoutView * nativeLayout = dynamic_cast<CLayoutView*>(layout);
				if (nativeLayout != 0) {
					nativeLayout->content = output;
				} else {
					throw CException("Invalid layout instance was given", 0, __FILE__, __LINE__);
				}
			}
			output = renderInternal(*layout, true);
		}
		afterRender(viewInstance, output);
		processOutput(output);

		if (ret) {
			return output;
		} else {
			Cws::app()->getOutputStack().top()->echo(output);
		}
	}
	return "";
}

bool CController::beforeRender(const IView & viewInstance)
{
	return true;
}

void CController::afterRender(const IView & viewInstance, string &output)
{
}

void CController::processOutput(string & output)
{
	CClientScript * cs = dynamic_cast<CClientScript*>(Cws::app()->getComponent("clientScript"));
	if (cs) {
		cs->render(output);
	}
//
//	// if using page caching, we should delay dynamic output replacement
//	if($this->_dynamicOutput!==null && $this->isCachingStackEmpty())
//	{
//		$output=$this->processDynamicOutput($output);
//		$this->_dynamicOutput=null;
//	}
//
//	if($this->_pageStates===null)
//		$this->_pageStates=$this->loadPageStates();
//	if(!empty($this->_pageStates))
//		$this->savePageStates($this->_pageStates,$output);*/
}

string CController::getUniqueId() const
{
	CModule * module = getModule();
	return module != 0 ? module->getId() + "/" + getId() : getId();
}

string CController::getRoute() const
{
	if (_actionId.empty()) {
		return getUniqueId();
	} else {
		return getUniqueId() + "/" + _actionId;
	}
}

void CController::setAction(const string & actionId)
{
	_actionId = actionId;
}

string CController::getAction() const
{
	return _actionId;
}

string CController::createUrl(TRouteStruct & route, const string & ampersand) const
{
	if (route.path.empty()) {
		route.path = getId() + "/" + getAction();
	} else if (route.path.find("/") == ::string::npos) {
		route.path = getId() + "/" + route.path;
	}
	CModule * module = getModule();
	if (module != 0 && route.path[0] != '/') {
		route.path = module->getId() + "/" + route.path;
	}
	route.path = CStringUtils::trim(route.path, "/");
	return Cws::app()->createUrl(route, ampersand);
}
