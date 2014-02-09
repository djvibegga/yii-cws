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
	_string _content;

public:
	virtual ~CMemoryOutputBuffer();
	virtual _string getContent() const;
	virtual void echo(const _string & content);
	virtual IOutputBuffer & operator<< (const _string &right);
};

#endif /* CMEMORYOUTPUTBUFFER_H_ */
