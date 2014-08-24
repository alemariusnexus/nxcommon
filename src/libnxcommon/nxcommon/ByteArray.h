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

#ifndef NXCOMMON_BYTEARRAY_H_
#define NXCOMMON_BYTEARRAY_H_

#include <nxcommon/config.h>
#include "AbstractSharedBuffer.h"
#include <cstring>

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
#include <QtCore/QString>
#endif




/**	\brief A simple and lightweight class for handling byte arrays with content sharing.
 *
 *	The data it stores may contain embedded null characters. The buffer is __not__ null terminated.
 *
 * 	See the documentation for the AbstractSharedBuffer class for more information.
 *
 *	@see AbstractSharedBuffer
 *	@see CString
 *	@see UString
 */
class ByteArray : public AbstractSharedBuffer<ByteArray, char>
{
public:
	static ByteArray from(char* s, size_t size, size_t bufSize)
			{ return ByteArray(s, size, bufSize, false); }
	static ByteArray from(char* s, size_t size)
			{ return from(s, size, size); }

	static ByteArray writeAlias(char* s, size_t size, size_t bufSize)
			{ return ByteArray(s, size, bufSize, false, false); }
	static ByteArray writeAlias(char* s, size_t size)
			{ return writeAlias(s, size, size); }

	static ByteArray readAlias(const char* s, size_t size)
			{ return ByteArray(s, size, false, false, false); }

public:
	ByteArray() : AbstractSharedBuffer() {}
	ByteArray(const char* data, size_t size, size_t capacity) : AbstractSharedBuffer(data, size, capacity) {}
	ByteArray(const char* data, size_t size) : AbstractSharedBuffer(data, size) {}

	template <typename ODerivedT, typename OUnitT, bool oterminated, OUnitT oterm>
	ByteArray(const AbstractSharedBuffer<ODerivedT, OUnitT, oterminated, oterm>& other)
			: AbstractSharedBuffer(other.d, other.msize*sizeof(OUnitT), other.mcapacity*sizeof(OUnitT), other.readAliasDummy) {}

	ByteArray(const ByteArray& other) : AbstractSharedBuffer(other) {}

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
	ByteArray(const QByteArray& barr) : ByteArray(barr.data(), barr.length()) {}
	ByteArray(const QString& str) : ByteArray(str.toUtf8()) {}

	operator QString() const { return QString::fromUtf8(get()); }
	operator QByteArray() const { return QByteArray(get(), length()); }
#endif

	ByteArray& append(const ByteArray& other) { AbstractSharedBuffer::append(other); return *this; }
	ByteArray& append(char c) { AbstractSharedBuffer::append(c); return *this; }

	ByteArray& prepend(const ByteArray& other) { AbstractSharedBuffer::prepend(other); return *this; }
	ByteArray& prepend(char c) { AbstractSharedBuffer::prepend(c); return *this; }

	bool operator<(const ByteArray& other) const { return memcmp(d.get(), other.d.get(), msize) < 0; }
	bool operator>(const ByteArray& other) const { return memcmp(d.get(), other.d.get(), msize) > 0; }
	bool operator<=(const ByteArray& other) const { return !(*this > other); }
	bool operator>=(const ByteArray& other) const { return !(*this < other); }
	bool operator==(const ByteArray& other) const { return memcmp(d.get(), other.d.get(), msize) == 0; }
	bool operator!=(const ByteArray& other) const { return !(*this == other); }

private:
	ByteArray(char* data, size_t size, size_t bufSize, bool)
			: AbstractSharedBuffer(data, size, bufSize, default_delete<char[]>())
	{
	}

	ByteArray(char* data, size_t size, size_t bufSize, bool, bool)
			: AbstractSharedBuffer(data, size, bufSize, false)
	{
	}

	ByteArray(const char* data, size_t size, bool, bool, bool)
			: AbstractSharedBuffer(data, size, false, false)
	{
	}
};

#endif /* NXCOMMON_BYTEARRAY_H_ */
