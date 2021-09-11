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

#include "strutil.h"
#include <algorithm>
#include <limits>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>

using std::string;





void strtolower(char* dest, const char* src, size_t len)
{
	size_t i;
	for (i = 0 ; i < len  &&  src[i] != '\0' ; i++) {
		dest[i] = tolower(src[i]);
	}
}


void strtoupper(char* dest, const char* src, size_t len)
{
	size_t i;
	for (i = 0 ; i < len  &&  src[i] != '\0' ; i++) {
		dest[i] = toupper(src[i]);
	}
}


const char* FindLastOccurrence(const char* str, const char* from, const char* chrs)
{
	while (from != str  &&  strchr(chrs, *from) == NULL)
		from--;

	if (from == str  &&  strchr(chrs, *from) == NULL)
		return NULL;

	return from;
}


const char* rtrimfind(const char* str, const char* rbegin, const char* chrs)
{
	while (rbegin != str-1  &&  strchr(chrs, *rbegin)  !=  NULL) {
		rbegin--;
	}

	return rbegin+1;
}


char* rtrim(char* str, const char* chrs)
{
	char* end = rtrimfind(str, chrs);
	*end = '\0';
	return end;

	/*int len = strlen(str);

	if (len == 0) {
		return str;
	}

	char* strBeg = str;
	str += len-1;
	while (str >= strBeg  &&  strchr(chrs, *str)  !=  NULL) {
		*str-- = '\0';
	}

	return str+1;*/
}


const char* ltrim(const char* str, char chr)
{
	size_t len = strlen(str);
	while (str != str+len  &&  *str == chr) str++;
	return str;
}


char* wrapText(const char* src, size_t cpl)
{
	string out;
	const char* lastWordStart = src;
	size_t numCharsInLine = 0;

	do {
		if (size_t(src-lastWordStart) > cpl) {
			// The currently parsed word is yet so long that it doesn't fit into a single line at all. This
			// means it will have to be split in the middle, which we do here. We put as many characters of
			// the word as possible into the current line.

			size_t charsLeft = cpl-numCharsInLine;
			out.append(lastWordStart, charsLeft);
			out += '\n';

			// Reposition src to point to the next character of the long word, which didn't fit on this line.
			src = lastWordStart+charsLeft;

			lastWordStart = src; // Pretend the rest is a new word
			numCharsInLine = 0;
			continue;
		}

		if (*src == '\0'  ||  *src == ' '  ||  *src == '\n') {
			size_t wordLen = src-lastWordStart; // Without space
			size_t charsLeft = cpl-numCharsInLine;

			if (charsLeft < wordLen) {
				// Not enough space for the word -> Create a newline
				out[out.length()-1] = '\n'; // A space was there before

				// For simplicity, we will just parse the word again. -1 is because src is incremented at
				// loop end.
				src = lastWordStart-1;

				numCharsInLine = 0;
			} else {
				out.append(lastWordStart, wordLen);

				if (charsLeft != wordLen  &&  *src == ' ') {
					// *src is a space, and there is enough space left in the line to append it.
					out += ' ';
					numCharsInLine += wordLen+1;
				} else {
					if (*src == ' ') {
						// *src is a space, but there's no space left for it. We will explicitly create a
						// newline here, because the algorithm assumes there's always a space after the last
						// inserted word.
						// We don't create this newline if this is the string end ('\0'). Also, if *src is
						// a hard break ('\n'), we use that as our newline.
						out += '\n';
					}
					numCharsInLine = 0;
				}

				if (*src == '\n') {
					// A hard break always breaks the current line.
					out += '\n';
					numCharsInLine = 0;
				}

				// Next word
				lastWordStart = src+1;
			}
		}
	} while (*src++ != '\0');

	char* ret = new char[out.length()+1];
	strcpy(ret, out.c_str());

	return ret;
}


char* indent(const char* src, const char* indStr, bool indentStart, const char* newline)
{
	const char* srcStart = src;
	size_t srcLen = strlen(src);
	size_t indStrLen = strlen(indStr);
	size_t newlineLen = strlen(newline);

	size_t numLines = 1;

	while (src) {
		if ((src = strstr(src, newline)) != NULL) {
			numLines++;
			src += newlineLen;
		}
	}

	size_t numIndents = numLines;
	if (!indentStart) {
		numIndents--;
	}

	src = srcStart;
	char* dest = new char[srcLen + numIndents*indStrLen + 1];
	char* destStart = dest;

	if (indentStart) {
		strcpy(dest, indStr);
		dest += indStrLen;
	}

	while (src != NULL) {
		const char* nlPos = strstr(src, newline);

		if (nlPos) {
			size_t lineLen = nlPos-src + newlineLen;

			strncpy(dest, src, lineLen);
			dest += lineLen;

			strcpy(dest, indStr);
			dest += indStrLen;

			src += lineLen;
		} else {
			strcpy(dest, src);
			src = NULL;
		}
	}

	return destStart;
}


bool WildcardMatch(const char* pattern, const char* text)
{
	bool escaped = false;
	while (*text != '\0') {
		if (escaped) {
			if (*text != *pattern) {
				return false;
			}
			escaped = false;
		} else {
			switch (*pattern) {
			case '?':
				break;
			case '*':
				do {
					pattern++;
				} while(*pattern == '*');
				if (*pattern == '\0') {
					return true;
				}
				while (*text != '\0') {
					if (WildcardMatch(pattern, text)) {
						return true;
					}
					text++;
				}
				return false;
			case '\\':
				escaped = true;
				text--;
				break;
			default:
				if (*pattern != *text) {
					return false;
				}
				break;
			}
		}
		text++;
		pattern++;
	}
	while (*pattern == '*') pattern++;
	return *pattern == '\0';
}


hash_t LowerHash(const char* str, size_t len)
{
	char* lStr = new char[len];
	strtolower(lStr, str, len);
	hash_t hash = Hash(lStr, len);
	delete[] lStr;
	return hash;
}


void FormatByteSize(char* str, uint64_t size, int precision)
{
	if (size > 1000000000) {
		sprintf(str, "%.*fGB", precision, size/1000000000.0f);
	} else if (size > 1000000) {
		sprintf(str, "%.*fMB", precision, size/1000000.0f);
	} else if (size > 1000) {
		sprintf(str, "%.*fkB", precision, size/1000.0f);
	} else {
		sprintf(str, "%llu", (long long unsigned int) size);
	}
}


size_t EscapeString(char* dest, const char* src, size_t srcLen, size_t destLen, const char* charsToEscape, char escapeChar)
{
	if (destLen == 0)
		return 0;

	char* destStart = dest;
	const char* srcEnd = src+srcLen;
	char* destEnd = dest+destLen - 1;

	while (src != srcEnd  &&  dest != destEnd) {
		char c = *src++;

		if (strchr(charsToEscape, c) != NULL) {
			*dest++ = escapeChar;

			if (dest != destEnd) {
				*dest++ = c;
			}
		} else {
			*dest++ = c;
		}
	}

	*dest = '\0';

	return dest - destStart;
}


size_t EscapeString(char* dest, const char* src, size_t destLen, const char* charsToEscape, char escapeChar)
{
	return EscapeString(dest, src, strlen(src), destLen, charsToEscape, escapeChar);
}


size_t nxstrnlen(const char* s, size_t maxlen)
{
#if (_XOPEN_SOURCE >= 700 || _POSIX_C_SOURCE >= 200809L)  ||  (_GNU_SOURCE)
	return strnlen(s, maxlen);
#elif defined(_MSC_VER)
	return strnlen(s, maxlen);
#else
	size_t len;
	for (len = 0 ; len < maxlen  &&  s[len] != '\0' ; len++);
	return len;
#endif
}


template <typename T, bool allowSigned, T maxValue>
T _StringToIntTExtended(const char* str, size_t len, int base, bool* success)
{
	if (len == 0) {
		if (success) {
			*success = false;
		}

		return 0;
	}

	if (base > 36) {
		if (success) {
			*success = false;
		}

		return 0;
	}

	int signMultiplier = 1;

	// First, see if there is a plus or minus sign in the beginning

	if (*str == '+') {
		str++;
		len--;
	} else if (*str == '-') {
		if (allowSigned) {
			signMultiplier = -1;
			str++;
			len--;
		} else {
			if (success) {
				*success = false;
			}

			return 0;
		}
	}

	// Now, see if there are any base specifiers at the beginning

	if (*str == '0') {
		if (base == 0  ||  base == 8) {
			// For base 0 (auto-determine) or 8, a leading zero means an octal value
			base = 8;
		}

		// For all other fixed bases, it's just ignored.
		str++;
		len--;
	}
	if (len >= 2  &&  str[0] == '0'  &&  str[1] == 'x') {
		if (base == 0  ||  base == 16) {
			// For base 0 (auto-determine) or 16, a leading '0x' signals a hexadecimal value
			base = 16;
		} else {
			// For all other fixed bases, it is invalid (because of the 'x').

			if (success) {
				*success = false;
			}

			return 0;
		}

		str += 2;
		len -= 2;
	}

	if (base == 0) {
		// If base is still 0 here, no base prefix was found, so we assume decimal numbers.
		base = 10;
	}

	T value = 0;
	T digitMultiplier = signMultiplier;

	const char* cstr = str + len;

	do {
		cstr--;

		char c = *cstr;

		T unitVal = std::numeric_limits<T>::max(); // Anything invalid initially

		if (c >= '0'  &&  c <= '9') {
			unitVal = c-'0';
		} else if (c >= 'A'  &&  c <= 'Z') {
			unitVal = c-'A' + 10;
		} else if (c >= 'a'  &&  c <= 'z') {
			unitVal = c-'a' + 10;
		}

		if (unitVal >= (T) base) {
			if (success) {
				*success = false;
			}

			return 0;
		}

		value += unitVal * digitMultiplier;

		digitMultiplier *= base;
	} while (cstr != str);

	if (success) {
		*success = true;
	}

	return value;
}


uint8_t StringToUInt8(const char* str, size_t len, int base, bool* success)
{
	return _StringToIntTExtended<uint8_t, false, UINT8_MAX>(str, len, base, success);
}


int8_t StringToInt8(const char* str, size_t len, int base, bool* success)
{
	return _StringToIntTExtended<int8_t, true, INT8_MAX>(str, len, base, success);
}


uint16_t StringToUInt16(const char* str, size_t len, int base, bool* success)
{
	return _StringToIntTExtended<uint16_t, false, UINT16_MAX>(str, len, base, success);
}


int16_t StringToInt16(const char* str, size_t len, int base, bool* success)
{
	return _StringToIntTExtended<int16_t, true, INT16_MAX>(str, len, base, success);
}


uint32_t StringToUInt32(const char* str, size_t len, int base, bool* success)
{
	return _StringToIntTExtended<uint32_t, false, UINT32_MAX>(str, len, base, success);
}


int32_t StringToInt32(const char* str, size_t len, int base, bool* success)
{
	return _StringToIntTExtended<int32_t, true, INT32_MAX>(str, len, base, success);
}


uint64_t StringToUInt64(const char* str, size_t len, int base, bool* success)
{
	return _StringToIntTExtended<uint64_t, false, UINT64_MAX>(str, len, base, success);
}


int64_t StringToInt64(const char* str, size_t len, int base, bool* success)
{
	return _StringToIntTExtended<int64_t, true, INT64_MAX>(str, len, base, success);
}


double StringToDouble(const char* str, bool* success)
{
	char* endPtr;
	double val = strtod(str, &endPtr);

	if (success) {
		*success = (*endPtr == '\0')  &&  (*str != '\0');
	}

	return val;
}


double StringToDouble(const char* str, size_t len, bool* success)
{
	char* termStr = new char[len+1];
	termStr[len] = '\0';
	memcpy(termStr, str, len);
	double val = StringToDouble(termStr, success);
	delete[] termStr;
	return val;
}


float StringToFloat(const char* str, bool* success)
{
	return (float) StringToDouble(str, success);
}


float StringToFloat(const char* str, size_t len, bool* success)
{
	return (float) StringToDouble(str, len, success);
}
