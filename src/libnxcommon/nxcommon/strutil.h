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

#ifndef STRUTIL_H_
#define STRUTIL_H_

#include <nxcommon/config.h>
#include <locale>
#include <cstring>
#include <cstdio>

using std::locale;
using std::collate;
using std::use_facet;


typedef long hash_t;



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


#endif /* STRUTIL_H_ */
