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

#ifndef ABSTRACTSHAREDBUFFER_H_
#define ABSTRACTSHAREDBUFFER_H_

#include <cstring>
#include <memory>
#include <algorithm>
#include "util.h"

using std::shared_ptr;
using std::default_delete;
using std::min;
using std::max;




template <typename T>
class _NopDeleter
{
public:
	_NopDeleter() {}
	void operator()(T*) const {}
};



template <typename UnitT, size_t termLen = 0, UnitT term = 0>
class AbstractSharedBuffer
{
private:
	static const shared_ptr<UnitT> _EmptyPtr;

public:
	bool isNull() const { return isnull; }
	size_t getSize() const { return size; }
	size_t getLength() const { return size-termLen; }
	size_t getCapacity() const { return capacity; }

	void squeeze() { resizeExactly(size); }

protected:
	AbstractSharedBuffer();
	AbstractSharedBuffer(const AbstractSharedBuffer<UnitT, termLen, term>& other);
	AbstractSharedBuffer(const UnitT* data, size_t size);
	AbstractSharedBuffer(const UnitT* data, size_t size, size_t capacity);

	template <typename Deleter>
	AbstractSharedBuffer(UnitT* data, size_t size, size_t capacity, Deleter del = default_delete<UnitT[]>());

	// Write-aliasing
	AbstractSharedBuffer(UnitT* data, size_t size, size_t capacity, bool);

	// Read-aliasing
	AbstractSharedBuffer(const UnitT* data, size_t size, bool, bool);

	void resizeExactly(size_t newCapacity);
	void resizeExactlyWithOffset(size_t newCapacity, size_t srcOffset, size_t destOffset, size_t copyLen);
	void resizeExactlyWithSplit(size_t newCapacity, size_t srcOffset1, size_t destOffset1, size_t copyLen1,
			size_t srcOffset2, size_t destOffset2, size_t copyLen2);

	void growExactly(size_t newCapacity);
	void growExactlyWithOffset(size_t newCapacity, size_t srcOffset, size_t destOffset, size_t copyLen);
	void growExactlyWithSplit(size_t newCapacity, size_t srcOffset1, size_t destOffset1, size_t copyLen1,
			size_t srcOffset2, size_t destOffset2, size_t copyLen2);

	void grow(size_t minCapacity);
	void growWithOffset(size_t minCapacity, size_t srcOffset, size_t destOffset, size_t copyLen);
	void growWithSplit(size_t minCapacity, size_t srcOffset1, size_t destOffset1, size_t copyLen1,
			size_t srcOffset2, size_t destOffset2, size_t copyLen2);

	void ensureUniqueness();

	void append(const AbstractSharedBuffer<UnitT, termLen, term>& other);
	void append(UnitT u);

	void prepend(const AbstractSharedBuffer<UnitT, termLen, term>& other);
	void prepend(UnitT u);

	void insert(size_t index, const AbstractSharedBuffer<UnitT, termLen, term>& other);
	void insert(size_t index, UnitT u);

	void assign(const AbstractSharedBuffer& other);

private:
	static size_t getGrownCapacity(size_t newCapacity);

protected:
	shared_ptr<UnitT> d;
	size_t size;
	size_t capacity;
	bool isnull;

private:
	shared_ptr<UnitT> readAliasDummy;
};


template <typename UnitT, size_t termLen, UnitT term>
const shared_ptr<UnitT> AbstractSharedBuffer<UnitT, termLen, term>::_EmptyPtr
		= shared_ptr<UnitT>(new UnitT[1] {term}, default_delete<UnitT[]>());




template <typename UnitT, size_t termLen, UnitT term>
AbstractSharedBuffer<UnitT, termLen, term>::AbstractSharedBuffer()
		: d(_EmptyPtr), size(termLen), capacity(termLen), isnull(true)
{
}


template <typename UnitT, size_t termLen, UnitT term>
AbstractSharedBuffer<UnitT, termLen, term>::AbstractSharedBuffer(const AbstractSharedBuffer<UnitT, termLen, term>& other)
		: d(other.d), size(other.size), capacity(other.capacity), isnull(other.isnull), readAliasDummy(other.readAliasDummy)
{
}


template <typename UnitT, size_t termLen, UnitT term>
AbstractSharedBuffer<UnitT, termLen, term>::AbstractSharedBuffer(const UnitT* data, size_t size)
		: d(data ? shared_ptr<UnitT>(new UnitT[size], default_delete<UnitT[]>()) : _EmptyPtr), size(size), capacity(size),
		  isnull(data == NULL)
{
	if (data)
		memcpy(d.get(), data, size*sizeof(UnitT));
}


template <typename UnitT, size_t termLen, UnitT term>
AbstractSharedBuffer<UnitT, termLen, term>::AbstractSharedBuffer(const UnitT* data, size_t size, size_t capacity)
		: d(data ? shared_ptr<UnitT>(new UnitT[capacity], default_delete<UnitT[]>()) : _EmptyPtr), size(size), capacity(capacity), isnull(data == NULL)
{
	if (data)
		memcpy(d.get(), data, size*sizeof(UnitT));
}


template <typename UnitT, size_t termLen, UnitT term>
template <typename Deleter>
AbstractSharedBuffer<UnitT, termLen, term>::AbstractSharedBuffer(UnitT* data, size_t size, size_t capacity, Deleter del)
		: d(data ? shared_ptr<UnitT>(data, del) : _EmptyPtr), size(size), capacity(capacity), isnull(data == NULL)
{
}


template <typename UnitT, size_t termLen, UnitT term>
AbstractSharedBuffer<UnitT, termLen, term>::AbstractSharedBuffer(UnitT* data, size_t size, size_t capacity, bool)
		: d(data ? shared_ptr<UnitT>(data, _NopDeleter<UnitT>()) : _EmptyPtr), size(size), capacity(capacity), isnull(data == NULL)
{
}


template <typename UnitT, size_t termLen, UnitT term>
AbstractSharedBuffer<UnitT, termLen, term>::AbstractSharedBuffer(const UnitT* data, size_t size, bool, bool)
		: d(data ? shared_ptr<UnitT>(const_cast<UnitT*>(data), _NopDeleter<UnitT>()) : _EmptyPtr), size(size), capacity(size),
		  isnull(data == NULL), readAliasDummy(d)
{
}


template <typename UnitT, size_t termLen, UnitT term>
size_t AbstractSharedBuffer<UnitT, termLen, term>::getGrownCapacity(size_t newCapacity)
{
	return GetNextPowerOfTwo(newCapacity);
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::resizeExactly(size_t newCapacity)
{
	UnitT* newD = new UnitT[newCapacity];
	size_t cpySize = min(newCapacity, size);
	memcpy(newD, d.get(), cpySize*sizeof(UnitT));
	d = shared_ptr<UnitT>(newD, default_delete<UnitT[]>());
	size = cpySize;
	capacity = newCapacity;
	isnull = false;
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::resizeExactlyWithOffset(size_t newCapacity, size_t srcOffset, size_t destOffset, size_t copyLen)
{
	UnitT* newD = new UnitT[newCapacity];
	size_t cpySize = min(copyLen, newCapacity-destOffset);
	memcpy(newD+destOffset, d.get()+srcOffset, cpySize*sizeof(UnitT));
	d = shared_ptr<UnitT>(newD, default_delete<UnitT[]>());
	size = cpySize+destOffset;
	capacity = newCapacity;
	isnull = false;
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::resizeExactlyWithSplit(size_t newCapacity, size_t srcOffset1, size_t destOffset1, size_t copyLen1,
			size_t srcOffset2, size_t destOffset2, size_t copyLen2)
{
	UnitT* newD = new UnitT[newCapacity];
	size_t cpySize1 = min(copyLen1, newCapacity-destOffset1);
	size_t cpySize2 = min(copyLen2, newCapacity-destOffset2);
	memcpy(newD+destOffset1, d.get()+srcOffset1, cpySize1*sizeof(UnitT));
	memcpy(newD+destOffset2, d.get()+srcOffset2, cpySize2*sizeof(UnitT));
	d = shared_ptr<UnitT>(newD, default_delete<UnitT[]>());
	size = max(cpySize1+destOffset1, cpySize2+destOffset2);
	capacity = newCapacity;
	isnull = false;
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::growExactly(size_t newCapacity)
{
	if (newCapacity > capacity)
		resizeExactly(newCapacity);
	else
		isnull = false;
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::growExactlyWithOffset(size_t newCapacity, size_t srcOffset, size_t destOffset, size_t copyLen)
{
	newCapacity = max(newCapacity, capacity);
	resizeExactlyWithOffset(newCapacity, srcOffset, destOffset, copyLen);
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::growExactlyWithSplit(size_t newCapacity, size_t srcOffset1, size_t destOffset1, size_t copyLen1,
		size_t srcOffset2, size_t destOffset2, size_t copyLen2)
{
	newCapacity = max(newCapacity, capacity);
	resizeExactlyWithSplit(newCapacity, srcOffset1, destOffset1, copyLen1, srcOffset2, destOffset2, copyLen2);
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::grow(size_t minCapacity)
{
	if (minCapacity > capacity)
		growExactly(getGrownCapacity(minCapacity));
	else
		isnull = false;
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::growWithOffset(size_t minCapacity, size_t srcOffset, size_t destOffset, size_t copyLen)
{
	growExactlyWithOffset(getGrownCapacity(minCapacity), srcOffset, destOffset, copyLen);
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::growWithSplit(size_t minCapacity, size_t srcOffset1, size_t destOffset1, size_t copyLen1,
		size_t srcOffset2, size_t destOffset2, size_t copyLen2)
{
	growExactlyWithSplit(getGrownCapacity(minCapacity), srcOffset1, destOffset1, copyLen1, srcOffset2, destOffset2, copyLen2);
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::ensureUniqueness()
{
	if (!d.unique())
		resizeExactly(capacity);
	else
		isnull = false;
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::append(const AbstractSharedBuffer<UnitT, termLen, term>& other)
{
	grow(size + other.size - termLen);
	memcpy(d.get() + size - termLen, other.d.get(), other.size*sizeof(UnitT));
	size += other.size - termLen;
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::append(UnitT u)
{
	grow(size + 1);

	if (termLen == 0) {
		d.get()[size] = u;
		size++;
	} else {
		d.get()[size-termLen] = u;
		d.get()[size] = term;
		size++;
	}
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::prepend(const AbstractSharedBuffer<UnitT, termLen, term>& other)
{
	growWithOffset(size + other.size - termLen, 0, other.size-termLen, size);
	memcpy(d.get(), other.d.get(), (other.size-termLen)*sizeof(UnitT));
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::prepend(UnitT u)
{
	growWithOffset(size + 1, 0, 1, size);
	d.get()[0] = u;
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::insert(size_t index, const AbstractSharedBuffer<UnitT, termLen, term>& other)
{
	growWithSplit(size + other.size - termLen, 0, 0, index, index, index+other.size-termLen, size-index);
	memcpy(d.get()+index, other.d.get(), (other.size-termLen)*sizeof(UnitT));
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::insert(size_t index, UnitT u)
{
	growWithSplit(size + 1, 0, 0, index, index, index+1, size-index);
	d.get()[index] = u;
}


template <typename UnitT, size_t termLen, UnitT term>
void AbstractSharedBuffer<UnitT, termLen, term>::assign(const AbstractSharedBuffer<UnitT, termLen, term>& other)
{
	if (this != &other) {
		d = other.d;
		size = other.size;
		capacity = other.capacity;
		isnull = other.isnull;
	}
}

#endif /* ABSTRACTSHAREDBUFFER_H_ */
