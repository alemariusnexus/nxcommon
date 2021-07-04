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

#include <nxcommon/config.h>
#include <cstring>
#include <cstddef>
#include <cstdio>
#include <memory>
#include <algorithm>
#include <vector>
#include "util.h"

using std::shared_ptr;
using std::default_delete;
using std::min;
using std::max;




/*template <class ImplT, typename UnitT, UnitT term>
class AbstractSharedString;*/



/**	\brief Provides a general-purpose shared memory interface for strings and other stuff.
 *
 * 	The buffer managed by this class is an array of the template type UnitT. The buffer is shared between instances of
 * 	this class and employs copy-on-write (COW) functionality. Associated with the buffer are a size and a capacity. The
 * 	size is the number of UnitTs (__not__ the number of bytes!) that make up the actual stored data. The capacity is the
 * 	number of UnitTs (again: not bytes) that are allocated for the buffer. The capacity is usually greater than the size,
 * 	so that content can be appended to the buffer without having to reallocate it.
 *
 * 	The AbstractSharedBuffer can optionally be terminated using a selectable terminator. This is commonly used for
 * 	null-termination of strings. If termination is requested, this class makes sure that the first element after the
 * 	actual data is always the terminator. _The size and capacity values are measured excluding the terminator_, which
 * 	means that the number of UnitTs allocated for the buffer is actually capacity+1.
 *
 * 	This class is an abstract base class and can not be used on its own. It uses a form of static polymorphism by means
 * 	of the curiously recurring template pattern (CRTP), which means that the first template parameter is actually the
 * 	class that derives from this class. This way, many methods that are already implemented here actually work with
 * 	objects of the derived class. The derived class can always re-implement methods if the basic implementation doesn't
 * 	suit them.
 */
template <typename DerivedT, typename UnitT, bool terminated = false, UnitT term = 0>
class AbstractSharedBuffer
{
	template <class OImplT, typename OUnitT, OUnitT oterm>
	friend class AbstractSharedString;

	friend class ByteArray;

private:
	// The empty pointers for terminated and unterminated buffers are different
	struct _EmptyPtrTerm { static const shared_ptr<UnitT> value; };
	struct _EmptyPtrNonterm { static const shared_ptr<UnitT> value; };

	// The length of the terminator (in UnitTs)
	struct _TermLen0 { static const size_t value = 0; };
	struct _TermLen1 { static const size_t value = 1; };

	// Select one of the above structs, depending on whether the buffer is terminated.
	typedef typename std::conditional<terminated, _EmptyPtrTerm, _EmptyPtrNonterm>::type EmptyPtr;

protected:
	struct MallocFreeDeleter
	{
		void operator()(UnitT* v) { free(v); }
	};

public:
	typedef typename std::conditional<terminated, _TermLen1, _TermLen0>::type TermLen;

public:

	// The following will be disabled for a while because of a compiler bug in GCC, probably related to
	// the following report, which prevents the code from compiling:
	//
	//		https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61507
	//
	// It seems to be fixed in GCC 4.9.1, but for now I want to support compilers that are a bit older.
	// Also, this method has very limited use anyway, so it's not much of a loss.
#if 0
	// Unfortunately, it must be named differently. If it's named "join", it will often be matched instead
	// of the other join()'s because the parameter pack can be matched more directly (i.e. with fewer type
	// conversions) to incoming parameter values. One example is calling join() with int instead of size_t:
	//
	// 		join((const DerivedT&) a, (const DerivedT*) b, 10)
	//
	// This will actually call the variadic method overload instead of the the array-based one, because
	// the last parameter is an int (!= size_t), which more directly matches the variadic method.
	//
	// Parameter pack overloading is a nightmare...
	template <class... Args>
	static DerivedT joinv(const DerivedT& separator = DerivedT(), Args... args);
#endif

	static DerivedT join(const DerivedT& separator, const DerivedT* bufs, size_t numBufs);

public:
	/**	\brief Determine whether this is a null buffer.
	 *
	 * 	A null buffer is a special buffer that is constructed using the default constructor. Null buffers are different
	 * 	from empty buffers (i.e.: all null buffers are empty buffers, but not all empty buffers are null buffers). A null
	 * 	buffer becomes a non-null buffer when it is modified in any way.
	 *
	 * 	NOTE: Having a null buffer does __not__ mean that the array buffer is a NULL pointer. Instead, it is implemented
	 * 		as an empty buffer (an allocated buffer with size and capacity 0) and a special null flag.
	 *
	 * 	@return true if this is a null buffer, false otherwise.
	 * 	@see isEmpty()
	 */
	bool isNull() const { return isnull; }

	/**	\brief Determine whether the buffer is empty.
	 *
	 * 	An empty buffer is a buffer with size 0. This includes null buffers, but also empty non-null buffers.
	 *
	 * 	@return true if empty, false otherwise.
	 * 	@see isNull()
	 */
	bool isEmpty() const { return msize == 0; }


	/**	\brief Return the size (the number of valid UnitTs, _not_ the number of bytes) of this buffer.
	 *
	 * 	This does _not_ include the terminator (if any).
	 *
	 * 	@return Size of the buffer, in UnitTs.
	 */
	size_t getSize() const { return msize; }

	/**	\brief Return the size (the number of valid UnitTs, _not_ the number of bytes) of this buffer.
	 *
	 * 	This is a synonym for getSize()
	 *
	 * 	@return Size of the buffer, in UnitTs.
	 * 	@see getSize()
	 */
	size_t getLength() const { return msize; }

	/**	\brief Return the size (the number of valid UnitTs, _not_ the number of bytes) of this buffer.
	 *
	 * 	This is a synonym for getSize()
	 *
	 * 	@return Size of the buffer, in UnitTs.
	 * 	@see getSize()
	 */
	size_t size() const { return msize; }

	/**	\brief Return the size (the number of valid UnitTs, _not_ the number of bytes) of this buffer.
	 *
	 * 	This is a synonym for getSize()
	 *
	 * 	@return Size of the buffer, in UnitTs.
	 * 	@see getSize()
	 */
	size_t length() const { return msize; }


	/**	\brief Return the capacity of this buffer.
	 *
	 * 	This is the allocated size of the buffer, in UnitTs (_not_ in bytes). It includes uninitialized memory after
	 * 	the \link getSize() size \endlink of the buffer.
	 *
	 * 	@return The buffer capacity.
	 */
	size_t getCapacity() const { return mcapacity; }

	/**	\brief Return the capacity of this buffer.
	 *
	 * 	This is a synonym for getCapacity().
	 *
	 * 	@return The buffer capacity.
	 * 	@see getCapacity()
	 */
	size_t capacity() const { return mcapacity; }


	/**	\brief Reserve space for the internal buffer.
	 *
	 * 	This method will change the capacity of the buffer. It can be used to increase or decrease the capacity, but it will
	 * 	never truncate the contents of the buffer (i.e., it will not make the capacity lower than the size).
	 *
	 * 	@param minCapacity The new minimum capacity. Usually, this method will allocate more than the requested amount, but
	 * 		the parameter can serve as a rough suggestion. It will also never allocate less than requested.
	 */
	void reserve(size_t minCapacity);

	/**	\brief Reserve more space for the internal buffer.
	 *
	 * 	This method works like reserve(size_t), but will never shrink the capacity.
	 *
	 * 	@param minCapacity The new minimum capacity. If it's lower than the current capacity, this method does nothing.
	 * 	@see reserve(size_t)
	 */
	void grow(size_t minCapacity);

	/**	\brief Squeeze the buffer to be as small as possible.
	 *
	 * 	This will reallocate the internal buffer with a capacity equal to its size.
	 *
	 * 	@see realloc(size_t)
	 */
	void squeeze() { realloc(msize); }


	/**	\brief Change the size of the actual data.
	 *
	 * 	This method changes the size of the valid data region in the buffer. If the new size is greater than the old size,
	 * 	the additional space on the end of the buffer is uninitialized. If the new size is lower, the buffer will be truncated.
	 * 	The capacity is automatically increased if necessary. The terminator (if any) will be moved accordingly.
	 *
	 * 	@param newSize The new size of the valid buffer data.
	 */
	void resize(size_t newSize);


	/**	\brief Return a read-only pointer to the internal buffer.
	 *
	 * 	Note that unless the buffer was configured for read- or write-aliasing, the underlying buffer stays valid only for the
	 * 	lifetime of the AbstractSharedBuffer.
	 *
	 * 	@return A pointer to the internal buffer.
	 */
	const UnitT* get() const { return d.get(); }

	/**	\brief Return a read-write pointer to the internal buffer.
	 *
	 * 	Calling this method actually causes a copy operation if the buffer is shared, so if you only need read access, consider
	 * 	using get() instead, which is a lightweight operation.
	 *
	 * 	You can read and modify the buffer in any way you want, in the whole area denoted by the buffer's capacity. If you
	 * 	choose to add or remove data from the buffer, be sure to call resize(size_t) afterwards to set the buffer's new valid
	 * 	data size.
	 *
	 * 	Note that unless the buffer was configured for read- or write-aliasing, the underlying buffer stays valid only for the
	 * 	lifetime of the AbstractSharedBuffer.
	 *
	 *	@return A read-write pointer to the internal buffer.
	 */
	UnitT* mget() { ensureUniqueness(); return d.get(); }


	ptrdiff_t indexOf(UnitT u, size_t offset = 0) const;

	ptrdiff_t indexOf(const DerivedT& other, size_t offset = 0) const;


	/**	\brief Append another buffer to the end of this buffer.
	 *
	 * 	@param other The other buffer
	 * 	@return Reference to this buffer.
	 */
	DerivedT& append(const DerivedT& other);

	/**	\brief Append a single UnitT to the end of this buffer.
	 *
	 * 	@param u The UnitT to append.
	 * 	@return Reference to this buffer.
	 */
	DerivedT& append(UnitT u);


	/**	\brief Append another buffer to the beginning of this buffer.
	 *
	 * 	@param other The other buffer.
	 * 	@return Reference to this buffer.
	 */
	DerivedT& prepend(const DerivedT& other);

	/**	\brief Append a single UnitT to the beginning of this buffer.
	 *
	 * 	If you want to prepend multiple UnitTs, consider using prepend(const DerivedT& other), because this method will have
	 * 	to copy the entire buffer for each prepended UnitT.
	 *
	 * 	@param u The UnitT to prepend.
	 * 	@return Reference to this buffer.
	 */
	DerivedT& prepend(UnitT u);


	/**	\brief Insert another buffer somewhere in this buffer.
	 *
	 * 	@param index The index before which the buffer is inserted.
	 * 	@param other The inserted buffer.
	 * 	@return Reference to this buffer.
	 */
	DerivedT& insert(size_t index, const DerivedT& other);

	/**	\brief Insert a single UnitT somewhere in this buffer.
	 *
	 * 	@param index The index before which the buffer is inserted.
	 * 	@param u The UnitT to insert.
	 * 	@return Reference to this buffer.
	 */
	DerivedT& insert(size_t index, UnitT u);


	/**	\brief Return an std::vector containing the data.
	 *
	 * 	@return A copy of the data as an std::vector, excluding the terminator (if any).
	 */
	operator std::vector<UnitT>() const { return std::vector<UnitT>(d.get(), d.get()+msize); }


	/**	\brief Return the UnitT at the given index.
	 *
	 * 	There is no variant of the method returning a reference, because that would lend itself to accidentally copying
	 * 	the whole buffer (COW) although no write access is actually needed. If you need write acces, use mget().
	 *
	 * 	@param idx The index of the UnitT in the buffer.
	 * 	@return The UnitT at the given index.
	 */
	UnitT operator[](size_t idx) const { return d.get()[idx]; }


	/**	\brief Assignment operator.
	 *
	 * 	This is a lightweight operation due to data sharing.
	 *
	 * 	@param other New buffer value.
	 * 	@return Reference to this buffer.
	 */
	DerivedT& operator=(const DerivedT& other);

	/**	\brief Append another buffer to the end of this one.
	 *
	 *	This is a synonyme for append(const DerivedT&).
	 *
	 *	@param The buffer to append.
	 *	@return reference to this buffer.
	 */
	DerivedT& operator+=(const DerivedT& other) { append(other); return *static_cast<DerivedT*>(this); }

	DerivedT& operator+=(UnitT u) { append(u); return *static_cast<DerivedT*>(this); }

	bool operator==(const DerivedT& other) const { return DerivedT::compare(*static_cast<const DerivedT*>(this), other) == 0; }
	bool operator!=(const DerivedT& other) const { return DerivedT::compare(*static_cast<const DerivedT*>(this), other) != 0; }
	bool operator<(const DerivedT& other) const { return DerivedT::compare(*static_cast<const DerivedT*>(this), other) < 0; }
	bool operator<=(const DerivedT& other) const { return DerivedT::compare(*static_cast<const DerivedT*>(this), other) <= 0; }
	bool operator>(const DerivedT& other) const { return DerivedT::compare(*static_cast<const DerivedT*>(this), other) > 0; }
	bool operator>=(const DerivedT& other) const { return DerivedT::compare(*static_cast<const DerivedT*>(this), other) >= 0; }

protected:
	/**	\brief Construct a null buffer.
	 *
	 * 	A null buffer has size and capacity 0 and a special null flag. Note that the actual buffer pointer is _not_ NULL.
	 */
	AbstractSharedBuffer();

	/**	\brief Copy constructor.
	 *
	 * 	This is a lightweight operation due to data sharing.
	 *
	 * 	@param other The buffer to copy.
	 */
	AbstractSharedBuffer(const AbstractSharedBuffer<DerivedT, UnitT, terminated, term>& other);

	/**	\brief Construct a new buffer as a copy of the given buffer.
	 *
	 * 	@param data Buffer data.
	 * 	@param size Size of the buffer, in UnitTs.
	 * 	@see AbstractSharedBuffer(const UnitT*, size_t, size_t)
	 */
	AbstractSharedBuffer(const UnitT* data, size_t size);

	/**	\brief Construct a new buffer as a copy of the given buffer.
	 *
	 * 	This constructor can be used to give the buffer an initial capacity that is bigger than the size. The input buffer
	 * 	does not have to be bigger than the size parameter, additional storage is left uninitialized.
	 *
	 * 	@param data Buffer data.
	 * 	@param size Size of the buffer, in UnitTs.
	 * 	@param capacity Initial capacity. This may not be lower than the size.
	 */
	AbstractSharedBuffer(const UnitT* data, size_t size, size_t capacity);
	AbstractSharedBuffer(size_t capacity);


	/**	\brief Construct a new buffer by taking ownership of the given array.
	 *
	 * 	No copy of the buffer is made. Ownership is transferred to this object.
	 *
	 * 	@param data Buffer data. If this AbstractSharedBuffer is terminated, the buffer _must_ have storage allocated
	 * 		for capacity+1 UnitTs and the last UnitT must be the terminator.
	 * 	@param size The valid buffer data size.
	 * 	@param capacity The current capacity of the input buffer, _excluding_ the terminator, if any.
	 * 	@param del A deleter functor that is used to free the data buffer when it's no longer in use.
	 */
	template <typename Deleter>
	AbstractSharedBuffer(UnitT* data, size_t size, size_t capacity, Deleter del = default_delete<UnitT[]>());

	/**	\brief Construct a new buffer that write-aliases the given array.
	 *
	 *	Ownership of the buffer will _not_ be taken by this class. The buffer will be used for reading and writing and can
	 *	take part in data sharing. The caller must ensure that the input buffer remains valid for the lifetime of this
	 *	AbstractSharedBuffer.
	 *
	 *	__Caution__: If you make a copy of a write-aliasing AbstractSharedBuffer and then call any modifying method on either
	 *	the original or the copy, the actual modified buffer will _not_ be the original write-aliased input buffer, because
	 *	of the COW semantics. If you want to use this constructor to create an AbstractSharedBuffer that modifies an external
	 *	buffer, you should not attempt to make a copy of this AbstractSharedBuffer, unless you _really_ know what you're doing.
	 *
	 *	@param data The input buffer. Must remain valid for the lifetime of this AbstractSharedBuffer. If this
	 *		AbstractSharedBuffer is terminated, the buffer _must_ have storage allocated for capacity+1 UnitTs and the last
	 *		UnitT must be the terminator.
	 *	@param size The valid buffer data size.
	 *	@param capacity The current capacity of the input buffer, _excluding_ the terminator, if any.
	 */
	AbstractSharedBuffer(UnitT* data, size_t size, size_t capacity, bool);

	/**	\brief Construct a new buffer that read-aliases the given array.
	 *
	 * 	Ownership of the buffer will _not_ be taken by tis class. The buffer will only be used for read-only operations, and
	 * 	a copy is made on any write attempt. The caller must ensure that the input buffer remains valid for the lifetime of
	 * 	this AbstractSharedBuffer.
	 *
	 * 	\param data The input buffer. Must remain valid for the lifetime of this AbstractSharedBuffer. If this
	 *		AbstractSharedBuffer is terminated, the buffer _must_ have storage allocated for capacity+1 UnitTs and the last
	 *		UnitT must be the terminator.
	 *	\param size The valid buffer data size.
	 *	\param capacity The current capacity of the input buffer, _excluding_ the terminator, if any.
	 */
	AbstractSharedBuffer(const UnitT* data, size_t size, bool, bool);

	/**	\brief Constructs a new buffer that aliases the array behind an external shared_ptr, sharing its reference count.
	 *
	 * 	Behavior is similar to either the read-aliasing or write-aliasing constructor, depending on the value used for
	 * 	readAliasDummy. The advantage of using this constructor over either the read-aliasing or write-aliasing constructors
	 * 	is that it shares a reference count with the given shared_ptr, which means that the data will remain valid even if
	 * 	the shared_ptr passed in as an argument is later destroyed.
	 *
	 * 	\param sharedPtr The input buffer. The newly created AbstractSharedBuffer shares a reference count with it, so it is
	 * 		guaranteed to remain valid for as long as the new object desires.
	 * 	\param size The valid buffer data size.
	 * 	\param capacity The current capacity of the input buffer, _excluding_ the terminator, if any.
	 * 	\param readAliasDummy Set this to a copy of sharedPtr (the actual raw pointer does not matter, so shared_ptr's
	 * 		aliasing constructor could be used) for read-aliasing, or an empty shared_ptr for write-aliasing. See comment
	 * 		on readAliasDummy member variable for more info.
	 * 	\param isnull Whether the new buffer is a null buffer. Must be given explicitly because a null AbstractSharedBuffer
	 * 		actually uses a non-null pointer inside its shared_ptr.
	 */
	AbstractSharedBuffer(const shared_ptr<UnitT>& sharedPtr, size_t size, size_t capacity,
			const shared_ptr<UnitT>& readAliasDummy, bool isnull);

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

protected:
	static int compare(const UnitT* a, const UnitT* b, size_t size)
	{
		return memcmp(a, b, size*sizeof(UnitT));
	}

	static UnitT* find(UnitT* beg, UnitT* end, UnitT c)
	{
		while (beg != end) {
			if (*beg == c) {
				return beg;
			}
			beg++;
		}
		return end;
	}



	static int compare(const UnitT* a, const UnitT* b, size_t aLen, size_t bLen)
	{
		int res = DerivedT::compare(a, b, min(aLen, bLen));
		return (res != 0) ? res : (int) (aLen-bLen);
	}

	static int compare(const DerivedT& a, const DerivedT& b)
	{
		return DerivedT::compare(a.get(), b.get(), a.length(), b.length());
	}

	static UnitT* find(UnitT* beg, UnitT* end, const UnitT* data, size_t size)
	{
		if (size == 0) {
			return beg;
		}

		UnitT d = *data;
		data++;
		size--;

		UnitT* mbeg = beg;

		while (size_t(end-mbeg) > size  &&  (mbeg = DerivedT::find(mbeg, end, d)) != end) {
			mbeg++;
			if (DerivedT::compare(mbeg, data, size) == 0) {
				return mbeg-1;
			}
		}

		return end;
	}

private:
	static size_t getGrownCapacity(size_t newCapacity);

protected:
	shared_ptr<UnitT> d;
	size_t msize;
	size_t mcapacity;
	bool isnull;

private:
	// The sole purpose of this is to make read-aliasing work. Normally, this is an empty shared_ptr that
	// isn't used for anything. If the read-aliasing constructor is used however, it will contain a copy
	// if this->d, so that this->d.unique() == false and a copy will be made on the next write operation,
	// which is the desired behavior for read-aliasing (see ensureUniqueness()).
	// The actual pointer stored in here does not matter, so the aliasing constructor of shared_ptr could
	// be used.
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
		: d(data ? shared_ptr<UnitT>(data, NopDeleter<UnitT>()) : EmptyPtr::value), msize(size), mcapacity(capacity), isnull(data == NULL)
{
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::AbstractSharedBuffer(const UnitT* data, size_t size, bool, bool)
		: d(data ? shared_ptr<UnitT>(const_cast<UnitT*>(data), NopDeleter<UnitT>()) : EmptyPtr::value), msize(size), mcapacity(size),
		  isnull(data == NULL), readAliasDummy(d)
{
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::AbstractSharedBuffer(const shared_ptr<UnitT>& sharedPtr,
		size_t size, size_t capacity, const shared_ptr<UnitT>& readAliasDummy, bool isnull)
		: d(sharedPtr ? sharedPtr : EmptyPtr::value), msize(size), mcapacity(capacity), isnull(isnull),
		  readAliasDummy(readAliasDummy)
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
	memcpy(newD, d.get(), msize*sizeof(UnitT));
	// We don't copy the terminator in the above memcpy, because it might not be there yet. This situation can pop up temporarily
	// in internal methods of this class, and these methods might use realloc() to fix that.
	if (terminated) newD[msize] = term;
	d = shared_ptr<UnitT>(newD, default_delete<UnitT[]>());
	mcapacity = newCapacity;
	isnull = false;
	readAliasDummy.reset();
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::reallocWithOffset(size_t newCapacity, size_t srcOffset, size_t destOffset)
{
	UnitT* newD = new UnitT[newCapacity + TermLen::value];
	size_t cpySize = msize-srcOffset;
	memcpy(newD+destOffset, d.get()+srcOffset, cpySize*sizeof(UnitT));
	msize += destOffset-srcOffset;
	// We don't copy the terminator in the above memcpy, because it might not be there yet. This situation can pop up temporarily
	// in internal methods of this class, and these methods might use reallocWithOffset() to fix that.
	if (terminated) newD[msize] = term;
	d = shared_ptr<UnitT>(newD, default_delete<UnitT[]>());
	mcapacity = newCapacity;
	isnull = false;
	readAliasDummy.reset();
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::reallocWithSplit(size_t newCapacity,
		size_t srcOffset1, size_t destOffset1, size_t copyLen1,
		size_t srcOffset2, size_t destOffset2)
{
	UnitT* newD = new UnitT[newCapacity];
	size_t copyLen2 = msize-srcOffset2;
	memcpy(newD+destOffset1, d.get()+srcOffset1, copyLen1*sizeof(UnitT));
	memcpy(newD+destOffset2, d.get()+srcOffset2, copyLen2*sizeof(UnitT));
	msize = destOffset2+copyLen2;
	// We don't copy the terminator in the above memcpy, because it might not be there yet. This situation can pop up temporarily
	// in internal methods of this class, and these methods might use reallocWithSplit() to fix that.
	if (terminated) newD[msize] = term;
	d = shared_ptr<UnitT>(newD, default_delete<UnitT[]>());
	mcapacity = newCapacity;
	isnull = false;
	readAliasDummy.reset();
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
void AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::growExactly(size_t newCapacity)
{
	if (newCapacity > mcapacity) {
		realloc(newCapacity);
	} else {
		ensureUniqueness();
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
		realloc(getGrownCapacity(minCapacity));
	} else {
		ensureUniqueness();
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
ptrdiff_t AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::indexOf(UnitT u, size_t offset) const
{
	UnitT* end = d.get() + msize;
	UnitT* o = DerivedT::find(d.get() + offset, end, u);
	return o != end ? (o - d.get()) : -1;
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
ptrdiff_t AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::indexOf(const DerivedT& other, size_t offset) const
{
	UnitT* end = d.get() + msize;
	UnitT* o = DerivedT::find(d.get() + offset, end, other.get(), other.length());
	return o != end ? (o - d.get()) : -1;
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
DerivedT& AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::operator=(const DerivedT& other)
{
	if (this != &other) {
		d = other.d;
		msize = other.msize;
		mcapacity = other.mcapacity;
		isnull = other.isnull;
	}
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
size_t AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::resize()
{
	UnitT* data = d.get();
	for (msize = 0 ; msize < mcapacity  &&  data[msize] != term ; msize++);
	return msize;
}


template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
DerivedT AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::join(const DerivedT& separator,
		const DerivedT* bufs, size_t numBufs)
{
	DerivedT joined;

	for (size_t i = 0 ; i < numBufs ; i++) {
		if (i != 0) {
			joined.append(separator);
		}
		joined.append(bufs[i]);
	}

	return joined;
}


#if 0
template <typename DerivedT, typename UnitT, bool terminated, UnitT term>
template <class... Args>
DerivedT AbstractSharedBuffer<DerivedT, UnitT, terminated, term>::joinv(const DerivedT& separator, Args... args)
{
	DerivedT bufs[sizeof...(args)] = { args... };
	return join(separator, bufs, sizeof...(args));
}
#endif

#endif /* ABSTRACTSHAREDBUFFER_H_ */
