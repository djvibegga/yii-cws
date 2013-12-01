/*
 * CBaseViewRenderer.h
 *
 *  Created on: 27 םמ‗ב. 2013 ד.
 *      Author: djvibegga
 */

#ifndef CBASEVIEWRENDERER_H_
#define CBASEVIEWRENDERER_H_

#include <web/renderers/CViewRenderer.h>

class CBaseViewRenderer: public CViewRenderer
{
public:
    CBaseViewRenderer(CModule * module);
    CBaseViewRenderer(const string &id, CModule * module);

protected:
    virtual bool generateViewFile(const string & sourceFile, const string & viewFile);
};

#endif /* CBASEVIEWRENDERER_H_ */
