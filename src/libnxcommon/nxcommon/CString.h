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
#include <cstdio>
#include "AbstractSharedString.h"
#include "strutil.h"
#include "ByteArray.h"
#include <string>
#include <ostream>

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
	static CString fromFloatWithMaxPrecision(float val, int precision);

	template <typename... Args>
	static CString format(const char* fmt, Args... args);

public:
	// NOTE: Constructor inheritance is not implemented in VS 2013...

	CString() : AbstractSharedString() {}
	CString(const CString& other) : AbstractSharedString(other) {}
	CString(const char* str, size_t len) : AbstractSharedString(str, len) {}
	CString(const char* str) : AbstractSharedString(str) {}
	CString(size_t capacity) : AbstractSharedString(capacity) {}
	CString(const ByteArray& other) : AbstractSharedString(other) {}
	CString(const std::string& str) : AbstractSharedString(str.c_str(), str.length()) {}

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

	ASS_DEFINE_PROTECTED_CONSTRUCTORS(CString)

protected:
	static CString convertFromLongLong(long long val, unsigned int base);
	static CString convertFromULongLong(unsigned long long val, unsigned int base);
	static CString convertFromDouble(double val);
};



namespace std
{
	template <>
	struct hash<CString>
	{
		typedef CString argument_type;
		typedef size_t result_type;

		result_type operator()(const argument_type& s) const
		{
			// TODO: This is slow because it copies the string. We would need to implement our
			// own hash function...
			return strHash(std::string(s.get(), s.length()));
		}
		std::hash<std::string> strHash;
	};
}








template <typename... Args>
CString CString::format(const char* fmt, Args... args)
{
	int len = snprintf(nullptr, 0, fmt, args...);
	char* buf = new char[len+1];
	snprintf(buf, len+1, fmt, args...);
	return CString::from(buf, len, len+1);
}



std::ostream& operator<<(std::ostream& stream, const CString& cstr);

#endif /* NXCOMMON_CSTRING_H_ */
