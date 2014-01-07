/*
 * TestWidget.cpp
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#include "TestWidget.h"
#include <base/Jvibetto.h>

TestWidget::TestWidget(CBaseController * owner)
: CWidget(owner)
{
}

TestWidget::~TestWidget()
{
}

boost::filesystem::path TestWidget::getLocalViewPath() const
{
	boost::filesystem::path path(Jvibetto::app()->getBasePath().string() + "/" + __FILE__);
	return boost::filesystem::path(path.parent_path().normalize().string() + "/views");
}

void TestWidget::init()
{

}

void TestWidget::run() throw (CException)
{
	*this
	<< "<p class=\"note\">"
	<< "ACDC"
	<< "</p>";
}
