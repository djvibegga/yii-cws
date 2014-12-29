/*
 * CTemplateEngine.h
 *
 *  Created on: Dec 27, 2014
 *      Author: djvibegga
 */

#ifndef CTEMPLATEENGINE_H_
#define CTEMPLATEENGINE_H_

#include <ctpp2/CTPP2SyscallFactory.hpp>
#include <ctpp2/CTPP2VM.hpp>
#include <ctpp2/CTPP2VMSTDLib.hpp>

#include "base/CApplicationComponent.h"

using namespace CTPP;

class CTemplateEngine: public CApplicationComponent
{
private:
	VM * _vm;
	SyscallFactory * _scFactory;

public:
	unsigned int maxHandlers;
	unsigned int maxArgStackSize;
	unsigned int maxCodeStackSize;
	unsigned int maxSteps;

	CTemplateEngine(CModule * module);
	virtual ~CTemplateEngine();
	virtual void init();
	VM * getVM() const;
	SyscallFactory * getSyscallFactory() const;

protected:
	virtual void applyConfig(const xml_node & config);
	virtual void registerExtHandlers(SyscallFactory * scFactory);
};

#endif /* CTEMPLATEENGINE_H_ */
