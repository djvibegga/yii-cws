#include "base/CTemplateEngine.h"

CTemplateEngine::CTemplateEngine(CModule * module)
: CApplicationComponent("templateEngine", module),
  _vm(0),
  _scFactory(0),
  maxHandlers(100),
  maxArgStackSize(4096),
  maxCodeStackSize(4096),
  maxSteps(10240)
{
}

string CTemplateEngine::getClassName() const
{
	return "CTemplateEngine";
}

CTemplateEngine::~CTemplateEngine()
{
	if (_vm != 0) {
		delete _vm;
	}
	if (_scFactory != 0) {
		delete _scFactory;
	}
}

void CTemplateEngine::init()
{
	CApplicationComponent::init();

	_scFactory = new SyscallFactory(maxHandlers);
	registerExtHandlers(_scFactory);
	STDLibInitializer::InitLibrary(*_scFactory);
	_vm = new VM(_scFactory, maxArgStackSize, maxCodeStackSize, maxSteps);
}

void CTemplateEngine::applyConfig(const xml_node & config)
{
	PARSE_XML_CONF_UINT_PROPERTY(config, maxHandlers, "maxHandlers");
	PARSE_XML_CONF_UINT_PROPERTY(config, maxArgStackSize, "maxArgStackSize");
	PARSE_XML_CONF_UINT_PROPERTY(config, maxCodeStackSize, "maxCodeStackSize");
	PARSE_XML_CONF_UINT_PROPERTY(config, maxSteps, "maxSteps");
}

void CTemplateEngine::registerExtHandlers(SyscallFactory * scFactory)
{
}

VM * CTemplateEngine::getVM() const
{
	return _vm;
}

SyscallFactory * CTemplateEngine::getSyscallFactory() const
{
	return _scFactory;
}
