/*
 * CTemplateView.cpp
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#include "web/CTemplateView.h"
#include "base/cpptempl.h"
#include "base/CStringUtils.h"
#include "base/Jvibetto.h"

TTemplateCacheMap CTemplateView::_templateCache;

CTemplateView::CTemplateView(const string & viewFile, cpptempl::data_map & data, const CBaseController * owner)
: CView(owner),
  _viewFile(viewFile),
  _data(data)
{
}

CTemplateView::~CTemplateView()
{
}

cpptempl::data_map & CTemplateView::getData() const
{
	return _data;
}

void CTemplateView::init()
{
}

void CTemplateView::run() throw (CException)
{
	wstring text = cpptempl::utf8_to_wide(CStringUtils::fileGetContents(_viewFile));
	cpptempl::data_map data = getData();
	if (!data.empty()) {
		cpptempl::token_vector tree;
		TTemplateCacheMap::const_iterator found = _templateCache.find(_viewFile);
		if (found == _templateCache.end()) {
			cpptempl::token_vector tokens;
			cpptempl::tokenize(text, tokens);
			parse_tree(tokens, tree);
			_templateCache[_viewFile] = tree;
		} else {
			tree = found->second;
		}
		std::wostringstream stream;
		cpptempl::render(stream, tree, data);
		text = stream.str();
	}
	Jvibetto::app()->getOutputStack().top()->echo(cpptempl::wide_to_utf8(text).c_str());
}
