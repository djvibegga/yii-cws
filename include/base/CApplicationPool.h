/*
 * CApplicationPool.h
 *
 *  Created on: Nov 6, 2014
 *      Author: djvibegga
 */

#ifndef CAPPLICATIONPOOL_H_
#define CAPPLICATIONPOOL_H_

#include "interfaces.h"
#include "base/CApplication.h"

class CApplicationPool: public IRunable
{
public:
	CApplicationPool(const string &configPath, int argc, char * const argv[]);
	virtual ~CApplicationPool();
	virtual void init() throw (CException);
	virtual void run() throw (CException);
	const xml_document & getConfigDocument() const;

	virtual CApplication * createAppInstance() const = 0;

protected:
	string configPath;
	int argc;
	char * const * argv;

private:
	xml_document * _xmlConfig;
};

#endif /* CAPPLICATIONPOOL_H_ */
