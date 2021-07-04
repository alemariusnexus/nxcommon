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
#include <stdint.h>

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
#include <QString>
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
class ByteArray : public AbstractSharedBuffer<ByteArray, uint8_t>
{
public:
	template <typename Deleter>
	static ByteArray fromCustomDelete(uint8_t* s, size_t size, size_t bufSize, Deleter del)
			{ return ByteArray(s, size, bufSize, del); }
	static ByteArray from(uint8_t* s, size_t size, size_t bufSize)
			{ return fromCustomDelete(s, size, bufSize, default_delete<uint8_t[]>()); }
	static ByteArray fromMalloc(uint8_t* s, size_t size, size_t bufSize)
			{ return fromCustomDelete(s, size, bufSize, MallocFreeDeleter()); }

	template <typename Deleter>
	static ByteArray fromCustomDelete(uint8_t* s, size_t size, Deleter del)
			{ return fromCustomDelete(s, size, size, del); }
	static ByteArray from(uint8_t* s, size_t size)
			{ return fromCustomDelete(s, size, default_delete<uint8_t[]>()); }
	static ByteArray fromMalloc(uint8_t* s, size_t size)
			{ return fromCustomDelete(s, size, MallocFreeDeleter()); }

	static ByteArray writeAlias(uint8_t* s, size_t size, size_t bufSize)
			{ return ByteArray(s, size, bufSize, false, false); }
	static ByteArray writeAlias(uint8_t* s, size_t size)
			{ return writeAlias(s, size, size); }

	static ByteArray readAlias(const uint8_t* s, size_t size)
			{ return ByteArray(s, size, false, false, false); }

public:
	ByteArray() : AbstractSharedBuffer() {}
	ByteArray(size_t capacity) : AbstractSharedBuffer(capacity) {}
	ByteArray(const uint8_t* data, size_t size, size_t capacity) : AbstractSharedBuffer(data, size, capacity) {}
	ByteArray(const uint8_t* data, size_t size) : AbstractSharedBuffer(data, size) {}

	template <typename ODerivedT, typename OUnitT, bool oterminated, OUnitT oterm>
	ByteArray(const AbstractSharedBuffer<ODerivedT, OUnitT, oterminated, oterm>& other)
			: AbstractSharedBuffer(std::reinterpret_pointer_cast<uint8_t>(other.d), other.msize*sizeof(OUnitT), other.mcapacity*sizeof(OUnitT),
					std::reinterpret_pointer_cast<uint8_t>(other.readAliasDummy), other.isnull) {}

	ByteArray(const ByteArray& other) : AbstractSharedBuffer(other) {}

#ifdef NXCOMMON_QT_SUPPORT_ENABLED
	ByteArray(const QByteArray& barr) : ByteArray((const uint8_t*) barr.data(), barr.length()) {}
	ByteArray(const QString& str) : ByteArray(str.toUtf8()) {}

	operator QString() const { return QString::fromUtf8((const char*) get()); }
	operator QByteArray() const { return QByteArray((const char*) get(), (int) length()); }
#endif

	ByteArray& append(const ByteArray& other) { AbstractSharedBuffer::append(other); return *this; }
	ByteArray& append(uint8_t c) { AbstractSharedBuffer::append(c); return *this; }

	ByteArray& prepend(const ByteArray& other) { AbstractSharedBuffer::prepend(other); return *this; }
	ByteArray& prepend(uint8_t c) { AbstractSharedBuffer::prepend(c); return *this; }

	bool operator<(const ByteArray& other) const { return memcmp(d.get(), other.d.get(), msize) < 0; }
	bool operator>(const ByteArray& other) const { return memcmp(d.get(), other.d.get(), msize) > 0; }
	bool operator<=(const ByteArray& other) const { return !(*this > other); }
	bool operator>=(const ByteArray& other) const { return !(*this < other); }
	bool operator==(const ByteArray& other) const { return memcmp(d.get(), other.d.get(), msize) == 0; }
	bool operator!=(const ByteArray& other) const { return !(*this == other); }

private:
	template <typename Deleter>
	ByteArray(uint8_t* data, size_t size, size_t bufSize, Deleter del = default_delete<uint8_t[]>())
			: AbstractSharedBuffer(data, size, bufSize, del)
	{
	}

	ByteArray(uint8_t* data, size_t size, size_t bufSize, bool, bool)
			: AbstractSharedBuffer(data, size, bufSize, false)
	{
	}

	ByteArray(const uint8_t* data, size_t size, bool, bool, bool)
			: AbstractSharedBuffer(data, size, false, false)
	{
	}
};

#endif /* NXCOMMON_BYTEARRAY_H_ */
