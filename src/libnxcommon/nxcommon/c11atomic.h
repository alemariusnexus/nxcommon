#pragma once


#ifdef __cplusplus

// stdatomic.h does not compile under C++ (no, 'extern "C"' does not help). See:
// 		http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0943r1.html
// Note that this relies on C11's _Atomic and C++'s std::atomic to be binary compatible, which
// is NOT guaranteed by the standard.
// TODO: Find a better way to do this

#include <atomic>
#define _Atomic(T) std::atomic<T>

using std::memory_order;
using std::memory_order_relaxed;
using std::memory_order_consume;
using std::memory_order_acquire;
using std::memory_order_release;
using std::memory_order_acq_rel;
using std::memory_order_seq_cst;

using std::atomic_flag;

using std::atomic_bool;
using std::atomic_char;
using std::atomic_schar;
using std::atomic_uchar;
using std::atomic_short;
using std::atomic_ushort;
using std::atomic_int;
using std::atomic_uint;
using std::atomic_long;
using std::atomic_ulong;
using std::atomic_llong;
using std::atomic_ullong;
using std::atomic_char16_t;
using std::atomic_char32_t;
using std::atomic_wchar_t;
using std::atomic_int8_t;
using std::atomic_uint8_t;
using std::atomic_int16_t;
using std::atomic_uint16_t;
using std::atomic_int32_t;
using std::atomic_uint32_t;
using std::atomic_int64_t;
using std::atomic_uint64_t;
using std::atomic_int_least8_t;
using std::atomic_uint_least8_t;
using std::atomic_int_least16_t;
using std::atomic_uint_least16_t;
using std::atomic_int_least32_t;
using std::atomic_uint_least32_t;
using std::atomic_int_least64_t;
using std::atomic_uint_least64_t;
using std::atomic_int_fast8_t;
using std::atomic_uint_fast8_t;
using std::atomic_int_fast16_t;
using std::atomic_uint_fast16_t;
using std::atomic_int_fast32_t;
using std::atomic_uint_fast32_t;
using std::atomic_int_fast64_t;
using std::atomic_uint_fast64_t;
using std::atomic_intptr_t;
using std::atomic_uintptr_t;
using std::atomic_size_t;
using std::atomic_ptrdiff_t;
using std::atomic_intmax_t;
using std::atomic_uintmax_t;

#else

#include <stdatomic.h>

#endif
