/*
 * CMemoryOutputBuffer.cpp
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#include "base/CMemoryOutputBuffer.h"
#include "base/CStringUtils.h"

CMemoryOutputBuffer::~CMemoryOutputBuffer()
{
}

_string CMemoryOutputBuffer::getContent() const
{
	return _content;
}

void CMemoryOutputBuffer::echo(const wstring & content)
{
	_content += wide_to_(content);
}

void CMemoryOutputBuffer::echo(const string & content)
{
	_content += utf8_to_(content);
}

IOutputBuffer & CMemoryOutputBuffer::operator<< (const wstring &right)
{
	_content += wide_to_(right);
	return *this;
}

IOutputBuffer & CMemoryOutputBuffer::operator<< (const string &right)
{
	_content += utf8_to_(right);
	return *this;
}
