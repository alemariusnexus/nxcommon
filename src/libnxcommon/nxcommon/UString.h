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

#ifndef NXCOMMON_USTRING_H_
#define NXCOMMON_USTRING_H_

#include <nxcommon/config.h>
#include "AbstractSharedString.h"
#include "CString.h"
#include "ByteArray.h"
#include <unicode/uchar.h>
#include <unicode/utf16.h>
#include <unicode/ustring.h>
#include <ostream>

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
#include <QString>
#endif



/**	\brief A simple and lightweight UTF-16 string class using data sharing.
 *
 * 	This class stores and handles characters encoded in UTF-16. It is a lightweight class, so don't expect sophisticated
 * 	locale-aware data handling, but the methods of this class generally understand Unicode, unless otherwise noted.
 *
 * 	See the documentation for the AbstractSharedString class for more information.
 *
 *	@see AbstractSharedString
 *	@see AbstractSharedBuffer
 *	@see CString
 *	@see ByteArray
 */
class UString : public AbstractSharedString<UString, UChar>
{
	friend class AbstractSharedString<UString, UChar>;

public:
	static UString fromUTF8(const CString& utf8);
	static UString fromASCII(const CString& ascii);

public:
	// NOTE: Constructor inheritance is not implemented in VS 2013...
	UString() : AbstractSharedString() {}
	UString(const UString& other) : AbstractSharedString(other) {}
	UString(const UChar* str, size_t len) : AbstractSharedString(str, len) {}
	UString(const UChar* str) : AbstractSharedString(str) {}
	UString(size_t capacity) : AbstractSharedString(capacity) {}
	UString(const ByteArray& other) : AbstractSharedString(other) {}

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
	UString(const QString& str) : UString((const UChar*) str.utf16()) {}

	operator QString() const { return QString((const QChar*) get(), length()); }
#endif

	UString& lower(const CString& locale = CString());
	UString& upper(const CString& locale = CString());

	bool isWhitespaceOnly() const;

	UString& ltrim();
	UString& rtrim();
	UString& trim();

	size_t toUTF8(char* dest, size_t destSize) const;
	CString toUTF8() const;

	ASS_DEFINE_PROTECTED_CONSTRUCTORS(UString)

protected:
	static UString convertFromLongLong(long long val, unsigned int base);
	static UString convertFromULongLong(unsigned long long val, unsigned int base);
	static UString convertFromDouble(double val);
};




std::ostream& operator<<(std::ostream& stream, const UString& cstr);

#endif /* NXCOMMON_USTRING_H_ */
