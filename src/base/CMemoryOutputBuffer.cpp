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

string CMemoryOutputBuffer::getContent() const
{
	return _content;
}

void CMemoryOutputBuffer::echo(const string & content)
{
	_content += content;
}

IOutputBuffer & CMemoryOutputBuffer::operator<< (const string &right)
{
	_content += right;
	return *this;
}
