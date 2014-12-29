/*
 * CBaseViewRenderer.cpp
 *
 *  Created on: 27 ����. 2013 �.
 *      Author: djvibegga
 */

#include <web/renderers/CBaseViewRenderer.h>
#include <stdlib.h>

CBaseViewRenderer::CBaseViewRenderer(CModule * module)
: CViewRenderer(module)
{
}

CBaseViewRenderer::CBaseViewRenderer(const string &id, CModule * module)
: CViewRenderer(id, module)
{
}

bool CBaseViewRenderer::generateViewFile(const string & sourceFile, const string & viewFile)
{
	string command = "ctpp2c " + sourceFile + " " + viewFile;
	return system(command.c_str()) == 0;
}
