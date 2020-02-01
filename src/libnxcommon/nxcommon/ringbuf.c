#include "ringbuf.h"
#include <stdio.h>
#include <string.h>


inline size_t min_size(size_t a, size_t b) { return a < b ? a : b; }



#ifdef _NX_HAVE_ATOMICS
#define _size_atomic_load(s) atomic_load_explicit((s), memory_order_acquire)
#define _size_atomic_fetch_add(s,v) atomic_fetch_add_explicit((s), (v), memory_order_release)
#define _size_atomic_fetch_sub(s,v) atomic_fetch_sub_explicit((s), (v), memory_order_release)
#define _size_atomic_store(s,v) atomic_store_explicit((s), (v), memory_order_release)
#else
#define _size_atomic_load(s) (*(s))
#define _size_atomic_fetch_add(s,v) ((*(s)) += (v))
#define _size_atomic_fetch_sub(s,v) ((*(s)) -= (v))
#define _size_atomic_store(s,v) ((*(s)) = v)
#endif



void ringbuf_create(ringbuf_t* rbuf, void* buffer, size_t elem_size, size_t capacity)
{
	rbuf->buffer = buffer;
	rbuf->elem_size = elem_size;
	rbuf->capacity = capacity;
	rbuf->read_offs = 0;
	rbuf->write_offs = 0;
	rbuf->count = 0;
}


size_t ringbuf_get_read_count(ringbuf_t* rbuf)
{
	return _size_atomic_load(&rbuf->count);
}


size_t ringbuf_get_write_count(ringbuf_t* rbuf)
{
	return rbuf->capacity - _size_atomic_load(&rbuf->count);
}


bool ringbuf_is_empty(ringbuf_t* rbuf)
{
	return _size_atomic_load(&rbuf->count) == 0;
}


bool ringbuf_is_full(ringbuf_t* rbuf)
{
	return _size_atomic_load(&rbuf->count) == rbuf->capacity;
}


size_t ringbuf_write(ringbuf_t* rbuf, const void* elems, size_t num_elems)
{
	num_elems = min_size(num_elems, ringbuf_get_write_count(rbuf));

	size_t len1 = min_size(num_elems, rbuf->capacity - rbuf->write_offs);
	size_t len2 = num_elems - len1;

	memcpy(((char*) rbuf->buffer) + rbuf->write_offs*rbuf->elem_size, elems, len1*rbuf->elem_size);
	memcpy(rbuf->buffer, ((char*) elems) + len1*rbuf->elem_size, len2*rbuf->elem_size);

	rbuf->write_offs = (rbuf->write_offs + num_elems) % rbuf->capacity;
	_size_atomic_fetch_add(&rbuf->count, num_elems);

	return num_elems;
}


size_t ringbuf_advance_write(ringbuf_t* rbuf, size_t num_elems)
{
	num_elems = min_size(num_elems, ringbuf_get_write_count(rbuf));

	rbuf->write_offs = (rbuf->write_offs + num_elems) % rbuf->capacity;
	_size_atomic_fetch_add(&rbuf->count, num_elems);

	return num_elems;
}


size_t ringbuf_read(ringbuf_t* rbuf, void* elems, size_t num_elems)
{
	num_elems = ringbuf_peek(rbuf, elems, num_elems);

	rbuf->read_offs = (rbuf->read_offs + num_elems) % rbuf->capacity;
	_size_atomic_fetch_sub(&rbuf->count, num_elems);

	return num_elems;
}


size_t ringbuf_advance_read(ringbuf_t* rbuf, size_t num_elems)
{
	num_elems = min_size(num_elems, ringbuf_get_read_count(rbuf));

	rbuf->read_offs = (rbuf->read_offs + num_elems) % rbuf->capacity;
	_size_atomic_fetch_sub(&rbuf->count, num_elems);

	return num_elems;
}


size_t ringbuf_peek(ringbuf_t* rbuf, void* elems, size_t num_elems)
{
	num_elems = min_size(num_elems, ringbuf_get_read_count(rbuf));

	size_t len1 = min_size(num_elems, rbuf->capacity - rbuf->read_offs);
	size_t len2 = num_elems - len1;

	memcpy(elems, ((char*) rbuf->buffer) + rbuf->read_offs*rbuf->elem_size, len1*rbuf->elem_size);
	memcpy(((char*) elems) + len1*rbuf->elem_size, rbuf->buffer, len2*rbuf->elem_size);

	return num_elems;
}


void ringbuf_clear(ringbuf_t* rbuf)
{
	rbuf->read_offs = 0;
	rbuf->write_offs = 0;
	_size_atomic_store(&rbuf->count, 0);
}



