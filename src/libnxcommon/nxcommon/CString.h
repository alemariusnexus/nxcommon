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

#ifndef NXCOMMON_CSTRING_H_
#define NXCOMMON_CSTRING_H_

#include <nxcommon/config.h>
#include "AbstractSharedString.h"
#include "strutil.h"
#include "ByteArray.h"

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
#include <QtCore/QString>
#endif




/**	\brief A simple and lightweight string class using data sharing.
 *
 * 	This class is used to store strings in an unspecified 8-bit character encoding and charset. It assumes that the charset
 * 	used is a subset of ASCII (meaning: all ASCII strings are valid and textually equivalent in that encoding) and works
 * 	using various functions from the C standard library.
 *
 * 	It can be used to store 8-bit based multi-byte encodings like UTF-8, but are not aware of them. Because UTF-8 is backwards
 * 	compatible with ASCII, many methods in this class are valid for UTF-8 strings, but don't expect any locale- or Unicode-aware
 * 	data handling.
 *
 * 	See the documentation for the AbstractSharedString class for more information.
 *
 *	@see AbstractSharedString
 *	@see AbstractSharedBuffer
 *	@see UString
 *	@see ByteArray
 */
class CString : public AbstractSharedString<CString, char>
{
	friend class AbstractSharedString<CString, char>;

public:
	using AbstractSharedString::AbstractSharedString;

	CString() : AbstractSharedString() {}

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
	CString(const QString& str) : CString(str.toUtf8().constData()) {}

	operator QString() const { return QString::fromUtf8(get(), length()); }
#endif

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

protected:
	static CString convertFromLongLong(long long val, unsigned int base);
	static CString convertFromULongLong(unsigned long long val, unsigned int base);
	static CString convertFromDouble(double val);
};

#endif /* NXCOMMON_CSTRING_H_ */
