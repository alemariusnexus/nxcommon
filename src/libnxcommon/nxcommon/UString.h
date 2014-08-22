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

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
#include <QtCore/QString>
#endif


class UString : public AbstractSharedBuffer<UString, UChar, true>
{
public:
	static UString from(UChar* s, size_t len, size_t bufSize)
			{ return UString(s, len, bufSize-1, false); }
	static UString from(UChar* s, size_t len)
			{ return from(s, len, len+1); }
	static UString from(UChar* s)
			{ return from(s, u_strlen(s)); }

	static UString writeAlias(UChar* s, size_t len, size_t bufSize)
			{ return UString(s, len, bufSize-1, false, false); }
	static UString writeAlias(UChar* s, size_t len)
			{ return writeAlias(s, len, len+1); }
	static UString writeAlias(UChar* s)
			{ return writeAlias(s, u_strlen(s)); }

	static UString readAlias(const UChar* s, size_t len)
			{ return UString(s, len, false, false, false); }
	static UString readAlias(const UChar* s)
			{ return readAlias(s, u_strlen(s)); }

	static UString fromUTF8(const ByteArray& utf8);
	static UString fromASCII(const CString& ascii);

public:
	UString() : AbstractSharedBuffer() {}
	UString(const UString& other) : AbstractSharedBuffer(other) {}
	UString(const UChar* str, size_t len) : AbstractSharedBuffer(str, len) {}
	UString(const UChar* str) : AbstractSharedBuffer(str, u_strlen(str)) {}

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
	UString(const QString& str) : UString((const UChar*) str.utf16()) {}

	operator QString() const { return QString((const QChar*) get(), length()); }
#endif

	UString& append(const UString& other) { AbstractSharedBuffer::append(other); return *this; }
	UString& append(UChar c) { AbstractSharedBuffer::append(c); return *this; }

	UString& append(long val);
	UString& append(unsigned long val);
	UString& append(int val);
	UString& append(unsigned int val);
	UString& append(float val);
	UString& append(double val);

	UString& operator<<(const UString& other) { return append(other); }
	UString& operator<<(UChar c) { return append(c); }
	UString& operator<<(long val) { return append(val); }
	UString& operator<<(unsigned long val) { return append(val); }
	UString& operator<<(int val) { return append(val); }
	UString& operator<<(unsigned int val) { return append(val); }
	UString& operator<<(float val) { return append(val); }
	UString& operator<<(double val) { return append(val); }

	UString& prepend(const UString& other) { AbstractSharedBuffer::prepend(other); return *this; }
	UString& prepend(UChar c) { AbstractSharedBuffer::prepend(c); return *this; }

	UString& lower(const CString& locale = CString());
	UString& upper(const CString& locale = CString());

	bool isWhitespaceOnly() const;

	UString& ltrim();
	UString& rtrim();
	UString& trim();

	bool operator<(const UString& other) const { return memcmp(d.get(), other.d.get(), msize*2) < 0; }
	bool operator>(const UString& other) const { return memcmp(d.get(), other.d.get(), msize*2) > 0; }
	bool operator<=(const UString& other) const { return !(*this > other); }
	bool operator>=(const UString& other) const { return !(*this < other); }
	bool operator==(const UString& other) const { return memcmp(d.get(), other.d.get(), msize*2) == 0; }
	bool operator!=(const UString& other) const { return !(*this == other); }

	size_t toUTF8(char* dest, size_t destSize) const;
	ByteArray toUTF8() const;

private:
	UString(UChar* data, size_t size, size_t bufSize, bool)
			: AbstractSharedBuffer(data, size, bufSize, default_delete<UChar[]>())
	{
	}

	UString(UChar* data, size_t size, size_t bufSize, bool, bool)
			: AbstractSharedBuffer(data, size, bufSize, false)
	{
	}

	UString(const UChar* data, size_t size, bool, bool, bool)
			: AbstractSharedBuffer(data, size, false, false)
	{
	}
};

#endif /* USTRING_H_ */
