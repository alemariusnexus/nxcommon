/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of nxcommon.

	nxcommon is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	nxcommon is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with nxcommon.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "CString.h"





CString& CString::ltrim(const char* chars)
{
	char* s = d.get();

	char* os = s;

	size_t len = length();
	while (s-os != len  &&  strchr(chars, *s)  !=  NULL) s++;

	growWithOffset(mcapacity, s-os, 0);

	return *this;

	/*char* nStr = new char[bufSize];
	strcpy(nStr, s);

	cstr = shared_ptr<char>(nStr, default_delete<char[]>());

	return *this;*/
}


/*CString CString::substr(size_t begin, size_t len) const
{
	char* sub = new char[len+1];
	memcpy(sub, d.get() + begin, len);
	sub[len] = '\0';
	return CString::from(sub, len);
}*/


CString& CString::append(long val)
{
	char buf[64];
	sprintf(buf, "%ld", val);
	return append(buf);
}


CString& CString::append(unsigned long val)
{
	char buf[64];
	sprintf(buf, "%lu", val);
	return append(buf);
}


CString& CString::append(int val)
{
	char buf[32];
	sprintf(buf, "%d", val);
	return append(buf);
}


CString& CString::append(unsigned int val)
{
	char buf[32];
	sprintf(buf, "%u", val);
	return append(buf);
}


CString& CString::append(float val)
{
	return append((double) val);
}


CString& CString::append(double val)
{
	char buf[64];
	sprintf(buf, "%f", val);
	return append(buf);
}


CString& CString::appendHex(unsigned int val)
{
	char buf[32];
	sprintf(buf, "%X", val);
	return append(buf);
}


CString& CString::appendHex(unsigned long val)
{
	char buf[64];
	sprintf(buf, "%lX", val);
	return append(buf);
}



