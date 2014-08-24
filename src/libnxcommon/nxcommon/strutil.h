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

#ifndef NXCOMMON_STRUTIL_H_
#define NXCOMMON_STRUTIL_H_

#include <nxcommon/config.h>
#include <locale>
#include <cstring>
#include <cstdio>

using std::locale;
using std::collate;
using std::use_facet;


typedef long hash_t;




/**
 *	The index into this array is a number system base. The corresponding value in the array is the maximum power of this
 *	base that fits in a 64-bit unsigned integer.
 *
 *	It was calculated using the following formula:
 *
 *	maxPower = pow(base, floor(log(UINT64_MAX) / log(base)));
 */
static const uint64_t _Uint64MaxPowersByBase[] = {
		UINT64_MAX,				// INVALID
		UINT64_MAX,				// INVALID

		9223372036854775808UL,	// 2 ^ 63
		12157665459056928801UL,	// 3 ^ 40
		4611686018427387904UL,	// 4 ^ 31
		7450580596923828125UL,	// 5 ^ 27
		4738381338321616896UL,	// 6 ^ 24
		3909821048582988049UL,	// 7 ^ 22
		9223372036854775808UL,	// 8 ^ 21
		12157665459056928801UL,	// 9 ^ 20
		10000000000000000000UL,	// 10 ^ 19
		5559917313492231481UL,	// 11 ^ 18
		2218611106740436992UL,	// 12 ^ 17
		8650415919381337933UL,	// 13 ^ 17
		2177953337809371136UL,	// 14 ^ 16
		6568408355712890625UL,	// 15 ^ 16
		1152921504606846976UL,	// 16 ^ 15
		2862423051509815793UL,	// 17 ^ 15
		6746640616477458432UL,	// 18 ^ 15
		15181127029874798299UL,	// 19 ^ 15
		1638400000000000000UL,	// 20 ^ 14
		3243919932521508681UL,	// 21 ^ 14
		6221821273427820544UL,	// 22 ^ 14
		11592836324538749809UL,	// 23 ^ 14
		876488338465357824UL,	// 24 ^ 13
		1490116119384765625UL,	// 25 ^ 13
		2481152873203736576UL,	// 26 ^ 13
		4052555153018976267UL,	// 27 ^ 13
		6502111422497947648UL,	// 28 ^ 13
		10260628712958602189UL,	// 29 ^ 13
		15943230000000000000UL,	// 30 ^ 13
		787662783788549761UL,	// 31 ^ 12
		1152921504606846976UL,	// 32 ^ 12
		1667889514952984961UL,	// 33 ^ 12
		2386420683693101056UL,	// 34 ^ 12
		3379220508056640625UL,	// 35 ^ 12
		4738381338321616896UL,	// 36 ^ 12
};


static const char _DigitToCharMap[] = {
		'0',
		'1',
		'2',
		'3',
		'4',
		'5',
		'6',
		'7',
		'8',
		'9',
		'A',
		'B',
		'C',
		'D',
		'E',
		'F',
		'G',
		'H',
		'I',
		'J',
		'K',
		'L',
		'M',
		'N',
		'O',
		'P',
		'Q',
		'R',
		'S',
		'T',
		'U',
		'V',
		'W',
		'X',
		'Y',
		'Z',
};





/**	\brief Converts src to lower case.
 *
 * 	src is assumed to be ASCII.
 *
 * 	@param dest The buffer to write the lower case characters to.
 *	@param src The mixed case characters.
 */
void strtolower(char* dest, const char* src, size_t len);
void strtoupper(char* dest, const char* src, size_t len);

inline void strtolower(char* dest, const char* src)
{
	size_t len = strlen(src);
	strtolower(dest, src, len);
	dest[len] = '\0';
}
inline void strtoupper(char* dest, const char* src)
{
	size_t len = strlen(src);
	strtoupper(dest, src, len);
	dest[len] = '\0';
}



const char* FindLastOccurrence(const char* str, const char* from, const char* chrs);

inline char* FindLastOccurrence(char* str, char* from, const char* chrs)
		{ return const_cast<char*>(FindLastOccurrence((const char*) str, (const char*) from, chrs)); }

inline const char* FindLastOccurrence(const char* str, const char* from, char chr)
		{ char b[2]; b[0] = chr; b[1] = '\0'; return FindLastOccurrence(str, from, b); }

inline char* FindLastOccurrence(char* str, char* from, char chr)
		{ char b[2]; b[0] = chr; b[1] = '\0'; return FindLastOccurrence(str, from, b); }



const char* rtrimfind(const char* str, const char* rbegin, const char* chrs);

inline char* rtrimfind(char* str, char* rbegin, const char* chrs)
		{ return const_cast<char*>(rtrimfind(str, rbegin, chrs)); }

inline const char* rtrimfind(const char* str, const char* rbegin, char chr)
		{ char b[2]; b[0] = chr; b[1] = '\0'; return rtrimfind(str, rbegin, b); }

inline char* rtrimfind(char* str, char* rbegin, char chr)
		{ char b[2]; b[0] = chr; b[1] = '\0'; return rtrimfind(str, rbegin, b); }


inline const char* rtrimfind(const char* str, const char* chrs)
		{ return rtrimfind(str, str+strlen(str)-1, chrs); }

inline char* rtrimfind(char* str, const char* chrs)
		{ return const_cast<char*>(rtrimfind((const char*) str, chrs)); }

inline const char* rtrimfind(const char* str, char chr)
		{ char b[2]; b[0] = chr; b[1] = '\0'; return rtrimfind(str, b); }

inline char* rtrimfind(char* str, char chr)
		{ char b[2]; b[0] = chr; b[1] = '\0'; return rtrimfind(str, b); }

char* rtrim(char* str, const char* chrs);

/**	\brief Trims chr from the right end of str.
 *
 * 	@param str The string to trim.
 * 	@param chr The character to remove from the end of str.
 */
inline char* rtrim(char* str, char chr) { char b[2]; b[0] = chr; b[1] = '\0'; return rtrim(str, b); }

const char* ltrim(const char* str, char chr);

/**	\brief Breaks a string into multiple lines using word wrapping.
 *
 * 	This function will try to wrap words by breaking at whitespace positions. If a word is longer than the
 * 	number of chars per line, it will however be broken on the last possible position in line.
 * 	Note that only plain space characters are used as word boundaries. Tabulators, Newlines and other
 * 	whitespace will break the wrapping.
 * 	The added newlines will always be \n only.
 *
 * 	@param src The text to be wrapped.
 * 	@param cpl Maximum number of characters per line.
 */
char* wrapText(const char* src, int cpl);

/**	\brief Indents a text.
 *
 * 	This inserts the indentation string after every newline in the string. It is assumed that the input
 * 	string uses only \n as line terminator.
 * 	Note that the indentation string is NOT added to the beginning of the string.
 */
char* indent(const char* src, const char* indStr);


bool WildcardMatch(const char* pattern, const char* text);



inline hash_t Hash(const char* str, size_t len)
{
	return use_facet< collate<char> >(locale()).hash(str, str+len);
}


inline hash_t Hash(const char* str)
{
	return Hash(str, strlen(str));
}


hash_t LowerHash(const char* str, size_t len);

inline hash_t LowerHash(const char* str)
{
	return LowerHash(str, strlen(str));
}


void FormatByteSize(char* str, uint64_t size, int precision = 3);

size_t EscapeString(char* dest, const char* src, size_t srcLen, size_t destLen, const char* charsToEscape, char escapeChar = '\\');

size_t EscapeString(char* dest, const char* src, size_t destLen, const char* charsToEscape, char escapeChar = '\\');

size_t nxstrnlen(const char* s, size_t maxlen);



uint8_t StringToUInt8(const char* str, size_t len, int base = 0, bool* success = NULL);
int8_t StringToInt8(const char* str, size_t len, int base = 0, bool* success = NULL);
uint16_t StringToUInt16(const char* str, size_t len, int base = 0, bool* success = NULL);
int16_t StringToInt16(const char* str, size_t len, int base = 0, bool* success = NULL);
uint32_t StringToUInt32(const char* str, size_t len, int base = 0, bool* success = NULL);
int32_t StringToInt32(const char* str, size_t len, int base = 0, bool* success = NULL);
uint64_t StringToUInt64(const char* str, size_t len, int base = 0, bool* success = NULL);
int64_t StringToInt64(const char* str, size_t len, int base = 0, bool* success = NULL);

inline uint8_t StringToUInt8(const char* str, int base = 0, bool* success = NULL)
		{ return StringToUInt8(str, strlen(str), base, success); }
inline int8_t StringToInt8(const char* str, int base = 0, bool* success = NULL)
		{ return StringToInt8(str, strlen(str), base, success); }
inline uint16_t StringToUInt16(const char* str, int base = 0, bool* success = NULL)
		{ return StringToUInt16(str, strlen(str), base, success); }
inline int16_t StringToInt16(const char* str, int base = 0, bool* success = NULL)
		{ return StringToInt16(str, strlen(str), base, success); }
inline uint32_t StringToUInt32(const char* str, int base = 0, bool* success = NULL)
		{ return StringToUInt32(str, strlen(str), base, success); }
inline int32_t StringToInt32(const char* str, int base = 0, bool* success = NULL)
		{ return StringToInt32(str, strlen(str), base, success); }
inline uint64_t StringToUInt64(const char* str, int base = 0, bool* success = NULL)
		{ return StringToUInt64(str, strlen(str), base, success); }
inline int64_t StringToInt64(const char* str, int base = 0, bool* success = NULL)
		{ return StringToInt64(str, strlen(str), base, success); }

double StringToDouble(const char* str, bool* success = NULL);
double StringToDouble(const char* str, size_t len, bool* success = NULL);

float StringToFloat(const char* str, bool* success = NULL);
float StringToFloat(const char* str, size_t len, bool* success = NULL);

template <class CharT>
size_t UInt64ToString(CharT* str, uint64_t value, unsigned int base = 10)
{
	CharT* origStr = str;
	uint64_t div = _Uint64MaxPowersByBase[base];

	while (value != 0) {
		unsigned int digit = value / div;

		if (str != origStr  ||  digit != 0) {
			*str++ = (CharT) _DigitToCharMap[digit];
		}

		value -= digit*div;
		div /= base;
	}

	*str = (CharT) '\0';

	return str-origStr;
}


#endif /* NXCOMMON_STRUTIL_H_ */
