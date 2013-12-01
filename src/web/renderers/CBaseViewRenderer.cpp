/*
 * CBaseViewRenderer.cpp
 *
 *  Created on: 27 ����. 2013 �.
 *      Author: djvibegga
 */

#include <web/renderers/CBaseViewRenderer.h>

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
    return true;
}
