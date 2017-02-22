#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <nxcommon/config.h>
#include "util.h"
#include <cstring>
#include <algorithm>
#include <memory>
#include <functional>
#include <atomic>

using std::unique_ptr;





/**
 * Data structure implementing a ring buffer. A ring buffer implements a queue, where elements are
 * written to the end of the buffer and read from the start of the buffer, with both operations
 * wrapping around once they read the buffer's end. A ring buffer is backed by a fixed-size buffer,
 * so if the buffer is full, writing is not possible until another element is read.
 *
 * This class allows one reader in parallel with one writer at maximum. Behavior is UNDEFINED if
 * there are multiple parallel readers, multiple parallel writers, or both.
 */
template <typename T, typename SizeType = size_t>
class RingBuffer
{
public:
	typedef SizeType size_type;

public:
	// TODO: More constructors. Like, really...
	// TODO: Consider allowing copy-construction (but which semantics? deep copy?)

	/**
	 * Construct a null RingBuffer.
	 */
	RingBuffer();

	/**
	 * Move constructor. This is not thread-safe. Doing this while another thread is reading and/or
	 * writing either 'this' or 'other' leads to undefiend behavior!
	 */
	RingBuffer(RingBuffer&& other);

	RingBuffer(const RingBuffer&) = delete;

	/**
	 * Write-aliasing constructor (class does not free memory)
	 */
	RingBuffer(T* buf, size_type capacity);

	/**
	 * Ownership-taking constructor (class frees memory)
	 */
	template <typename Deleter>
	RingBuffer(T* buf, size_type capacity, Deleter del);

	/**
	 * Create a new buffer of given capacity, owned by the new object.
	 */
	RingBuffer(size_type capacity);

	bool isNull() const;

	bool isEmpty() const;
	bool isFull() const;

	size_type capacity() const { return mcapacity; }
	size_type getCapacity() const { return capacity(); }

	size_type readSize() const;
	size_type getReadSize() const { return readSize(); }

	size_type writeSize() const;
	size_type getWriteSize() const { return writeSize(); }

	size_type write(const T* data, size_type len);
	bool write(const T& elem) { return write(&elem, 1) == 1; }

	size_type advanceWrite(size_type len);

	size_type read(T* data, size_type len);
	T read() { T d; read(&d, 1); return d; }

	size_type advanceRead(size_type len);
	size_type skip(size_type len) { return advanceRead(len); }

	size_type peek(T* data, size_type len);
	T& peek();

	void clear();

	/**
	 * Move assignment. This is NOT thread-safe. Doing this while another thread is reading and/or
	 * writing either 'this' or 'other' leads to undefined behavior!
	 */
	RingBuffer& operator=(RingBuffer&& other);

	RingBuffer& operator=(const RingBuffer&) = delete;

private:
	unique_ptr<T[], std::function<void (T*)>> buf;
	size_type mcapacity;
	size_type readOffs;
	size_type writeOffs;
	std::atomic<size_type> mcount; // Used as a general memory ordering atomic using release/acquire
};






template <typename T, typename SizeType>
RingBuffer<T, SizeType>::RingBuffer()
		: buf(), mcapacity(0), readOffs(1), mcount(0)
{
	// NOTE: Other members need not be initialized. mcapacity and mcount are enough to make both
	// readSize() and writeSize() return 0, effectively making all modifying members of this
	// class no-ops. Setting readOffs to something invalid (> mcapacity) is necessary only for
	// implementing isNull(): To distinguish an actual null RingBuffer from e.g. a "normal" one
	// with capacity 0 or a null backend buffer. No idea if that's useful, but that's how it is.
	// buf should be set to null, because we might return a pointer to it at some point in time.
}


template <typename T, typename SizeType>
RingBuffer<T, SizeType>::RingBuffer(RingBuffer&& other)
		: buf(other.buf), mcapacity(other.mcapacity), readOffs(other.readOffs), writeOffs(other.writeOffs),
		  mcount(other.mcount.load(std::memory_order_release))
{
	// Set same values as default constructor would set
	//other.buf = nullptr; // Done automatically by move-constructor of unique_ptr<T[]>
	other.mcapacity = 0;
	other.readOffs = 1;
	other.mcount = 0;
}


template <typename T, typename SizeType>
RingBuffer<T, SizeType>::RingBuffer(T* buf, size_type capacity)
		: buf(buf, NopDeleter<T>()), mcapacity(capacity), readOffs(0), writeOffs(0), mcount(0)
{
}


template <typename T, typename SizeType>
template <typename Deleter>
RingBuffer<T, SizeType>::RingBuffer(T* buf, size_type capacity, Deleter del)
		: buf(buf, del), mcapacity(capacity), readOffs(0), writeOffs(0), mcount(0)
{

}


template <typename T, typename SizeType>
RingBuffer<T, SizeType>::RingBuffer(size_type capacity)
		: buf(new T[capacity], std::default_delete<T[]>()), mcapacity(capacity), readOffs(0), writeOffs(0), mcount(0)
{
}


template <typename T, typename SizeType>
bool RingBuffer<T, SizeType>::isNull() const
{
	// See note in default constructor
	return readOffs > mcapacity;
}


template <typename T, typename SizeType>
bool RingBuffer<T, SizeType>::isEmpty() const
{
	// "acquire" might not be needed when called from outside (but definitely when called from
	// write(), read(), etc., because we need to ensure that changes to the buffer are visible).
	// For now, we'll do it all the time.
	return mcount.load(std::memory_order_acquire) == 0;
}


template <typename T, typename SizeType>
bool RingBuffer<T, SizeType>::isFull() const
{
	// "acquire" might not be needed when called from outside (but definitely when called from
	// write(), read(), etc., because we need to ensure that changes to the buffer are visible).
	// For now, we'll do it all the time.
	return mcount.load(std::memory_order_acquire) == mcapacity;
}


template <typename T, typename SizeType>
typename RingBuffer<T, SizeType>::size_type RingBuffer<T, SizeType>::readSize() const
{
	// "acquire" might not be needed when called from outside (but definitely when called from
	// write(), read(), etc., because we need to ensure that changes to the buffer are visible).
	// For now, we'll do it all the time.
	return mcount.load(std::memory_order_acquire);
}


template <typename T, typename SizeType>
typename RingBuffer<T, SizeType>::size_type RingBuffer<T, SizeType>::writeSize() const
{
	// "acquire" might not be needed when called from outside (but definitely when called from
	// write(), read(), etc., because we need to ensure that changes to the buffer are visible).
	// For now, we'll do it all the time.
	return mcapacity - mcount.load(std::memory_order_acquire);
}


template <typename T, typename SizeType>
typename RingBuffer<T, SizeType>::size_type RingBuffer<T, SizeType>::write(const T* data, size_type len)
{
	// Acquires mcount, making sure all changes of previous write()/read() calls are visible
	len = std::min(len, writeSize());

	size_type len1 = std::min(len, (size_type) (mcapacity - writeOffs));
	size_type len2 = len - len1;

	memcpy(buf.get() + writeOffs, data, len1 * sizeof(T));
	memcpy(buf.get(), data+len1, len2 * sizeof(T));

	writeOffs = (writeOffs + len) % mcapacity;

	// Make sure another thread that reads mcount (in parallel read() or sequential write()) will
	// also see all changes above
	mcount.fetch_add(len, std::memory_order_release);

	return len;
}


template <typename T, typename SizeType>
typename RingBuffer<T, SizeType>::size_type RingBuffer<T, SizeType>::advanceWrite(size_type len)
{
	// Acquires mcount, making sure all changes of previous write()/read() calls are visible
	len = std::min(len, writeSize());

	writeOffs = (writeOffs + len) % mcapacity;

	// A sequential write()/advanceWrite() in another thread must be guaranteed to see writeOffs updated, too.
	// This is why we need "release" instead of "relaxed".
	mcount.fetch_add(len, std::memory_order_release);

	return len;
}


template <typename T, typename SizeType>
typename RingBuffer<T, SizeType>::size_type RingBuffer<T, SizeType>::read(T* data, size_type len)
{
	len = peek(data, len);

	readOffs = (readOffs + len) % mcapacity;

	// Make sure another thread that reads mcount (in sequential read() or parallel write()) will
	// also see all changes above
	mcount.fetch_sub(len, std::memory_order_release);

	return len;
}


template <typename T, typename SizeType>
typename RingBuffer<T, SizeType>::size_type RingBuffer<T, SizeType>::advanceRead(size_type len)
{
	len = std::min(len, readSize());

	readOffs = (readOffs + len) % mcapacity;

	// A sequential read()/advanceRead() in another thread must be guaranteed to see readOffs updated, too.
	// This is why we need "release" instead of "relaxed".
	mcount.fetch_sub(len, std::memory_order_release);

	return len;
}


template <typename T, typename SizeType>
typename RingBuffer<T, SizeType>::size_type RingBuffer<T, SizeType>::peek(T* data, size_type len)
{
	// Acquires mcount, making sure all changes of previous write()/read() calls are visible
	len = std::min(len, readSize());

	size_type len1 = std::min(len, (size_type) (mcapacity - readOffs));
	size_type len2 = len - len1;

	memcpy(data, buf.get() + readOffs, len1 * sizeof(T));
	memcpy(data + len1, buf.get(), len2 * sizeof(T));

	return len;
}


template <typename T, typename SizeType>
T& RingBuffer<T, SizeType>::peek()
{
	// Acquires mcount, making sure all changes of previous write()/read() calls are visible
	readSize();
	return buf.get()[readOffs];
}


template <typename T, typename SizeType>
void RingBuffer<T, SizeType>::clear()
{
	readOffs = 0;
	writeOffs = 0;

	// This method is not allowed in parallel with write() nor with read(), but other threads might call
	// one of them right after this method ends. In this case, we will ensure that the other thread sees
	// all changes above when reading mcount.
	mcount.store(0, std::memory_order_release);
}


template <typename T, typename SizeType>
RingBuffer<T, SizeType>& RingBuffer<T, SizeType>::operator=(RingBuffer&& other)
{
	buf = std::move(other.buf);
	mcapacity = other.mcapacity;
	readOffs = other.readOffs;
	writeOffs = other.writeOffs;
	mcount = other.mcount.load(std::memory_order_release);

	// Set same values as default constructor would set
	//other.buf = nullptr; // Done automatically by move-assignment of unique_ptr<T[]>
	other.mcapacity = 0;
	other.readOffs = 1;
	other.mcount = 0;

	return *this;
}


#endif /* RINGBUFFER_H_ */
