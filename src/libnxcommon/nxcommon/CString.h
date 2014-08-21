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




class CString : public AbstractSharedBuffer<char, 1>
{
public:
	static CString from(char* s, size_t len, size_t bufCapacity)
			{ return CString(s, len+1, bufCapacity, false); }
	static CString from(char* s, size_t len)
			{ return from(s, len, len+1); }
	static CString from(char* s)
			{ return from(s, strlen(s)); }

	static CString writeAlias(char* s, size_t len, size_t bufCapacity)
			{ return CString(s, len+1, bufCapacity, false, false); }
	static CString writeAlias(char* s, size_t len)
			{ return writeAlias(s, len, len+1); }
	static CString writeAlias(char* s)
			{ return writeAlias(s, strlen(s)); }

	static CString readAlias(const char* s, size_t len)
			{ return CString(s, len+1, false, false, false); }
	static CString readAlias(const char* s)
			{ return readAlias(s, strlen(s)); }

	/*static CString from(char* s)
			{ return from(s, strlen(s)+1); }
	static CString from(char* s, size_t bufSize)
			{ CString c; c.cstr = shared_ptr<char>(s, default_delete<char[]>()); c.bufSize = bufSize; return c; }*/

public:
	CString(const char* cstr, size_t len)
			: AbstractSharedBuffer(cstr, len+1, len+1)
	{
		if (cstr)
			d.get()[len] = '\0';
	}
	CString(const char* cstr)
			: AbstractSharedBuffer(cstr, cstr ? strlen(cstr)+1 : 1)
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

	size_t length() const { return size-1; }
	size_t getSize() const { return length(); }
	CString& lower() { ensureUniqueness(); strtolower(d.get(), d.get()); return *this; }
	CString& upper() { ensureUniqueness(); strtoupper(d.get(), d.get()); return *this; }
	CString& ltrim(char c) { char b[2]; b[0] = c; b[1] = '\0'; return ltrim(b); }
	CString& ltrim(const char* chars);
	CString& ltrim() { return ltrim(" \t\r\n"); }
	CString& rtrim(char c) { ensureUniqueness(); ::rtrim(d.get(), c); recalculateSize(); return *this; }
	CString& rtrim(const char* chars) { ensureUniqueness(); ::rtrim(d.get(), chars); recalculateSize(); return *this; }
	CString& rtrim() { return rtrim(" \t\r\n"); }
	CString& trim(char c) { rtrim(c); ltrim(c); return *this; }
	CString& trim(const char* chars) { rtrim(chars); ltrim(chars); return *this; }
	CString& trim() { return trim(" \t\r\n"); }
	const char* get() const { return d.get(); }
	char* mget() { ensureUniqueness(); return d.get(); }
	CString& append(const CString& other) { AbstractSharedBuffer::append(other); return *this; }
	CString& append(char c) { AbstractSharedBuffer::append(c); return *this;  }
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

	CString substr(size_t begin, size_t len) const;
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
	void reserve(size_t size) { return grow(size); }

	bool isNull() const { return AbstractSharedBuffer::isNull(); }
	bool isEmpty() const { return isNull()  ||  d.get()[0] == '\0'; }

private:
	CString(char* data, size_t size, size_t capacity, bool)
			: AbstractSharedBuffer(data, size, capacity, default_delete<char[]>())
	{
	}

	CString(char* data, size_t size, size_t capacity, bool, bool)
			: AbstractSharedBuffer(data, size, capacity, false)
	{
	}

	CString(const char* data, size_t size, bool, bool, bool)
			: AbstractSharedBuffer(data, size, false, false)
	{
	}

	void recalculateSize() { size = strlen(d.get())+1; }
};

#endif /* CSTRING_H_ */
