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

#include "Reader.h"




CString Reader::readFixedLengthString(size_t len)
{
	char* buf = new char[len+1];
	buf[len] = '\0';
	size_t numRead = read(buf, len);
	return CString::from(buf, len+1);
}


CString Reader::readNullTerminatedString(bool* terminatorFound)
{
	CString str("");
	str.reserve(64);

	char buf[4096];

	bool terminated = false;
	size_t numRead;

	char* bufEnd = buf+sizeof(buf);

	do {
		char* dest;
		for (dest = buf ; dest != bufEnd  &&  !terminated  &&  numRead == 1 ; dest++) {
			numRead = read(dest, 1);

			if (*dest == '\0')
				terminated = true;
		}

		str.append(CString(buf, dest-buf));
	} while (!terminated  &&  numRead == 1);

	if (terminatorFound)
		*terminatorFound = terminated;

	return str;
}
