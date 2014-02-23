/*
 * CTemplateView.cpp
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#include "web/CTemplateView.h"
#include "base/cpptempl.h"
#include "base/CProfiler.h"
#include "base/Jvibetto.h"
#include "utils/CFile.h"

TDynamicTemplateCacheMap CTemplateView::_dynamicTemplateCache;
TStaticTemplateCacheMap CTemplateView::_staticTemplateCache;

CTemplateView::CTemplateView(const string & viewFile, const cpptempl::data_map & data, const CBaseController * owner)
: CView(owner),
  _viewFile(viewFile),
  _data(data)
{
}

CTemplateView::CTemplateView(const string & viewFile, const CBaseController * owner)
: CView(owner),
  _viewFile(viewFile)
{
}

CTemplateView::~CTemplateView()
{
}

void CTemplateView::setData(const cpptempl::data_map & data)
{
	_data = cpptempl::data_map(data);
}

cpptempl::data_map & CTemplateView::getData()
{
	return _data;
}

void CTemplateView::init()
{
	CView::init();
}

void CTemplateView::run() throw (CException)
{
	PROFILE_BEGIN("CTemplateView::run(). View file: " + _viewFile);
	cpptempl::data_map & data = getData();
	wstring text;
	TStaticTemplateCacheMap::const_iterator found = _staticTemplateCache.find(_viewFile);
	if (found == _staticTemplateCache.end()) {
		_staticTemplateCache[_viewFile] = text = cpptempl::utf8_to_wide(
			CFile::getContents(_viewFile)
		);
	} else {
		text = found->second;
	}
	if (!data.empty()) {
		cpptempl::token_vector tree;
		TDynamicTemplateCacheMap::const_iterator found = _dynamicTemplateCache.find(_viewFile);
		if (found == _dynamicTemplateCache.end()) {
			cpptempl::token_vector tokens;
			cpptempl::tokenize(text, tokens);
			parse_tree(tokens, tree);
			_dynamicTemplateCache[_viewFile] = tree;
		} else {
			tree = found->second;
		}
		std::wostringstream stream;
		cpptempl::render(stream, tree, data);
		text = stream.str();
	}
	Jvibetto::app()->getOutputStack().top()->echo(
#ifdef _UNICODE
		text
#else
	    cpptempl::wide_to_utf8(text)
#endif
	);
	PROFILE_END();
}
