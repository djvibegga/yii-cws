/*
 * CMemoryOutputBuffer.h
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#ifndef CMEMORYOUTPUTBUFFER_H_
#define CMEMORYOUTPUTBUFFER_H_

#include "base/CComponent.h"

class CMemoryOutputBuffer: public CComponent, public IOutputBuffer
{
private:
	string _content;

public:
	virtual ~CMemoryOutputBuffer();
	virtual string getContent() const;
	virtual void echo(const string & content);
	virtual IOutputBuffer & operator<< (const string &right);
};

#endif /* CMEMORYOUTPUTBUFFER_H_ */