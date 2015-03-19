/*
 * CApplicationPool.cpp
 *
 *  Created on: Nov 6, 2014
 *      Author: djvibegga
 */

#include "base/CApplicationPool.h"
#include "base/CAsyncTask.h"

CApplicationPool::CApplicationPool(const string &configPath, int argc, char * const argv[])
: configPath(configPath),
  argc(argc),
  argv(argv),
  _isActive(false)
{
	_xmlConfig = new xml_document();
}

CApplicationPool::~CApplicationPool()
{
}

void CApplicationPool::init() throw (CException)
{
	xml_parse_result result = _xmlConfig->load_file(configPath.c_str());
	if (result.status != status_ok) {
		stringstream ss;
		ss << "Can\'t parse application config: '" << configPath << "'.";
		throw CException(ss.str());
	}

	CApplication::_mainThreadId = CApplication::convertThreadIdToLong(boost::this_thread::get_id());
}

void CApplicationPool::run() throw (CException)
{
	_isActive = true;

	if (boost::filesystem::exists("server.pid")) {
		throw CException("Application pool already has been started.");
	}

	const xml_node &configRoot = _xmlConfig->root();
	int instancesCount = 0;
	PARSE_XML_CONF_UINT_PROPERTY(
		configRoot.child("server").child("instance"),
		instancesCount, "count"
	);
	cout << "Starting the application with " << instancesCount << " instances: " << endl;
	for (int i = 0; i < instancesCount; ++i) {
		CApplication * instance = createAppInstance();
		instance->setPool(this);
		CAsyncTask * task = new CAsyncTask(instance);
		task->init();
		task->run();
		cout << "The application instance " << (i + 1) << " has runned..." << endl;
		usleep(10000); //TODO: remove this hardcore. This is temp fix to run miltiple app instances
	}
	cout << "The application pool has runned." << endl;
}

const xml_document & CApplicationPool::getConfigDocument() const
{
	return *_xmlConfig;
}

bool CApplicationPool::getIsActive() const
{
	return _isActive;
}
