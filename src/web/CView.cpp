/*
 * CView.cpp
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#include "web/CView.h"
#include "base/Jvibetto.h"
#include <typeinfo>

TViewPathMap CView::_viewPaths;

CView::CView(const CBaseController * owner)
: CBaseController(),
  _owner(owner),
  _outputBuffer(0)
{
}

CView::~CView()
{
}

const CBaseController * CView::getOwner() const
{
	return _owner;
}

const CController * CView::getController() const
{
	const CController * casted = reinterpret_cast<const CController*>(_owner);
	if (casted != 0) {
		return casted;
	} else {
		return 0;
	}
}

boost::filesystem::path CView::getLocalViewPath() const
{
	return boost::filesystem::path("");
}

boost::filesystem::path CView::getViewPath(bool checkTheme) const
{
	string className = typeid(this).name();
	string scope = checkTheme ? "theme" : "local";
	string key = className + scope;
	TViewPathMap::const_iterator found = _viewPaths.find(key);
	if (found != _viewPaths.end()) {
		return found->second;
	} else {
		/*if (checkTheme && ($theme=Yii::app()->getTheme())!==null) {
			$path=$theme->getViewPath().DIRECTORY_SEPARATOR;
			$path.=$className;
			if(is_dir($path))
				return self::$_viewPaths[$className]['theme']=$path;
		}*/
		boost::filesystem::path path = getLocalViewPath();
		_viewPaths[key] = path;
		return path;
	}
}

string CView::getViewFile(const string & viewName) const throw (CException)
{
	IViewRenderer * renderer = Jvibetto::app()->getViewRenderer();
	string viewFile;
	if (viewName.find(".") != ::string::npos) {
		viewFile = Jvibetto::getPathOfAlias(viewName).string() + renderer->fileExtension;
	} else {
		viewFile = getViewPath(true).string() + "/" + viewName + renderer->fileExtension;
		if (boost::filesystem::exists(boost::filesystem::path(viewFile))) {
			return viewFile;
		} else {
			viewFile = getViewPath(false).string() + "/" + viewName + renderer->fileExtension;
		}
	}
	if (boost::filesystem::exists(boost::filesystem::path(viewFile))) {
		return viewFile;
	}
	return "";
}

string CView::render(const string & view, cpptempl::data_map & data, bool ret) throw (CException)
{
	string viewFile = getViewFile(view);
	if (!viewFile.empty()) {
		return renderFile(viewFile, data, ret);
	} else {
		stringstream ss;
		ss << "Can't find the view \"" << view << "\"";
		throw CException(ss.str());
	}
}

void CView::setOutputBuffer(IOutputBuffer * buffer)
{
	_outputBuffer = buffer;
}

IOutputBuffer * CView::getOutputBuffer() const
{
	return _outputBuffer;
}

string CView::getContent() const
{
	return _outputBuffer->getContent();
}

void CView::echo(const string & content)
{
	_outputBuffer->echo(content);
}

IOutputBuffer & CView::operator<< (const string &right)
{
	*_outputBuffer << right;
	return *this;
}
