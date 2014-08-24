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


CString CString::convertFromLongLong(long long val, unsigned int base)
{
	CString str = convertFromULongLong(llabs(val), base);

	if (val < 0) {
		str.prepend('-');
	}

	return str;
}


CString CString::convertFromULongLong(unsigned long long val, unsigned int base)
{
	CString str(128);
	str.resize(ULongLongToString(str.mget(), val, base));
	return str;
}


CString CString::convertFromDouble(double val)
{
	CString str(64);
	sprintf(str.mget(), "%f", val);
	str.resize();
	return str;
}



