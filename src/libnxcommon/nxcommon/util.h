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

#ifndef NXCOMMON_UTIL_H_
#define NXCOMMON_UTIL_H_

#include <nxcommon/config.h>
#include <cstdlib>
#include <climits>


#define PS() uint64_t psS = GetTickcount();
#define PE(n) uint64_t psE = GetTickcount(); printf("%s took %d\n", (n), (int) (psE-psS));


//#ifdef __GNUC__
#if defined(__GNUC__)  &&  (__GNUC__ > 4  ||  (__GNUC__ == 4  &&  __GNUC_MINOR__ >= 3))

#define SwapEndianness16(v) ((uint16_t) ((((v) >> 8) & 0xFF)  |  (((v) << 8) & 0xFF00)))
#define SwapEndianness32(v) (__builtin_bswap32((v)))
#define SwapEndianness64(v) (__builtin_bswap64((v)))

#elif defined(_MSC_VER)

#define SwapEndianness16(v) (_byteswap_ushort((v)))
#define SwapEndianness32(v) (_byteswap_ulong((v)))
#define SwapEndianness64(v) (_byteswap_uint64((v)))

#else

#define SwapEndianness16(v) ((uint16_t) ((((v) >> 8) & 0xFF)  |  (((v) << 8) & 0xFF00)))

#define SwapEndianness32(v) ((uint32_t) ( \
		(((v) >> 24) & 0xFF) \
		| (((v) >> 8) & 0xFF00) \
		| (((v) << 8) & 0xFF0000) \
		| (((v) << 24) & 0xFF000000)))

#define SwapEndianness64(v) ((uint64_t) ( \
		(((v) >> 56) & 0xFFULL) \
		| (((v) >> 40) & 0xFF00ULL) \
		| (((v) >> 24) & 0xFF0000ULL) \
		| (((v) >> 8) & 0xFF000000ULL) \
		| (((v) << 8) & 0xFF00000000ULL) \
		| (((v) << 24) & 0xFF0000000000ULL) \
		| (((v) << 40) & 0xFF000000000000ULL) \
		| (((v) << 56) & 0xFF00000000000000ULL)))

#endif

inline float SwapEndiannessF32(float val)
{
	uint32_t uswp = SwapEndianness32(*((uint32_t*) &val));
	return *((float*) &uswp);
}


inline double SwapEndiannessF64(double val)
{
	uint64_t uswp = SwapEndianness64(*((uint64_t*) &val));
	return *((double*) &uswp);
}


template <typename T>
inline T SwapEndianness(T val);

template <> inline uint8_t SwapEndianness(uint8_t val) { return val; }
template <> inline int8_t SwapEndianness(int8_t val) { return val; }
template <> inline uint16_t SwapEndianness(uint16_t val) { return SwapEndianness16(val); }
template <> inline int16_t SwapEndianness(int16_t val) { return SwapEndianness16(val); }
template <> inline uint32_t SwapEndianness(uint32_t val) { return SwapEndianness32(val); }
template <> inline int32_t SwapEndianness(int32_t val) { return SwapEndianness32(val); }
template <> inline uint64_t SwapEndianness(uint64_t val) { return SwapEndianness64(val); }
template <> inline int64_t SwapEndianness(int64_t val) { return SwapEndianness64(val); }
template <> inline float SwapEndianness(float val) { return SwapEndiannessF32(val); }
template <> inline double SwapEndianness(double val) { return SwapEndiannessF64(val); }


#ifdef NXCOMMON_LITTLE_ENDIAN

#define ToLittleEndian16(v) (v)
#define ToLittleEndian32(v) (v)
#define ToLittleEndian64(v) (v)
#define ToLittleEndianF32(v) (v)
#define ToLittleEndianF64(v) (v)

#define ToBigEndian16(v) (SwapEndianness16((v)))
#define ToBigEndian32(v) (SwapEndianness32((v)))
#define ToBigEndian64(v) (SwapEndianness64((v)))
#define ToBigEndianF32(v) (SwapEndiannessF32((v)))
#define ToBigEndianF64(v) (SwapEndiannessF64((v)))

#define FromLittleEndian16(v) (v)
#define FromLittleEndian32(v) (v)
#define FromLittleEndian64(v) (v)
#define FromLittleEndianF32(v) (v)
#define FromLittleEndianF64(v) (v)

#define FromBigEndian16(v) (SwapEndianness16((v)))
#define FromBigEndian32(v) (SwapEndianness32((v)))
#define FromBigEndian64(v) (SwapEndianness64((v)))
#define FromBigEndianF32(v) (SwapEndiannessF32((v)))
#define FromBigEndianF64(v) (SwapEndiannessF64((v)))

template <typename T> inline T ToLittleEndian(T val) { return val; }
template <typename T> inline T ToBigEndian(T val) { return SwapEndianness<T>(val); }
template <typename T> inline T FromLittleEndian(T val) { return val; }
template <typename T> inline T FromBigEndian(T val) { return SwapEndianness<T>(val); }

#else

#define ToLittleEndian16(v) (SwapEndianness16((v)))
#define ToLittleEndian32(v) (SwapEndianness32((v)))
#define ToLittleEndian64(v) (SwapEndianness64((v)))
#define ToLittleEndianF32(v) (SwapEndiannessF32((v)))
#define ToLittleEndianF64(v) (SwapEndiannessF64((v)))

#define ToBigEndian16(v) (v)
#define ToBigEndian32(v) (v)
#define ToBigEndian64(v) (v)
#define ToBigEndianF32(v) (v)
#define ToBigEndianF64(v) (v)

#define FromLittleEndian16(v) (SwapEndianness16((v)))
#define FromLittleEndian32(v) (SwapEndianness32((v)))
#define FromLittleEndian64(v) (SwapEndianness64((v)))
#define FromLittleEndianF32(v) (SwapEndiannessF32((v)))
#define FromLittleEndianF64(v) (SwapEndiannessF64((v)))

#define FromBigEndian16(v) (v)
#define FromBigEndian32(v) (v)
#define FromBigEndian64(v) (v)
#define FromBigEndianF32(v) (v)
#define FromBigEndianF64(v) (v)

template <typename T> inline T ToLittleEndian(T val) { return SwapEndianness<T>(val); }
template <typename T> inline T ToBigEndian(T val) { return val; }
template <typename T> inline T FromLittleEndian(T val) { return SwapEndianness<T>(val); }
template <typename T> inline T FromBigEndian(T val) { return val; }

#endif



#if UINT8_MAX != 0xFFU

// Taken from stdint.h in glibc, with modifications.
// Define these macros here temporarily, because some crappy systems (in particular: Android's bionic libc) define
// these values with a type cast in them, so they are not usable in preprocessor #if checks.

#define _NX_INT8_MAX		(127)
#define _NX_INT16_MAX		(32767)
#define _NX_INT32_MAX		(2147483647)
#define _NX_INT64_MAX		(9223372036854775807ULL)

#define _NX_UINT8_MAX		(255)
#define _NX_UINT16_MAX		(65535)
#define _NX_UINT32_MAX		(4294967295U)
#define _NX_UINT64_MAX		(18446744073709551615ULL)

#else

#define _NX_INT8_MAX	INT8_MAX
#define _NX_INT16_MAX	INT16_MAX
#define _NX_INT32_MAX	INT32_MAX
#define _NX_INT64_MAX	INT64_MAX

#define _NX_UINT8_MAX	UINT8_MAX
#define _NX_UINT16_MAX	UINT16_MAX
#define _NX_UINT32_MAX	UINT32_MAX
#define _NX_UINT64_MAX	UINT64_MAX

#endif


// ********** DATA TYPE SIZES **********

// (unsigned short)

#if USHRT_MAX == _NX_UINT8_MAX
#define USHORT_SIZE 1
#elif USHRT_MAX == _NX_UINT16_MAX
#define USHORT_SIZE 2
#elif USHRT_MAX == _NX_UINT32_MAX
#define USHORT_SIZE 4
#elif USHRT_MAX == _NX_UINT64_MAX
#define USHORT_SIZE 8
#else
#error USHORT_SIZE could not be determined!
#endif

#if SHRT_MAX == _NX_INT8_MAX
#define SHORT_SIZE 1
#elif SHRT_MAX == _NX_INT16_MAX
#define SHORT_SIZE 2
#elif SHRT_MAX == _NX_INT32_MAX
#define SHORT_SIZE 4
#elif SHRT_MAX == _NX_INT64_MAX
#define SHORT_SIZE 8
#else
#error SHORT_SIZE could not be determined!
#endif


// (unsigned) int

#if UINT_MAX == _NX_UINT8_MAX
#define UINT_SIZE 1
#elif UINT_MAX == _NX_UINT16_MAX
#define UINT_SIZE 2
#elif UINT_MAX == _NX_UINT32_MAX
#define UINT_SIZE 4
#elif UINT_MAX == _NX_UINT64_MAX
#define UINT_SIZE 8
#else
#error UINT_SIZE could not be determined!
#endif

#if INT_MAX == _NX_INT8_MAX
#define INT_SIZE 1
#elif INT_MAX == _NX_INT16_MAX
#define INT_SIZE 2
#elif INT_MAX == _NX_INT32_MAX
#define INT_SIZE 4
#elif INT_MAX == _NX_INT64_MAX
#define INT_SIZE 8
#else
#error INT_SIZE could not be determined!
#endif


// (unsigned) long

#if ULONG_MAX == _NX_UINT8_MAX
#define ULONG_SIZE 1
#elif ULONG_MAX == _NX_UINT16_MAX
#define ULONG_SIZE 2
#elif ULONG_MAX == _NX_UINT32_MAX
#define ULONG_SIZE 4
#elif ULONG_MAX == _NX_UINT64_MAX
#define ULONG_SIZE 8
#else
#error ULONG_SIZE could not be determined!
#endif

#if LONG_MAX == _NX_INT8_MAX
#define LONG_SIZE 1
#elif LONG_MAX == _NX_INT16_MAX
#define LONG_SIZE 2
#elif LONG_MAX == _NX_INT32_MAX
#define LONG_SIZE 4
#elif LONG_MAX == _NX_INT64_MAX
#define LONG_SIZE 8
#else
#error LONG_SIZE could not be determined!
#endif


// (unsigned) long long

#ifdef LLONG_MAX

#if ULLONG_MAX == _NX_UINT8_MAX
#define ULONGLONG_SIZE 1
#elif ULLONG_MAX == _NX_UINT16_MAX
#define ULONGLONG_SIZE 2
#elif ULLONG_MAX == _NX_UINT32_MAX
#define ULONGLONG_SIZE 4
#elif ULLONG_MAX == _NX_UINT64_MAX
#define ULONGLONG_SIZE 8
#else
#error ULONGLONG_SIZE could not be determined!
#endif

#if LLONG_MAX == _NX_INT8_MAX
#define LONGLONG_SIZE 1
#elif LLONG_MAX == _NX_INT16_MAX
#define LONGLONG_SIZE 2
#elif LLONG_MAX == _NX_INT32_MAX
#define LONGLONG_SIZE 4
#elif LLONG_MAX == _NX_INT64_MAX
#define LONGLONG_SIZE 8
#else
#error LONGLONG_SIZE could not be determined!
#endif

#endif




// Zero-length arrays are illegal, and MSVC actually throws an error for them.
#ifdef NXCOMMON_LITTLE_ENDIAN
#define _DECLNUM(n,t) struct { t n; char _p_##n[sizeof(uint64_t)-sizeof(t) + 1]; }
#else
#define _DECLNUM(n,t) struct { char _p_##n[sizeof(uint64_t)-sizeof(t) + 1]; t n; }
#endif


/*union VariantNum {
	_DECLNUM(ui64, uint64_t);
	_DECLNUM(i64, int64_t);

	_DECLNUM(ui32, uint32_t);
	_DECLNUM(i32, int32_t);

	_DECLNUM(ui16, uint16_t);
	_DECLNUM(i16, int16_t);

	_DECLNUM(ui8, uint8_t);
	_DECLNUM(i8, int8_t);

	_DECLNUM(f, float);
	_DECLNUM(d, double);
};*/


class VariantNum
{
public:
	VariantNum() : ui64(0), i64(0), i32(0), i16(0), i8(0), f(0.0f), d(0.0) {}
	VariantNum(const VariantNum& other)
			: ui64(other.ui64), i64(other.i64), i32(other.i32), i16(other.i16), i8(other.i8), f(other.f), d(other.d) {}

	void reset() { setUInt64(0); }

	void setFromUInt() { f = (float) ui64; d = (double) ui64; }
	void setFromInt8() { i16 = i8; i32 = i8; i64 = i8; ui64 = i8; f = i8; d = i8; }
	void setFromInt16() { i8 = (int8_t) i16; i32 = i16; i64 = i16; ui64 = i16; f = i16; d = i16; }
	void setFromInt32() { i8 = (int8_t) i32; i16 = (int16_t) i32; i64 = i32; ui64 = i32; f = (float) i32; d = i32; }
	void setFromInt64() { i8 = (int8_t) i64; i16 = (int16_t) i64; i32 = (int32_t) i64; ui64 = i64; f = (float) i64; d = (double) i64; }
	void setFromFloat() { i64 = (int64_t) f; i32 = (int32_t) i64; i16 = (int16_t) i64; i8 = (int8_t) i64; ui64 = (uint64_t) f; d = f; }
	void setFromDouble() { i64 = (int64_t) d; i32 = (int32_t) i64; i16 = (int16_t) i64; i8 = (int8_t) i64; ui64 = (uint64_t) d; f = (float) d; }

	void setUInt8(uint8_t val) { setUInt64(val); }
	void setUInt16(uint16_t val) { setUInt64(val); }
	void setUInt32(uint32_t val) { setUInt64(val); }
	void setUInt64(uint64_t val) { ui64 = val; setFromUInt(); }
	void setInt8(int8_t val) { i8 = val; setFromInt8(); }
	void setInt16(int16_t val) { i16 = val; setFromInt16(); }
	void setInt32(int32_t val) { i32 = val; setFromInt32(); }
	void setInt64(int64_t val) { i64 = val; setFromInt64(); }
	void setFloat(float val) { f = val; setFromFloat(); }
	void setDouble(double val) { d = val; setFromDouble(); }

public:
	union
	{
		_DECLNUM(ui64, uint64_t);
		_DECLNUM(ui32, uint32_t);
		_DECLNUM(ui16, uint16_t);
		_DECLNUM(ui8, uint8_t);
	};

	_DECLNUM(i64, int64_t);
	_DECLNUM(i32, int32_t);
	_DECLNUM(i16, int16_t);
	_DECLNUM(i8, int8_t);

	_DECLNUM(f, float);
	_DECLNUM(d, double);
};


/**
 * A delete functor that does nothing (i.e. does not delete its argument).
 */
template <typename T>
class NopDeleter
{
public:
	NopDeleter() {}
	void operator()(T*) const {}
};






uint64_t GetTickcount();
uint64_t GetTickcountMicroseconds();

#ifdef _POSIX_VERSION
uint64_t GetEpochTickcount();
#endif

float RandomFloat(float min, float max);

bool RandomBool();

bool IsTimeBetween(int8_t timeH, int8_t timeM, int8_t startH, int8_t startM, int8_t endH, int8_t endM);

inline unsigned int GetNextPowerOfTwo(unsigned int v)
{
	v--;
	for (unsigned int i = 1 ; i < sizeof(unsigned int) * 8 ; i <<= 1)
		v |= v >> i;
	return v+1;
}

inline unsigned int RoundToMultiple(unsigned int val, unsigned int multiple)
{
	if (multiple == 0)
		return val;

	unsigned int rem = val % multiple;

	if (rem == 0)
		return val;

	return val + multiple - rem;
}

void SleepMilliseconds(unsigned int time);


#endif /* NXCOMMON_UTIL_H_ */
