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

#include <memory>
#include <cstring>
#include "strutil.h"

using std::shared_ptr;
using std::default_delete;



class CString {
public:
	static CString from(char* s)
			{ return from(s, strlen(s)+1); }
	static CString from(char* s, size_t bufSize)
			{ CString c; c.cstr = shared_ptr<char>(s, default_delete<char[]>()); c.bufSize = bufSize; return c; }

public:
	CString(const char* cstr, size_t len)
	{
		if (cstr) {
			bufSize = len+1;
			this->cstr = shared_ptr<char>(new char[bufSize], default_delete<char[]>());
			this->cstr.get()[len] = '\0';
			strncpy(this->cstr.get(), cstr, len);
		} else {
			bufSize = 0;
			this->cstr = shared_ptr<char>((char*) NULL, default_delete<char[]>());
		}
	}
	CString(const char* cstr)
	{
		if (cstr) {
			size_t len = strlen(cstr);
			bufSize = len+1;
			this->cstr = shared_ptr<char>(new char[len+1], default_delete<char[]>());
			strcpy(this->cstr.get(), cstr);
		} else {
			bufSize = 0;
			this->cstr = shared_ptr<char>((char*) NULL, default_delete<char[]>());
		}
	}
	CString(const CString& other) : cstr(other.cstr), bufSize(other.bufSize) {}
	CString() : cstr((char*) NULL), bufSize(0) {}
	size_t length() const { return cstr.get() ? strlen(cstr.get()) : 0; }
	CString& lower() { ensureUniqueness(); strtolower(cstr.get(), cstr.get()); return *this; }
	CString& upper() { ensureUniqueness(); strtoupper(cstr.get(), cstr.get()); return *this; }
	CString& ltrim(char c) { char b[2]; b[0] = c; b[1] = '\0'; return ltrim(b); }
	CString& ltrim(const char* chars);
	CString& ltrim() { return ltrim(" \t\r\n"); }
	CString& rtrim(char c) { ensureUniqueness(); ::rtrim(cstr.get(), c); return *this; }
	CString& rtrim(const char* chars) { ensureUniqueness(); ::rtrim(cstr.get(), chars); return *this; }
	CString& rtrim() { return rtrim(" \t\r\n"); }
	CString& trim(char c) { rtrim(c); ltrim(c); return *this; }
	CString& trim(const char* chars) { rtrim(chars); ltrim(chars); return *this; }
	CString& trim() { return trim(" \t\r\n"); }
	const char* get() const { return cstr.get(); }
	CString& append(const CString& other);

	CString substr(size_t begin, size_t len) const;
	CString substr(size_t begin) const { return substr(begin, length()-begin); }

	bool operator<(const CString& other) const { return strcmp(cstr.get(), other.cstr.get()) < 0; }
	bool operator>(const CString& other) const { return strcmp(cstr.get(), other.cstr.get()) > 0; }
	bool operator<=(const CString& other) const { return !(*this > other); }
	bool operator>=(const CString& other) const { return !(*this < other); }
	bool operator==(const CString& other) const
	{
		return cstr.get() == 0  ||  other.cstr.get() == 0
				? cstr.get() == other.cstr.get()
				: strcmp(cstr.get(), other.cstr.get()) == 0;
	}
	bool operator!=(const CString& other) const { return !(*this == other); }
	char& operator[](size_t idx) { ensureUniqueness(); return cstr.get()[idx]; }
	const char& operator[](size_t idx) const { return cstr.get()[idx]; }
	CString& operator=(const CString& other) { cstr = other.cstr; bufSize = other.bufSize; return *this; }
	CString& operator+=(const CString& other) { append(other); return *this; }
	void reserve(size_t size) { grow(size); }

	bool isNull() const { return cstr.get() == NULL; }
	bool isEmpty() const { return isNull()  ||  cstr.get()[0] == '\0'; }

private:
	void ensureUniqueness() { if (!cstr.unique()) copy(); }
	void grow(size_t minSize);
	void realloc(size_t size);
	void copy() { realloc(bufSize); }

private:
	shared_ptr<char> cstr;
	size_t bufSize;
};




inline CString operator+(const CString& s1, const CString& s2)
{
	CString s;
	s.reserve(s1.length() + s2.length());
	s.append(s1);
	s.append(s2);
	return s;
}

#endif /* CSTRING_H_ */
