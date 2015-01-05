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

string CMemoryOutputBuffer::getContent() const
{
	return _content;
}

void CMemoryOutputBuffer::echo(const wstring & content)
{
	_content += CStringUtils::wide_to_utf8(content);
}

void CMemoryOutputBuffer::echo(const string & content)
{
	_content += content;
}

IOutputBuffer & CMemoryOutputBuffer::operator<< (const wstring &right)
{
	_content += CStringUtils::wide_to_utf8(right);
	return *this;
}

IOutputBuffer & CMemoryOutputBuffer::operator<< (const string &right)
{
	_content += right;
	return *this;
}
