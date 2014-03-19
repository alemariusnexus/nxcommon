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

#ifndef USTRING_H_
#define USTRING_H_

#include "AbstractSharedBuffer.h"
#include "CString.h"
#include "ByteArray.h"
#include <unicode/uchar.h>
#include <unicode/utf16.h>
#include <unicode/ustring.h>


class UString : public AbstractSharedBuffer<UChar, 1>
{
public:
	static UString from(UChar* s, size_t len, size_t bufCapacity)
			{ return UString(s, len+1, bufCapacity, false); }
	static UString from(UChar* s, size_t len)
			{ return from(s, len, len+1); }
	static UString from(UChar* s)
			{ return from(s, u_strlen(s)); }

	static UString writeAlias(UChar* s, size_t len, size_t bufCapacity)
			{ return UString(s, len+1, bufCapacity, false, false); }
	static UString writeAlias(UChar* s, size_t len)
			{ return writeAlias(s, len, len+1); }
	static UString writeAlias(UChar* s)
			{ return writeAlias(s, u_strlen(s)); }

	static UString readAlias(const UChar* s, size_t len)
			{ return UString(s, len+1, false, false, false); }
	static UString readAlias(const UChar* s)
			{ return readAlias(s, u_strlen(s)); }

	static UString fromUTF8(const ByteArray& utf8);

public:
	UString() : AbstractSharedBuffer() {}
	UString(const UString& other) : AbstractSharedBuffer(other) {}
	UString(const UChar* str, size_t len) : AbstractSharedBuffer(str, len+1) {}
	UString(const UChar* str) : AbstractSharedBuffer(str, u_strlen(str)+1) {}

	size_t length() const { return size-1; }
	bool isEmpty() const { return size == 1; }
	const UChar* get() const { return d.get(); }

	UString& append(const UString& other) { AbstractSharedBuffer::append(other); return *this; }
	UString& append(UChar c) { AbstractSharedBuffer::append(c); return *this; }

	UString& prepend(const UString& other) { AbstractSharedBuffer::prepend(other); return *this; }
	UString& prepend(UChar c) { AbstractSharedBuffer::prepend(c); return *this; }

	UString& lower(const CString& locale = CString());
	UString& upper(const CString& locale = CString());

	bool isWhitespaceOnly() const;

	UString& ltrim();
	UString& rtrim();
	UString& trim();

	bool operator<(const UString& other) const { return memcmp(d.get(), other.d.get(), size*2) < 0; }
	bool operator>(const UString& other) const { return memcmp(d.get(), other.d.get(), size*2) > 0; }
	bool operator<=(const UString& other) const { return !(*this > other); }
	bool operator>=(const UString& other) const { return !(*this < other); }
	bool operator==(const UString& other) const { return memcmp(d.get(), other.d.get(), size*2) == 0; }
	bool operator!=(const UString& other) const { return !(*this == other); }

	size_t toUTF8(char* dest, size_t destSize) const;
	ByteArray toUTF8() const;

private:
	UString(UChar* data, size_t size, size_t capacity, bool)
			: AbstractSharedBuffer(data, size, capacity, default_delete<UChar[]>())
	{
	}

	UString(UChar* data, size_t size, size_t capacity, bool, bool)
			: AbstractSharedBuffer(data, size, capacity, false)
	{
	}

	UString(const UChar* data, size_t size, bool, bool, bool)
			: AbstractSharedBuffer(data, size, false, false)
	{
	}
};

#endif /* USTRING_H_ */
