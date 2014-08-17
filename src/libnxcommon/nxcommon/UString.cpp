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

#include "UString.h"






template <class IntT>
UChar* _FormatUIntUTF16(UChar buf[64], IntT val)
{
	UChar* beg = buf + 64 - 1;
	*beg = 0x0000;
	do {
		*--beg = (val % 10) + u'0';
	} while ((val /= 10)  !=  0);
	return beg;
}


template <class IntT>
UChar* _FormatIntUTF16(UChar buf[64], IntT val)
{
	bool neg = (val < 0);
	if (neg) {
		val = -val;
	}
	UChar* beg = buf + 64 - 1;
	*beg = 0x0000;
	do {
		*--beg = (val % 10) + u'0';
	} while ((val /= 10)  !=  0);
	if (neg) {
		*--beg = u'-';
	}
	return beg;
}





UString UString::fromUTF8(const ByteArray& utf8)
{
	// I believe the upper limit for the needed space in UTF-8 -> UTF-16 conversion is twice the length of
	// the UTF-8 string. This is the worst case, when all code points in utf8 are encoded with a single byte
	// only. In this case, they occupy exactly two bytes in UTF-16. For more than one byte in utf8, double
	// would be 4 bytes or more, which is the maximum width of a code point in UTF-16, so the factor two
	// works in all cases.

	int32_t len;
	UErrorCode errcode = U_ZERO_ERROR;
	UChar* utf16 = new UChar[utf8.length()+1];
	u_strFromUTF8(utf16, utf8.length()+1, &len, utf8.get(), utf8.length(), &errcode);
	return UString::from(utf16, len, utf8.length()+1);
}


UString UString::fromASCII(const CString& ascii)
{
	size_t len = ascii.length();
	UChar* utf16 = new UChar[len+1];
	const char* asciiStr = ascii.get();

	// ASCII is the beginning of the Unicode BMP, and the BMP is encoded directly through code point values in
	// UTF-16, so we just 'widen' our ASCII data.
	for (size_t i = 0 ; i < len ; i++) {
		utf16[i] = (UChar) asciiStr[i];
	}

	return UString::from(utf16, len);
}


UString& UString::lower(const CString& locale)
{
	UString cpy(*this);

	ensureUniqueness();
	UErrorCode errcode = U_ZERO_ERROR;

	int32_t len = u_strToLower(d.get(), capacity, d.get(), size-1, locale.get(), &errcode);

	if (len >= size) {
		grow(len+1);
		errcode = U_ZERO_ERROR;
		u_strToLower(d.get(), capacity, cpy.d.get(), cpy.size-1, locale.get(), &errcode);
	}

	return *this;
}


UString& UString::upper(const CString& locale)
{
	UString cpy(*this);

	ensureUniqueness();
	UErrorCode errcode = U_ZERO_ERROR;

	int32_t len = u_strToUpper(d.get(), capacity, d.get(), size-1, locale.get(), &errcode);

	if (len >= size) {
		grow(len+1);
		errcode = U_ZERO_ERROR;
		u_strToUpper(d.get(), capacity, cpy.d.get(), cpy.size-1, locale.get(), &errcode);
	}

	size = len+1;

	return *this;
}


UString& UString::ltrim()
{
	if (isEmpty())
		return *this;

	ensureUniqueness();

	int32_t len = size-1;
	int32_t i = 0;
	UChar32 c;
	UChar* str = d.get();
	int32_t begin = 0;

	for (;;) {
		begin = i;
		if (i >= len)
			break;
		U16_NEXT(str, i, len, c);
		if (c != 0x20  &&  !u_isUWhiteSpace(c))
			break;
	}

	if (begin >= len) {
		str[0] = 0;
		size = 0;
	} else {
		growWithOffset(capacity, begin, 0, size-begin);
	}

	return *this;
}


UString& UString::rtrim()
{
	if (isEmpty())
		return *this;

	ensureUniqueness();

	int32_t i = size-1;
	UChar32 c;
	UChar* str = d.get();
	int32_t len = 0;

	for (;;) {
		len = i;
		if (i <= 0)
			break;
		U16_PREV(str, 0, i, c);
		if (c != 0x20  &&  !u_isUWhiteSpace(c))
			break;
	}

	str[len] = 0;
	size = len+1;

	return *this;
}


UString& UString::trim()
{
	rtrim();
	return ltrim();
}


size_t UString::toUTF8(char* dest, size_t destSize) const
{
	UErrorCode errcode = U_ZERO_ERROR;
	int32_t destLen;
	u_strToUTF8(dest, destSize, &destLen, d.get(), size-1, &errcode);
	return destLen;
}


ByteArray UString::toUTF8() const
{
	char* outbuf = new char[size*4];
	size_t len = toUTF8(outbuf, size*4);
	return ByteArray::from(outbuf, len, size*4);
}


bool UString::isWhitespaceOnly() const
{
	const UChar* str = d.get();
	int32_t len = size-1;
	int32_t i = 0;
	UChar32 c;

	while (i < len) {
		U16_NEXT(str, i, len, c);

		if (c != 0x20  &&  !u_isUWhiteSpace(c))
			return false;
	}

	return true;
}


UString& UString::append(long val)
{
	UChar buf[64];
	UChar* beg = _FormatIntUTF16<long>(buf, val);
	append(UString(beg, buf-beg-1));
	return *this;
}


UString& UString::append(unsigned long val)
{
	UChar buf[64];
	UChar* beg = _FormatUIntUTF16<unsigned long>(buf, val);
	append(UString(beg, buf-beg-1));
	return *this;
}


UString& UString::append(int val)
{
	UChar buf[64];
	UChar* beg = _FormatIntUTF16<int>(buf, val);
	append(UString(beg, buf-beg-1));
	return *this;
}


UString& UString::append(unsigned int val)
{
	UChar buf[64];
	UChar* beg = _FormatUIntUTF16<unsigned int>(buf, val);
	append(UString(beg, buf+sizeof(buf)/sizeof(UChar)-beg-1));
	return *this;
}


UString& UString::append(float val)
{
	return append((double) val);
}


UString& UString::append(double val)
{
	// TODO: Maybe there's a nicer option?

	char buf[64];
	int len = sprintf(buf, "%f", val);

	UChar u16Buf[64];

	// sprintf only prints ASCII characters, which are at the beginning of the Unicode BMP, and the BMP is encoded
	// as the code point value in UTF-16, so we basically just have to 'widen' the output of sprintf here.
	for (int i = 0 ; i < len ; i++) {
		u16Buf[i] = (UChar) buf[i];
	}

	return append(UString(u16Buf, len));
}

