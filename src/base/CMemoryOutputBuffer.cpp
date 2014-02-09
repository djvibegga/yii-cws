/*
 * CMemoryOutputBuffer.cpp
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#include "base/CMemoryOutputBuffer.h"

CMemoryOutputBuffer::~CMemoryOutputBuffer()
{
}

_string CMemoryOutputBuffer::getContent() const
{
	return _content;
}

void CMemoryOutputBuffer::echo(const _string & content)
{
	_content += content;
}

IOutputBuffer & CMemoryOutputBuffer::operator<< (const _string &right)
{
	_content += right;
	return *this;
}
