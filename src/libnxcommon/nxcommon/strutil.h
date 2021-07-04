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
#include <nxcommon/util.h>
#include <locale>
#include <cstring>
#include <cstdio>

using std::locale;
using std::collate;
using std::use_facet;


typedef long hash_t;



// These arrays are indexed by a numeric base between 2 and 36 (0 and 1 are invalid) and contain the maximum power of that base that
// fits in an unsigned long long type.
// These arrays were generated using the following C++ program (using the GMP library):
/*
#include <cstdio>
#include <cstdio>
#include <stdint.h>
#include <gmp.h>

#define NUM_BITS 128
#define TYPE_NAME "unsigned long long"
#define ARR_SUFFIX "ULongLong"

int main(int argc, char** argv)
{
	mpz_t maxVal;
	mpz_init(maxVal);
	mpz_set_ui(maxVal, 2);
	mpz_pow_ui(maxVal, maxVal, NUM_BITS);
	mpz_sub_ui(maxVal, maxVal, 1);

	printf("static const %s _MaxPowersByBase%s[] = {", TYPE_NAME, ARR_SUFFIX);

	printf("0u, 0u");

	for (unsigned int base = 2 ; base <= 36 ; base++) {
		mpz_t maxPower;
		mpz_init(maxPower);
		mpz_set_ui(maxPower, 0);

		mpz_t nextPower;
		mpz_init(nextPower);
		mpz_set_ui(nextPower, base);

		while (mpz_cmp(nextPower, maxVal) < 0) {
			mpz_set(maxPower, nextPower);
			mpz_mul_ui(nextPower, nextPower, base);
		}

		gmp_printf(", %Zuu", maxPower, base);
	}

	printf(" };\n");

    return 0;
}
*/
#if ULONGLONG_SIZE == 4
static const unsigned long long _MaxPowersByBaseULongLong[] = {0u, 0u, 2147483648u, 3486784401u, 1073741824u, 1220703125u, 2176782336u, 1977326743u, 1073741824u, 3486784401u, 1000000000u, 2357947691u, 429981696u, 815730721u, 1475789056u, 2562890625u, 268435456u, 410338673u, 612220032u, 893871739u, 1280000000u, 1801088541u, 2494357888u, 3404825447u, 191102976u, 244140625u, 308915776u, 387420489u, 481890304u, 594823321u, 729000000u, 887503681u, 1073741824u, 1291467969u, 1544804416u, 1838265625u, 2176782336u };
#elif ULONGLONG_SIZE == 8
static const unsigned long long _MaxPowersByBaseULongLong[] = {0u, 0u, 9223372036854775808u, 12157665459056928801u, 4611686018427387904u, 7450580596923828125u, 4738381338321616896u, 3909821048582988049u, 9223372036854775808u, 12157665459056928801u, 10000000000000000000u, 5559917313492231481u, 2218611106740436992u, 8650415919381337933u, 2177953337809371136u, 6568408355712890625u, 1152921504606846976u, 2862423051509815793u, 6746640616477458432u, 15181127029874798299u, 1638400000000000000u, 3243919932521508681u, 6221821273427820544u, 11592836324538749809u, 876488338465357824u, 1490116119384765625u, 2481152873203736576u, 4052555153018976267u, 6502111422497947648u, 10260628712958602189u, 15943230000000000000u, 787662783788549761u, 1152921504606846976u, 1667889514952984961u, 2386420683693101056u, 3379220508056640625u, 4738381338321616896u };
#elif ULONGLONG_SIZE == 16
static const unsigned long long _MaxPowersByBaseULongLong[] = {0u, 0u, 170141183460469231731687303715884105728u, 147808829414345923316083210206383297601u, 85070591730234615865843651857942052864u, 277555756156289135105907917022705078125u, 134713546244127343440523266742756048896u, 107006904423598033356356300384937784807u, 85070591730234615865843651857942052864u, 147808829414345923316083210206383297601u, 100000000000000000000000000000000000000u, 340039485861577398992406882305761986971u, 59066822915424320448445358917464096768u, 74829695578286078013428929473144712489u, 66408730383449729837806206197059026944u, 43143988327398919500410556793212890625u, 21267647932558653966460912964485513216u, 139288917338851014461418017489467720433u, 45517159607903340355793714778287898624u, 230466617897195215045509519405933293401u, 53687091200000000000000000000000000000u, 220983347100817338120002444455525554981u, 38711059958478986452554295441868455936u, 134393854047545109686936775588697536481u, 18437563379178327736384102280592359424u, 55511151231257827021181583404541015625u, 160059109085386090080713531498405298176u, 16423203268260658146231467800709255289u, 42277452950578284263485622772148731904u, 105280501585190501232597819292755591721u, 254186582832900000000000000000000000000u, 19232792489931358333837313998767870751u, 42535295865117307932921825928971026432u, 91801229324973413645775482048441660193u, 193630125104980427932766033374162714624u, 11419131242070580387175083160400390625u, 22452257707354557240087211123792674816u };
#else
#error Unsupported value of ULONGLONG_SIZE!
#endif



static const char _DigitToCharMap[] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
		'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
		'U', 'V', 'W', 'X', 'Y', 'Z',
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
char* wrapText(const char* src, size_t cpl);

/**	\brief Indents a text.
 *
 * 	This inserts the indentation string after every newline in the string. It is assumed that the input
 * 	string uses only \n as line terminator.
 * 	Note that the indentation string is NOT added to the beginning of the string.
 */
char* indent(const char* src, const char* indStr, bool indentStart = false, const char* newline = "\n");


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
size_t ULongLongToString(CharT* str, unsigned long long value, unsigned int base = 10)
{
	if (value == 0) {
		str[0] = '0';
		str[1] = '\0';
		return 1;
	}

	CharT* origStr = str;

	// Write it in reverse...
	while (value != 0) {
		unsigned int digit = value % base;
		*str++ = (CharT) _DigitToCharMap[digit];
		value /= base;
	}

	size_t len = str-origStr;

	*str-- = '\0';

	// Aaaand reverse it...
	while (origStr < str) {
		CharT tmp = *origStr;
		*origStr = *str;
		*str = tmp;
		origStr++;
		str--;
	}

	return len;

	// TODO: It doesn't work. And I don't care. Fuck this shit.
	/*if (value == 0) {
		str[0] = '0';
		str[1] = '\0';
		return 1;
	}

	CharT* origStr = str;
	unsigned long long div = _MaxPowersByBaseULongLong[base];

	while (value != 0) {
		unsigned int digit = value / div;

		if (str != origStr  ||  digit != 0) {
			*str++ = (CharT) _DigitToCharMap[digit];
		}

		value -= digit*div;
		div /= base;
	}

	*str = (CharT) '\0';

	return str-origStr;*/
}


#endif /* NXCOMMON_STRUTIL_H_ */
