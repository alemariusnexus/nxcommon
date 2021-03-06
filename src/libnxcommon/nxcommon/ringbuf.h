#ifndef RINGBUF_H_
#define RINGBUF_H_

#include <nxcommon/config.h>
#include <stdlib.h>
#include <stdbool.h>


#ifndef __STDC_NO_ATOMICS__

// MSVC does not support stdatomic.h, which is required in c11atomic.h
// TODO: Find a way to implement it anyway.
#ifndef _MSC_VER
#define _NX_HAVE_ATOMICS
#endif

#endif

#ifdef _NX_HAVE_ATOMICS
#include "c11atomic.h"
#endif

#ifdef _NX_HAVE_ATOMICS
#define RINGBUF_IS_THREADSAFE
#endif



#ifdef __cplusplus
extern "C" {
#endif


typedef struct ringbuf_t
{
	void* buffer;
	size_t elem_size;
	size_t capacity;
	size_t read_offs;
	size_t write_offs;
#ifdef _NX_HAVE_ATOMICS
	atomic_size_t count;
#else
	size_t count;
#endif
} ringbuf_t;


void ringbuf_create(ringbuf_t* rbuf, void* buffer, size_t elem_size, size_t capacity);

size_t ringbuf_get_read_count(ringbuf_t* rbuf);
size_t ringbuf_get_write_count(ringbuf_t* rbuf);

bool ringbuf_is_empty(ringbuf_t* rbuf);
bool ringbuf_is_full(ringbuf_t* rbuf);

size_t ringbuf_write(ringbuf_t* rbuf, const void* elems, size_t num_elems);
size_t ringbuf_advance_write(ringbuf_t* rbuf, size_t num_elems);

size_t ringbuf_read(ringbuf_t* rbuf, void* elems, size_t num_elems);
size_t ringbuf_advance_read(ringbuf_t* rbuf, size_t num_elems);

size_t ringbuf_peek(ringbuf_t* rbuf, void* elems, size_t num_elems);

void ringbuf_clear(ringbuf_t* rbuf);


#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RINGBUF_H_ */
