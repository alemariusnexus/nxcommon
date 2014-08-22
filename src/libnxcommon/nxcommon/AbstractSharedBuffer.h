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



/**	\brief Provides a general-purpose shared memory interface for strings and other stuff.
 *
 * 	This class manages an array of base units represented by the UnitT template type. It uses data sharing and
 * 	reference counting to provide fast copying and automatic memory management, is able to resize its content
 * 	on the fly if necessary.
 *
 * 	The buffer is shared between instances of this class, and copying this class will just copy the pointer to
 * 	this shared buffer. Copy-on-write (COW) is used to be able to modify the buffer. The buffer memory is
 * 	automatically freed when the last AbstractSharedBuffer referencing it is destroyed.
 *
 * 	This class can optionally make sure that the buffer is terminated
 */
template <typename DerivedT, typename UnitT, bool terminated = false, UnitT term = 0>
class AbstractSharedBuffer
{
private:
	//static const shared_ptr<UnitT> _EmptyPtr;

	struct _EmptyPtrTerm { static const shared_ptr<UnitT> value; };
	struct _EmptyPtrNonterm { static const shared_ptr<UnitT> value; };

	struct _TermLen0 { static const size_t value = 0; };
	struct _TermLen1 { static const size_t value = 1; };

	typedef typename std::conditional<terminated, _TermLen1, _TermLen0>::type TermLen;
	typedef typename std::conditional<terminated, _EmptyPtrTerm, _EmptyPtrNonterm>::type EmptyPtr;

public:
	bool isNull() const { return isnull; }
	bool isEmpty() const { return msize == 0; }

	size_t getSize() const { return msize; }
	size_t getLength() const { return msize; }
	size_t size() const { return msize; }
	size_t length() const { return msize; }

	size_t getCapacity() const { return mcapacity; }
	size_t capacity() const { return mcapacity; }

	void reserve(size_t minCapacity);
	void grow(size_t minCapacity);
	void squeeze() { realloc(msize); }

	void resize(size_t newSize);

	const UnitT* get() const { return d.get(); }
	UnitT* mget() { ensureUniqueness(); return d.get(); }

	DerivedT substr(size_t begin, size_t len) const;

	DerivedT& lower();

	DerivedT& append(const DerivedT& other);
	DerivedT& append(UnitT u);

	DerivedT& prepend(const DerivedT& other);
	DerivedT& prepend(UnitT u);

	DerivedT& insert(size_t index, const DerivedT& other);
	DerivedT& insert(size_t index, UnitT u);

protected:
	AbstractSharedBuffer();
	AbstractSharedBuffer(const AbstractSharedBuffer<DerivedT, UnitT, terminated, term>& other);
	AbstractSharedBuffer(const UnitT* data, size_t size);
	AbstractSharedBuffer(const UnitT* data, size_t size, size_t capacity);
	AbstractSharedBuffer(size_t capacity);

	template <typename Deleter>
	AbstractSharedBuffer(UnitT* data, size_t size, size_t capacity, Deleter del = default_delete<UnitT[]>());

	// Write-aliasing
	AbstractSharedBuffer(UnitT* data, size_t size, size_t capacity, bool);

	// Read-aliasing
	AbstractSharedBuffer(const UnitT* data, size_t size, bool, bool);

	void realloc(size_t newCapacity);
	void reallocWithOffset(size_t newCapacity, size_t srcOffset, size_t destOffset);
	void reallocWithSplit(size_t newCapacity, size_t srcOffset1, size_t destOffset1, size_t copyLen1,
			size_t srcOffset2, size_t destOffset2);

	void growExactly(size_t newCapacity);
	void growExactlyWithOffset(size_t newCapacity, size_t srcOffset, size_t destOffset);
	void growExactlyWithSplit(size_t newCapacity, size_t srcOffset1, size_t destOffset1, size_t copyLen1,
			size_t srcOffset2, size_t destOffset2);

	void growWithOffset(size_t minCapacity, size_t srcOffset, size_t destOffset);
	void growWithSplit(size_t minCapacity, size_t srcOffset1, size_t destOffset1, size_t copyLen1,
			size_t srcOffset2, size_t destOffset2);

	void ensureUniqueness();

	void assign(const AbstractSharedBuffer& other);

	size_t resize();

private:
	static size_t getGrownCapacity(size_t newCapacity);

protected:
	shared_ptr<UnitT> d;
	size_t msize;
	size_t mcapacity;
	bool isnull;

private:
	shared_ptr<UnitT> readAliasDummy;
};




template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
const shared_ptr<UnitT> AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::_EmptyPtrTerm::value = shared_ptr<UnitT>(new UnitT[1] {term}, default_delete<UnitT[]>());

template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
const shared_ptr<UnitT> AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::_EmptyPtrNonterm::value = shared_ptr<UnitT>(new UnitT[0] {}, default_delete<UnitT[]>());




template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::AbstractSharedBuffer()
		: d(EmptyPtr::value), msize(0), mcapacity(0), isnull(true)
{
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::AbstractSharedBuffer(const AbstractSharedBuffer<DerivedT, UnitT, terminated, term>& other)
		: d(other.d), msize(other.msize), mcapacity(other.mcapacity), isnull(other.isnull), readAliasDummy(other.readAliasDummy)
{
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::AbstractSharedBuffer(const UnitT* data, size_t size)
		: d(data ? shared_ptr<UnitT>(new UnitT[size + TermLen::value], default_delete<UnitT[]>()) : EmptyPtr::value),
		  msize(size), mcapacity(size), isnull(data == NULL)
{
	if (data) {
		memcpy(d.get(), data, size*sizeof(UnitT));

		if (terminated) {
			d.get()[size] = term;
		}
	}
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::AbstractSharedBuffer(const UnitT* data, size_t size, size_t capacity)
		: d(data ? shared_ptr<UnitT>(new UnitT[capacity + TermLen::value], default_delete<UnitT[]>()) : EmptyPtr::value),
		  msize(size), mcapacity(capacity), isnull(data == NULL)
{
	if (data) {
		memcpy(d.get(), data, size*sizeof(UnitT));

		if (terminated) {
			d.get()[size] = term;
		}
	}
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::AbstractSharedBuffer(size_t capacity)
		: d(shared_ptr<UnitT>(new UnitT[capacity + TermLen::value], default_delete<UnitT[]>())), msize(0), mcapacity(capacity),
		  isnull(false)
{
	if (terminated) {
		d.get()[0] = term;
	}
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
template <typename Deleter>
AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::AbstractSharedBuffer(UnitT* data, size_t size, size_t capacity, Deleter del)
		: d(data ? shared_ptr<UnitT>(data, del) : EmptyPtr::value), msize(size), mcapacity(capacity), isnull(data == NULL)
{
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::AbstractSharedBuffer(UnitT* data, size_t size, size_t capacity, bool)
		: d(data ? shared_ptr<UnitT>(data, _NopDeleter<UnitT>()) : EmptyPtr::value), msize(size), mcapacity(capacity), isnull(data == NULL)
{
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::AbstractSharedBuffer(const UnitT* data, size_t size, bool, bool)
		: d(data ? shared_ptr<UnitT>(const_cast<UnitT*>(data), _NopDeleter<UnitT>()) : EmptyPtr::value), msize(size), mcapacity(size),
		  isnull(data == NULL), readAliasDummy(d)
{
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
size_t AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::getGrownCapacity(size_t newCapacity)
{
	return GetNextPowerOfTwo(newCapacity);
}


// Assumption: newCapacity >= size
template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::realloc(size_t newCapacity)
{
	UnitT* newD = new UnitT[newCapacity + TermLen::value];
	// TODO: This doesn't copy the null terminator, at least when size is the minimum. When size is NOT the minimum, then
	// 		newCapacity would have to include the null terminator, which is not what a 'capacity' means in this class
	memcpy(newD, d.get(), (msize + TermLen::value)*sizeof(UnitT));
	d = shared_ptr<UnitT>(newD, default_delete<UnitT[]>());
	mcapacity = newCapacity;
	isnull = false;
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::reallocWithOffset(size_t newCapacity, size_t srcOffset, size_t destOffset)
{
	UnitT* newD = new UnitT[newCapacity + TermLen::value];
	size_t cpySize = msize-srcOffset + TermLen::value;
	memcpy(newD+destOffset, d.get()+srcOffset, cpySize*sizeof(UnitT));
	d = shared_ptr<UnitT>(newD, default_delete<UnitT[]>());
	msize += destOffset-srcOffset;
	mcapacity = newCapacity;
	isnull = false;
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::reallocWithSplit(size_t newCapacity,
		size_t srcOffset1, size_t destOffset1, size_t copyLen1,
		size_t srcOffset2, size_t destOffset2)
{
	UnitT* newD = new UnitT[newCapacity];
	size_t copyLen2 = msize-srcOffset2 + TermLen::value;
	memcpy(newD+destOffset1, d.get()+srcOffset1, copyLen1*sizeof(UnitT));
	memcpy(newD+destOffset2, d.get()+srcOffset2, copyLen2*sizeof(UnitT));
	d = shared_ptr<UnitT>(newD, default_delete<UnitT[]>());
	msize = destOffset2+copyLen2 - TermLen::value;
	mcapacity = newCapacity;
	isnull = false;
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::growExactly(size_t newCapacity)
{
	if (newCapacity > mcapacity) {
		realloc(newCapacity);
	} else {
		isnull = false;
	}
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::growExactlyWithOffset(size_t newCapacity, size_t srcOffset, size_t destOffset)
{
	newCapacity = max(newCapacity, mcapacity);
	reallocWithOffset(newCapacity, srcOffset, destOffset);
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::growExactlyWithSplit(size_t newCapacity, size_t srcOffset1, size_t destOffset1, size_t copyLen1,
		size_t srcOffset2, size_t destOffset2)
{
	newCapacity = max(newCapacity, mcapacity);
	reallocWithSplit(newCapacity, srcOffset1, destOffset1, copyLen1, srcOffset2, destOffset2);
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::reserve(size_t minCapacity)
{
	minCapacity = getGrownCapacity(minCapacity);

	if (minCapacity >= msize) {
		growExactly(minCapacity);
	}
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::grow(size_t minCapacity)
{
	if (minCapacity > mcapacity) {
		growExactly(getGrownCapacity(minCapacity));
	} else {
		isnull = false;
	}
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::resize(size_t newSize)
{
	grow(newSize);
	msize = newSize;

	if (terminated) {
		d.get()[newSize] = term;
	}
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::growWithOffset(size_t minCapacity, size_t srcOffset, size_t destOffset)
{
	growExactlyWithOffset(getGrownCapacity(minCapacity), srcOffset, destOffset);
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::growWithSplit(size_t minCapacity, size_t srcOffset1, size_t destOffset1, size_t copyLen1,
		size_t srcOffset2, size_t destOffset2)
{
	growExactlyWithSplit(getGrownCapacity(minCapacity), srcOffset1, destOffset1, copyLen1, srcOffset2, destOffset2);
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::ensureUniqueness()
{
	if (!d.unique()) {
		realloc(mcapacity);
	} else {
		isnull = false;
	}
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
DerivedT& AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::append(const DerivedT& other)
{
	grow(msize + other.msize);
	memcpy(d.get() + msize, other.d.get(), (other.msize + TermLen::value)*sizeof(UnitT));
	msize += other.msize;
	return *static_cast<DerivedT*>(this);
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
DerivedT& AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::append(UnitT u)
{
	grow(msize + 1);
	d.get()[msize] = u;
	msize++;

	if (terminated) {
		d.get()[msize] = term;
	}

	return *static_cast<DerivedT*>(this);
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
DerivedT& AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::prepend(const DerivedT& other)
{
	growWithOffset(msize + other.msize, 0, other.msize);
	memcpy(d.get(), other.d.get(), other.msize*sizeof(UnitT));
	return *static_cast<DerivedT*>(this);
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
DerivedT& AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::prepend(UnitT u)
{
	growWithOffset(msize + 1, 0, 1);
	d.get()[0] = u;
	return *static_cast<DerivedT*>(this);
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
DerivedT& AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::insert(size_t index, const DerivedT& other)
{
	growWithSplit(msize + other.msize, 0, 0, index, index, index+other.msize);
	memcpy(d.get()+index, other.d.get(), other.msize*sizeof(UnitT));
	return *static_cast<DerivedT*>(this);
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
DerivedT& AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::insert(size_t index, UnitT u)
{
	growWithSplit(msize + 1, 0, 0, index, index, index+1);
	d.get()[index] = u;
	return *static_cast<DerivedT*>(this);
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::assign(const AbstractSharedBuffer<DerivedT, UnitT, terminated, term>& other)
{
	if (this != &other) {
		d = other.d;
		msize = other.msize;
		mcapacity = other.mcapacity;
		isnull = other.isnull;
	}
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
DerivedT AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::substr(size_t begin, size_t len) const
{
	UnitT* sub = new UnitT[len + TermLen::value];
	memcpy(sub, d.get() + begin, len * sizeof(UnitT));
	if (terminated) sub[len] = term;
	return DerivedT::from(sub, len);
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
size_t AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::resize()
{
	UnitT* data = d.get();
	for (msize = 0 ; msize < mcapacity  &&  data[msize] != term ; msize++);
	return msize;
}

#endif /* ABSTRACTSHAREDBUFFER_H_ */
