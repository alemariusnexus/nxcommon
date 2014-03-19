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

#ifndef BYTEARRAY_H_
#define BYTEARRAY_H_

#include "AbstractSharedBuffer.h"
#include <cstring>



class ByteArray : public AbstractSharedBuffer<char>
{
public:
	static ByteArray from(char* s, size_t size, size_t bufCapacity)
			{ return ByteArray(s, size, bufCapacity, false); }
	static ByteArray from(char* s, size_t size)
			{ return from(s, size, size); }

	static ByteArray writeAlias(char* s, size_t size, size_t bufCapacity)
			{ return ByteArray(s, size, bufCapacity, false, false); }
	static ByteArray writeAlias(char* s, size_t size)
			{ return writeAlias(s, size, size); }

	static ByteArray readAlias(const char* s, size_t size)
			{ return ByteArray(s, size, false, false, false); }

public:
	ByteArray() : AbstractSharedBuffer() {}
	ByteArray(const ByteArray& other) : AbstractSharedBuffer(other) {}
	ByteArray(const char* data, size_t size, size_t capacity) : AbstractSharedBuffer(data, size, capacity) {}
	ByteArray(const char* data, size_t size) : AbstractSharedBuffer(data, size) {}

	char* getMutable() { ensureUniqueness(); return d.get(); }
	const char* get() const { return d.get(); }
	size_t length() const { return size; }

	void setSize(size_t size) { this->size = size; }

	ByteArray& append(const ByteArray& other) { AbstractSharedBuffer::append(other); return *this; }
	ByteArray& append(char c) { AbstractSharedBuffer::append(c); return *this; }

	ByteArray& prepend(const ByteArray& other) { AbstractSharedBuffer::prepend(other); return *this; }
	ByteArray& prepend(char c) { AbstractSharedBuffer::prepend(c); return *this; }

	bool operator<(const ByteArray& other) const { return memcmp(d.get(), other.d.get(), size) < 0; }
	bool operator>(const ByteArray& other) const { return memcmp(d.get(), other.d.get(), size) > 0; }
	bool operator<=(const ByteArray& other) const { return !(*this > other); }
	bool operator>=(const ByteArray& other) const { return !(*this < other); }
	bool operator==(const ByteArray& other) const { return memcmp(d.get(), other.d.get(), size) == 0; }
	bool operator!=(const ByteArray& other) const { return !(*this == other); }

	ByteArray& operator=(const ByteArray& other) { assign(other); return *this; }

private:
	ByteArray(char* data, size_t size, size_t capacity, bool)
			: AbstractSharedBuffer(data, size, capacity, default_delete<char[]>())
	{
	}

	ByteArray(char* data, size_t size, size_t capacity, bool, bool)
			: AbstractSharedBuffer(data, size, capacity, false)
	{
	}

	ByteArray(const char* data, size_t size, bool, bool, bool)
			: AbstractSharedBuffer(data, size, false, false)
	{
	}
};

#endif /* BYTEARRAY_H_ */
