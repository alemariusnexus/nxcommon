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




CString CString::fromFloatWithMaxPrecision(float val, int precision)
{
	const char* fmt = "%.*f";
	int bufSize = snprintf(NULL, 0, fmt, (int) precision, val);
	char* buf = new char[bufSize+1];
	snprintf(buf, bufSize+1, fmt, (int) precision, val);
	CString str = CString::from(buf, bufSize, bufSize+1);

	// TODO: This is not how you're supposed to implement different decimal point chars...
	if (strchr(buf, '.')  ||  strchr(buf, ',')) {
		str.rtrim('0');

		if (str.get()[str.length()-1] == '.'  ||  str.get()[str.length()-1] == ',') {
			// We removed all digits after the decimal point. In that case we want to keep a single zero
			str.append('0');
		}
	}

	return str;
}


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


CString CString::indented(const CString& indent, bool indentStart, const CString& newline) const
{
	char* indStr = ::indent(get(), indent.get(), indentStart, newline.get());
	return CString::from(indStr);
}



std::ostream& operator<<(std::ostream& stream, const CString& cstr)
{
	return stream << cstr.get();
}



