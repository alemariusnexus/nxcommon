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

#ifndef CSTRING_H_
#define CSTRING_H_

#include "AbstractSharedBuffer.h"
#include "strutil.h"
#include "ByteArray.h"

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
#include <QtCore/QString>
#endif




class CString : public AbstractSharedBuffer<CString, char, true>
{
public:
	static CString from(char* s, size_t len, size_t bufSize)
			{ return CString(s, len, bufSize-1, false); }
	static CString from(char* s, size_t len)
			{ return from(s, len, len+1); }
	static CString from(char* s)
			{ return from(s, strlen(s)); }

	static CString writeAlias(char* s, size_t len, size_t bufSize)
			{ return CString(s, len, bufSize-1, false, false); }
	static CString writeAlias(char* s, size_t len)
			{ return writeAlias(s, len, len+1); }
	static CString writeAlias(char* s)
			{ return writeAlias(s, strlen(s)); }

	static CString readAlias(const char* s, size_t len)
			{ return CString(s, len, false, false, false); }
	static CString readAlias(const char* s)
			{ return readAlias(s, strlen(s)); }

	/*static CString from(char* s)
			{ return from(s, strlen(s)+1); }
	static CString from(char* s, size_t bufSize)
			{ CString c; c.cstr = shared_ptr<char>(s, default_delete<char[]>()); c.bufSize = bufSize; return c; }*/

public:
	CString(const char* cstr, size_t len)
			: AbstractSharedBuffer(cstr, len, len)
	{
	}
	CString(const char* cstr)
			: AbstractSharedBuffer(cstr, cstr ? strlen(cstr) : 0)
	{
	}
	CString(const ByteArray& barr)
			: CString(barr.get(), nxstrnlen(barr.get(), barr.length()))
	{
	}
	CString(const CString& other) : AbstractSharedBuffer(other) {}
	CString() : AbstractSharedBuffer() {}
	CString(size_t capacity) : AbstractSharedBuffer(capacity) {}

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
	CString(const QString& str) : CString(str.toUtf8().constData()) {}

	operator QString() const { return QString::fromUtf8(get(), length()); }
#endif

	using AbstractSharedBuffer::resize;
	size_t resize() { msize = strlen(d.get()); }

	CString& lower() { ensureUniqueness(); strtolower(d.get(), d.get()); return *this; }
	CString& upper() { ensureUniqueness(); strtoupper(d.get(), d.get()); return *this; }

	CString& ltrim(char c) { char b[2]; b[0] = c; b[1] = '\0'; return ltrim(b); }
	CString& ltrim(const char* chars);
	CString& ltrim() { return ltrim(" \t\r\n"); }
	CString& rtrim(char c) { ensureUniqueness(); msize = ::rtrim(d.get(), c) - d.get(); return *this; }
	CString& rtrim(const char* chars) { ensureUniqueness(); msize = ::rtrim(d.get(), chars) - d.get(); return *this; }
	CString& rtrim() { return rtrim(" \t\r\n"); }
	CString& trim(char c) { rtrim(c); ltrim(c); return *this; }
	CString& trim(const char* chars) { rtrim(chars); ltrim(chars); return *this; }
	CString& trim() { return trim(" \t\r\n"); }

	using AbstractSharedBuffer::append;
	CString& append(long val);
	CString& append(unsigned long val);
	CString& append(int val);
	CString& append(unsigned int val);
	CString& append(float val);
	CString& append(double val);
	CString& appendHex(unsigned int val);
	CString& appendHex(unsigned long val);

	CString& operator<<(const CString& other) { return append(other); }
	CString& operator<<(char c) { return append(c); }
	CString& operator<<(long val) { return append(val); }
	CString& operator<<(unsigned long val) { return append(val); }
	CString& operator<<(int val) { return append(val); }
	CString& operator<<(unsigned int val) { return append(val); }
	CString& operator<<(float val) { return append(val); }
	CString& operator<<(double val) { return append(val); }

	using AbstractSharedBuffer::substr;
	CString substr(size_t begin) const { return substr(begin, length()-begin); }

	bool operator<(const CString& other) const { return strcmp(d.get(), other.d.get()) < 0; }
	bool operator>(const CString& other) const { return strcmp(d.get(), other.d.get()) > 0; }
	bool operator<=(const CString& other) const { return !(*this > other); }
	bool operator>=(const CString& other) const { return !(*this < other); }
	bool operator==(const CString& other) const { return strcmp(d.get(), other.d.get()) == 0; }
	bool operator!=(const CString& other) const { return !(*this == other); }
	char& operator[](size_t idx) { ensureUniqueness(); return d.get()[idx]; }
	const char& operator[](size_t idx) const { return d.get()[idx]; }
	CString& operator=(const CString& other) { assign(other); return *this; }
	CString& operator+=(const CString& other) { append(other); return *this; }

private:
	CString(char* data, size_t size, size_t bufSize, bool)
			: AbstractSharedBuffer(data, size, bufSize, default_delete<char[]>())
	{
	}

	CString(char* data, size_t size, size_t bufSize, bool, bool)
			: AbstractSharedBuffer(data, size, bufSize, false)
	{
	}

	CString(const char* data, size_t size, bool, bool, bool)
			: AbstractSharedBuffer(data, size, false, false)
	{
	}
};

#endif /* CSTRING_H_ */
