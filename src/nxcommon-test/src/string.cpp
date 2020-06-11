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

#include "global.h"
#include <nxcommon/CString.h>
#include <nxcommon/strutil.h>
#include <vector>
#include <list>
#include <set>

#ifdef NXCOMMON_UNICODE_ENABLED
#include <nxcommon/UString.h>
#include <nxcommon/unicodeutil.h>
#endif

using std::vector;
using std::list;
using std::set;




TEST(StringTest, CheckStrutilAndCString)
{
	struct CaseConvTest {
		const char* src;
		const char* dst;
		size_t len;
	};

	CaseConvTest utests[] = {
			{"Hallo Welt!",								"HALLO WELT!",							0},
			{" ?%StrINg  MIt}So**nderzei$$cH!en ",		" ?%STRING  MIT}SO**NDERZEI$$CH!EN ",	0},
			{"F",										"F",									0},
			{"a",										"A",									0},
			{"Fubar!",									"FUbar!",								2},
			{"The quick brown fox",						"THE QUICk brown fox",					8}
	};
	CaseConvTest ltests[] = {
			{"Hallo Welt!",								"hallo welt!",							0},
			{" ?%StrINg  MIt}So**nderzei$$cH!en ",		" ?%string  mit}so**nderzei$$ch!en ",	0},
			{"F",										"f",									0},
			{"a",										"a",									0},
			{"FubAr!",									"fubAr!",								2},
			{"The QUick bROWn fox",						"the quick bROWn fox",					8}
	};

	for (unsigned int i = 0 ; i < sizeof(utests) / sizeof(CaseConvTest) ; i++) {
		CaseConvTest& t = utests[i];
		char dst[512];

		if (t.len == 0) {
			strtoupper(dst, t.src);
		} else {
			strcpy(dst, t.src);
			strtoupper(dst, t.src, t.len);
		}

		EXPECT_STREQ(t.dst, dst) << "strtoupper test #" << i << " failed!";

		if (t.len == 0) {
			CString csrc(t.src);
			EXPECT_EQ(CString(t.dst), csrc.upper()) << "CString::upper() test #" << i << " failed!";
		}
	}
	for (unsigned int i = 0 ; i < sizeof(ltests) / sizeof(CaseConvTest) ; i++) {
		CaseConvTest& t = ltests[i];
		char dst[512];

		if (t.len == 0) {
			strtolower(dst, t.src);
		} else {
			strcpy(dst, t.src);
			strtolower(dst, t.src, t.len);
		}

		EXPECT_STREQ(t.dst, dst) << "strtolower test #" << i << " failed!";

		if (t.len == 0) {
			CString csrc(t.src);
			EXPECT_EQ(CString(t.dst), csrc.lower()) << "CString::lower() test #" << i << " failed!";
		}
	}

	enum TrimMode { Left, Right, Both };

	struct TrimTest {
		const char* src;
		const char* dst;
		char c;
		TrimMode mode;
	};

	TrimTest trims[] = {
			{"   Hallo Welt!  ",				"Hallo Welt!  ",				' ',	Left},
			{"    F  ranz! ",					"F  ranz! ",					' ',	Left},
			{"Hallo! ",							"Hallo! ",						' ',	Left},
			{"/home/alemariusnexus",			"home/alemariusnexus",			'/',	Left},
			{"/home/alemariusnexus/",			"/home/alemariusnexus",			'/',	Right},
			{"/home/alemariusnexus/",			"home/alemariusnexus",			'/',	Both},
			{"  <a>Hallo Welt!</a>  ",			"<a>Hallo Welt!</a>",			' ',	Both},
			{"None",							"None",							' ',	Both},
			{"  ",								"",								' ',	Left},
			{"  ",								"",								' ',	Right},
			{"  ",								"",								' ',	Both},
			{"",								"",								' ',	Left},
			{"",								"",								' ',	Right},
			{"",								"",								' ',	Both},
	};

	for (unsigned int i = 0 ; i < sizeof(trims) / sizeof(TrimTest) ; i++) {
		TrimTest& t = trims[i];

		if (t.mode == Left) {
			EXPECT_STREQ(t.dst, ltrim(t.src, t.c)) << "Trim test #" << i << " failed!";
			EXPECT_EQ(CString(t.dst), CString(t.src).ltrim(t.c)) << "CString trim test #" << i << " failed!";
		} else if (t.mode == Right) {
			char buf[512];
			strcpy(buf, t.src);
			rtrim(buf, t.c);
			EXPECT_STREQ(t.dst, buf) << "Trim test #" << i << " failed!";
			EXPECT_EQ(CString(t.dst), CString(t.src).rtrim(t.c)) << "CString trim test #" << i << " failed!";
		} else {
			EXPECT_EQ(CString(t.dst), CString(t.src).trim(t.c)) << "CString trim test #" << i << " failed!";
		}
	}

	struct IndentTest {
		const char* src;
		const char* dst;
		const char* ind;
		bool indStart;
		const char* nl;
	};

	IndentTest indents[] = {
			{	"  Hallo Welt!\nDieser Text sollte\n  eingerueckt werden,\n\ndenke ich!",
				"  Hallo Welt!\n  Dieser Text sollte\n    eingerueckt werden,\n  \n  denke ich!",
				"  ",
				false,
				"\n"
			},
			{	"  Hallo Welt!\nDieser Text sollte\n  eingerueckt werden,\n\ndenke ich!",
				"    Hallo Welt!\n  Dieser Text sollte\n    eingerueckt werden,\n  \n  denke ich!",
				"  ",
				true,
				"\n"
			},
			{	"",
				"",
				"Franz",
				false,
				"\n"
			},
			{	"\n",
				"\nHallo Welt!",
				"Hallo Welt!",
				false,
				"\n"
			},
			{	"\n",
				"Hallo Welt!\nHallo Welt!",
				"Hallo Welt!",
				true,
				"\n"
			},
			{
				"This should be\nindented as two lines\r\nand not\nas four.",
				"  This should be\nindented as two lines\r\n  and not\nas four.",
				"  ",
				true,
				"\r\n"
			}
	};

	for (unsigned int i = 0 ; i < sizeof(indents) / sizeof(IndentTest) ; i++) {
		IndentTest& t = indents[i];

		char* indRes = indent(t.src, t.ind, t.indStart, t.nl);
		EXPECT_STREQ(t.dst, indRes) << "indent() test #" << i << " failed!";

		CString csrc1 = CString::readAlias(t.src);
		CString cindRes1 = csrc1.indented(CString::readAlias(t.ind),
				t.indStart, CString::readAlias(t.nl));
		EXPECT_EQ(CString::readAlias(t.src), csrc1) << "CString::indented() test #" << i << ".1 failed!";
		EXPECT_EQ(CString::readAlias(t.dst), cindRes1) << "CString::indented() test #" << i << ".2 failed!";

		CString csrc2 = CString::readAlias(t.src);
		CString cindRes2 = csrc2.indent(CString::readAlias(t.ind),
				t.indStart, CString::readAlias(t.nl));
		EXPECT_EQ(cindRes2, csrc2) << "CString::indent() test #" << i << ".1 failed!";
		EXPECT_EQ(CString::readAlias(t.dst), cindRes2) << "CString::indent() test #" << i << ".2 failed!";

		delete[] indRes;
	}

	struct WildcardTest {
		const char* pattern;
		const char* str;
		bool matches;
	};

	WildcardTest wildcards[] = {
			{"*",								"*Hallo Welt!#**?aa",								true},
			{"*",								"",													true},
			{"Ha*t",							"Hallo Welt",										true},
			{"Ha*t",							"Hallo Welt!",										false},
			{"Hallo We?t!",						"Hallo Welt!",										true},
			{"Hallo We?t!",						"Hallo WeXt!",										true},
			{"Hallo We?t!",						"Hallo Wellt!",										false},
			{"Ich * ?ich!",						"Ich mag dich!",									true},
			{"Ich * ?ich!",						"Ich mag mich!",									true},
			{"Ich * ?ich!",						"Ich hasse dich!",									true},
			{"Ich * ?ich!",						"Ich bin ich!",										false},
			{"Ich * ?ich!",						"Ich wich!",										false},
			{"Ich * ?ich!",						"Ich  wich!",										true}
	};

	for (unsigned int i = 0 ; i < sizeof(wildcards) / sizeof(WildcardTest) ; i++) {
		WildcardTest& t = wildcards[i];

		if (t.matches) {
			EXPECT_TRUE(WildcardMatch(t.pattern, t.str)) << "Wildcard pattern #" << i << " (" << t.pattern
					<< ") didn't match the text (" << t.str << ")!";
		} else {
			EXPECT_FALSE(WildcardMatch(t.pattern, t.str)) << "Wildcard pattern #" << i << " (" << t.pattern
					<< ") matched the text (" << t.str << "), although it shouldn't!";
		}
	}

	EXPECT_EQ(Hash("hallo welt!"), LowerHash("Hallo Welt!")) << "LowerHash() function does not work!";


	char* tstr1 = new char[64];
	strcpy(tstr1, "Hallo schoene Welt!");
	CString cstr1(tstr1);
	EXPECT_EQ(CString("Hallo schoene Welt!"), cstr1);
	EXPECT_TRUE(cstr1 == "Hallo schoene Welt!");
	EXPECT_TRUE(cstr1 != "Hallo schoene welt!");
	strcpy(tstr1, "Hallo grausame Welt!");
	EXPECT_EQ(CString("Hallo schoene Welt!"), cstr1);
	delete[] tstr1;

	char* tstr2 = new char[64];
	strcpy(tstr2, "Hallo schoene Welt!");
	CString cstr2 = CString::from(tstr2, strlen(tstr2), 64);
	EXPECT_EQ(CString("Hallo schoene Welt!"), cstr2);
	strcpy(tstr2, "Hallo grausame Welt!");
	cstr2.resize();
	EXPECT_EQ(CString("Hallo grausame Welt!"), cstr2);

	EXPECT_EQ(CString((const char*) NULL), CString());
	EXPECT_TRUE(CString().isNull());

	cstr1 = CString("  ##Hallo Welt!#### ");
	cstr2 = cstr1;
	EXPECT_EQ(cstr1, CString("  ##Hallo Welt!#### "));
	EXPECT_EQ(cstr2, CString("  ##Hallo Welt!#### "));
	cstr2.lower();
	EXPECT_EQ(cstr1, CString("  ##Hallo Welt!#### "));
	EXPECT_EQ(cstr2, CString("  ##hallo welt!#### "));
	cstr1.upper();
	EXPECT_EQ(cstr1, CString("  ##HALLO WELT!#### "));
	EXPECT_EQ(cstr2, CString("  ##hallo welt!#### "));
	EXPECT_EQ(20, cstr1.length());
	EXPECT_EQ(20, cstr2.length());
	cstr1.ltrim();
	EXPECT_EQ(cstr1, CString("##HALLO WELT!#### "));
	EXPECT_EQ(cstr2, CString("  ##hallo welt!#### "));
	EXPECT_EQ(18, cstr1.length());
	EXPECT_EQ(20, cstr2.length());
	cstr2.rtrim();
	EXPECT_EQ(cstr1, CString("##HALLO WELT!#### "));
	EXPECT_EQ(cstr2, CString("  ##hallo welt!####"));
	cstr1.trim('#');
	EXPECT_EQ(cstr1, CString("HALLO WELT!#### "));
	EXPECT_EQ(cstr2, CString("  ##hallo welt!####"));
	cstr2.trim(' ');
	EXPECT_EQ(cstr1, CString("HALLO WELT!#### "));
	EXPECT_EQ(cstr2, CString("##hallo welt!####"));
	cstr2.trim('#');
	EXPECT_EQ(cstr1, CString("HALLO WELT!#### "));
	EXPECT_EQ(cstr2, CString("hallo welt!"));
	cstr2.mget()[0] = 'H';
	cstr2.mget()[6] = 'W';
	EXPECT_EQ(cstr1, CString("HALLO WELT!#### "));
	EXPECT_EQ(cstr2, CString("Hallo Welt!"));
	cstr1.append(cstr2);
	EXPECT_EQ(cstr1, CString("HALLO WELT!#### Hallo Welt!"));
	EXPECT_EQ(cstr2, CString("Hallo Welt!"));
	EXPECT_EQ(27, cstr1.length());
	EXPECT_EQ(11, cstr2.length());

	cstr1 = CString("Hallo Welt!");
	cstr2 = cstr1;
	EXPECT_EQ(cstr1, cstr2);
	cstr1.mget()[6] = 'G';
	cstr1.mget()[9] = 'd';
	EXPECT_EQ(cstr1, CString("Hallo Geld!"));
	EXPECT_EQ(cstr2, CString("Hallo Welt!"));
	EXPECT_NE(cstr1, cstr2);

	cstr1 = CString("Hallo Welt!", 5);
	EXPECT_EQ(CString("Hallo"), cstr1);

	EXPECT_NE(CString(), CString("Hallo Welt!"));
	EXPECT_NE(CString("Hallo Welt!"), CString());

	EXPECT_LT(CString("Hallo Geld!"), CString("Hallo Welt"));
	EXPECT_GT(CString("Ich"), CString("Du"));
	EXPECT_GT(CString("Liebe"), CString("Geld"));
	EXPECT_LT(CString("Boese"), CString("Gut"));

	const char* readAliasedBuf = "The quick brown fox.";

	CString rastr = CString::readAlias(readAliasedBuf);
	EXPECT_EQ(CString(readAliasedBuf), rastr);
	EXPECT_EQ(readAliasedBuf, rastr.get());
	rastr.upper();
	EXPECT_NE(readAliasedBuf, rastr.get());
	EXPECT_EQ(CString("THE QUICK BROWN FOX."), rastr);
	EXPECT_NE(CString(readAliasedBuf), rastr);
	EXPECT_EQ(CString("The quick brown fox."), CString(readAliasedBuf));

	char writeAliasedBuf[64];
	strcpy(writeAliasedBuf, readAliasedBuf);

	CString wastr = CString::writeAlias(writeAliasedBuf, strlen(writeAliasedBuf), sizeof(writeAliasedBuf));
	EXPECT_EQ(CString(readAliasedBuf), wastr);
	EXPECT_EQ(writeAliasedBuf, wastr.get());
	wastr.append(CString(" It is running over the fields."));
	EXPECT_EQ(writeAliasedBuf, wastr.get());
	EXPECT_EQ(CString("The quick brown fox. It is running over the fields."), wastr);
	EXPECT_EQ(wastr, CString(writeAliasedBuf));
	wastr << CString(" Its tail is gleaming in the morning sun."); // So cheesy...
	EXPECT_NE(writeAliasedBuf, wastr.get());
	EXPECT_EQ(CString("The quick brown fox. It is running over the fields. Its tail is gleaming in the morning sun."), wastr);
	EXPECT_NE(wastr, CString(writeAliasedBuf));
	EXPECT_EQ(CString("The quick brown fox. It is running over the fields."), CString(writeAliasedBuf));

	const char* mallocdStr = "Fuck yeah, sparkle sparkle sparkle!";

	char* mallocdBuf = (char*) malloc(64);
	strcpy(mallocdBuf, mallocdStr);

	CString mstr = CString::fromCustomDelete(mallocdBuf, [](char* v) { free(v); });
	EXPECT_EQ(CString(mallocdStr), mstr);
	EXPECT_EQ(mallocdBuf, mstr.get());
	EXPECT_EQ(mallocdBuf, mstr.mget());

	mallocdBuf = (char*) malloc(64);
	strcpy(mallocdBuf, mallocdStr);

	mstr = CString::fromMalloc(mallocdBuf);
	EXPECT_EQ(CString(mallocdStr), mstr);
	EXPECT_EQ(mallocdBuf, mstr.get());
	EXPECT_EQ(mallocdBuf, mstr.mget());

	mallocdBuf = (char*) malloc(64);
	strcpy(mallocdBuf, mallocdStr);

	mstr = CString::fromCustomDelete(mallocdBuf, strlen(mallocdBuf), 64, [](char* v) { free(v); });
	EXPECT_EQ(CString(mallocdStr), mstr);
	EXPECT_EQ(mallocdBuf, mstr.get());
	EXPECT_EQ(mallocdBuf, mstr.mget());
	EXPECT_GE(63, mstr.capacity());

	mallocdBuf = (char*) malloc(64);
	strcpy(mallocdBuf, mallocdStr);

	mstr = CString::fromMalloc(mallocdBuf, strlen(mallocdBuf), 64);
	EXPECT_EQ(CString(mallocdStr), mstr);
	EXPECT_EQ(mallocdBuf, mstr.get());
	EXPECT_EQ(mallocdBuf, mstr.mget());
	EXPECT_GE(63, mstr.capacity());


	CString nstr;

	EXPECT_TRUE(nstr.isNull());
	EXPECT_NE((const char*) NULL, nstr.get());
	EXPECT_EQ(0, nstr.length());
	EXPECT_EQ(nstr, CString(""));
	nstr.append(CString("abc"));
	EXPECT_FALSE(nstr.isNull());

	CString nstr2(NULL, 0);
	EXPECT_TRUE(nstr2.isNull());
	EXPECT_NE((const char*) NULL, nstr2.get());
	EXPECT_EQ(nstr2, CString(""));


	{
		CString tstr("INSERT INTO ");
		tstr += "test ";
		tstr << "(id, text, tid) VALUES (3, 'Hallo, grausame Welt!', 7)";
		EXPECT_EQ(CString("INSERT INTO test (id, text, tid) VALUES (3, 'Hallo, grausame Welt!', 7)"), tstr);
	}

	EXPECT_EQ(CString("perfect smiles"), CString("Their perfect smiles and heartless lusts").substr(6, 14));

	{
		CString tstr(32);
		EXPECT_GE(32, tstr.capacity());

		char* d = tstr.mget();

		strcpy(d, "Hello World!");

		EXPECT_EQ(0, tstr.size());
		tstr.resize();
		EXPECT_EQ(12, tstr.size());

		CString tstr2 = tstr;

		EXPECT_EQ(tstr, tstr2);
		tstr.prepend(CString("Blah blah yabba yabba harlequin originality harr harr: "));
		EXPECT_NE(tstr, tstr2);

		tstr2 = tstr;
		EXPECT_EQ(tstr, tstr2);
		tstr2.mget()[3] = 'x';
		EXPECT_NE(tstr, tstr2);
	}

	{
		EXPECT_EQ(CString("1337"), CString().append((unsigned long) 1337));
		EXPECT_EQ(CString("1337"), CString() << (long) 1337);
		EXPECT_EQ(CString("0"), CString() << (long) 0);
		EXPECT_EQ(CString("1000"), CString() << (long) 1000);
		EXPECT_EQ(CString("10000"), CString() << (long) 10000);
		EXPECT_EQ(CString("1234"), CString() << (long) 1234);
		EXPECT_EQ(CString("12345"), CString() << (long) 12345);
		EXPECT_EQ(CString("-1234567"), CString().append((int) -1234567));
		EXPECT_EQ(CString("12D687"), CString().append(1234567, 16));
		EXPECT_EQ(CString("100101101011010000111"), CString().append(1234567, 2));
		EXPECT_EQ(CString("-42243331"), CString().append(-1234567, 6));
		EXPECT_EQ(CString("QGLJ"), CString().append(1234567, 36));
		EXPECT_EQ(CString("-NG0040B80HPI6"), CString().append(-8332950177670314940L, 29));
		EXPECT_EQ(CString("0xFFFFFFFFFFFFFFFF"), CString("0x").append(UINT64_MAX, 16));
		EXPECT_EQ(CString("-777"), CString().append(-511, 8));
	}

	{
		EXPECT_EQ(CString("Hello cruel world!"), CString("Hello world!").insert(5, " cruel"));
		EXPECT_EQ(CString("Hello world!"), CString("Hello world!").insert(5, ""));
		EXPECT_EQ(CString("Donaudampfschifffahrt"), CString("dampfschifffahrt").insert(0, "Donau"));
		EXPECT_EQ(CString("Dampfschifffahrtsgesellschaft"), CString("Dampfschifffahrt").insert(16, "sgesellschaft"));
	}

	{
		EXPECT_LT(CString("Core"), CString("Corn"));
		EXPECT_GT(CString("Hello"), CString("Hell"));
		EXPECT_LT(CString("Hello World"), CString("Yellow"));
	}

	{
		CString tstr("Test Blah Blah");

		ByteArray barr(tstr);
		EXPECT_EQ(tstr.length(), barr.length());
		EXPECT_EQ(tstr.get(), barr.get());
		barr += 'X';
		EXPECT_NE(tstr.get(), barr.get());

		tstr.reserve(64);
		ByteArray barr2(tstr);
		EXPECT_EQ(tstr.get(), barr2.get());
		barr2.append('X');
		EXPECT_NE(tstr.get(), barr2.get());
	}

	{
		CString tstr("Blablubb 32");
		tstr.resize(256);

		CString tstr2(tstr);
		EXPECT_EQ(tstr.get(), tstr2.get());

		// Even if the requested capacity is lower than the current capacity, grow() MUST at the very least trigger a COW copy.
		tstr2.grow(1);
		EXPECT_NE(tstr.get(), tstr2.get());
	}

	EXPECT_EQ(CString("123.457"), CString::fromFloatWithMaxPrecision(123.456789f, 3));
	EXPECT_EQ(CString("123.456"), CString::fromFloatWithMaxPrecision(123.456123f, 3));
	EXPECT_EQ(CString("0.15"), CString::fromFloatWithMaxPrecision(0.15, 3));
	EXPECT_EQ(CString("0.101"), CString::fromFloatWithMaxPrecision(0.10101f, 3));
	EXPECT_EQ(CString("0.101"), CString::fromFloatWithMaxPrecision(0.10101f, 4));
	EXPECT_EQ(CString("13.0"), CString::fromFloatWithMaxPrecision(13.0f, 4));
	EXPECT_EQ(CString("13.0"), CString::fromFloatWithMaxPrecision(13.00001f, 4));
	EXPECT_EQ(CString("0.0"), CString::fromFloatWithMaxPrecision(0.0f, 10));

	EXPECT_EQ(CString("Numbers: 74159 -35, Hex: 0xDEADBEEF, String: [Just a test]"),
			CString::format("Numbers: %d %d, Hex: 0x%X, String: %s", 74159, -35, 0xDEADBEEF, "[Just a test]"));
	EXPECT_EQ(CString("Just a static string"), CString::format("Just a static string"));

	{
		CString strs[] = { "eins", "zwei", "und auch drei" };
		EXPECT_EQ(CString("eins, zwei, und auch drei"), CString::join((const CString&) CString(", "), (const CString*) strs, (size_t) 3));
	}

	// See comment at AbstractSharedBuffer::joinv()
#if 0
	EXPECT_EQ(CString("eins:zwei:und auch drei"), CString::joinv(":", "eins", "zwei", "und auch drei"));
	EXPECT_EQ(CString("Just one element"), CString::joinv(", ", "Just one element"));
	EXPECT_EQ(CString("Just one element"), CString::joinv(", ", "Just one element"));
	EXPECT_EQ(CString(""), CString::joinv(", "));
	EXPECT_EQ(CString(", , "), CString::joinv(", ", "", "", ""));
	EXPECT_EQ(CString("append for dummies."), CString::joinv("", "append ", "for ", "dummies."));
#endif

	{
		const char* strs[] = { "one", "two", "and three" };
		EXPECT_EQ(CString("one, two, and three"), CString::join(", ", strs, 3));
	}

	EXPECT_TRUE(CString("Hello World").startsWith("Hello"));
	EXPECT_FALSE(CString("Hello World").startsWith("HELLO"));
	EXPECT_TRUE(CString("Hello World").startsWith("Hello World"));
	EXPECT_FALSE(CString("Hello World").startsWith("Hello World, how are you?"));
	EXPECT_TRUE(CString("Hello World").startsWith(""));
	EXPECT_TRUE(CString("").startsWith(""));

	EXPECT_TRUE(CString("Hello World").endsWith("World"));
	EXPECT_FALSE(CString("Hello World").endsWith("WORLD"));
	EXPECT_TRUE(CString("Hello World").endsWith("Hello World"));
	EXPECT_FALSE(CString("Hello World").endsWith("He said: Hello World"));
	EXPECT_TRUE(CString("Hello World").endsWith(""));
	EXPECT_TRUE(CString("").endsWith(""));

	EXPECT_EQ(0, CString("Hello World").indexOf('H'));
	EXPECT_EQ(2, CString("Hello World").indexOf('l'));
	EXPECT_EQ(2, CString("Hello World").indexOf('l', 2));
	EXPECT_EQ(3, CString("Hello World").indexOf('l', 3));
	EXPECT_EQ(9, CString("Hello World").indexOf('l', 4));
	EXPECT_EQ(5, CString("Hello World").indexOf(' '));
	EXPECT_EQ(-1, CString("Hello World").indexOf('x'));
	EXPECT_EQ(-1, CString("Hello World").indexOf('e', 2));
	EXPECT_EQ(-1, CString("").indexOf('e'));
	EXPECT_EQ(-1, CString("Hello").indexOf('o', 5));

	EXPECT_EQ(0, CString("Hello World").indexOf("Hell"));
	EXPECT_EQ(6, CString("Hello World").indexOf("Wor"));
	EXPECT_EQ(6, CString("Hello World").indexOf("W"));
	EXPECT_EQ(9, CString("Hello World").indexOf("ld"));
	EXPECT_EQ(2, CString("Hello World").indexOf("ll"));
	EXPECT_EQ(2, CString("Hello World").indexOf("ll", 2));
	EXPECT_EQ(-1, CString("Hello World").indexOf("ll", 3));
	EXPECT_EQ(-1, CString("Hello World").indexOf("Helo"));
	EXPECT_EQ(-1, CString("Hello World").indexOf("ld "));
	EXPECT_EQ(-1, CString("Hello World").indexOf("Hello World and more"));
	EXPECT_EQ(-1, CString("Hello World").indexOf("ld and yet even more"));
	EXPECT_EQ(-1, CString("Hello").indexOf("o", 5));
	EXPECT_EQ(0, CString("Hello").indexOf(""));
	EXPECT_EQ(2, CString("Hello").indexOf("", 2));

	EXPECT_EQ(vector<CString>({"eins", "zwei", "drei"}), CString("eins;zwei;drei").split(';'));
	EXPECT_EQ(vector<CString>({"eins", "zwei", "", "", "drei", "vier"}),
			CString("eins;zwei;;;drei;vier").split(';'));
	EXPECT_EQ(vector<CString>({"just one value"}), CString("just one value").split(';'));
	EXPECT_EQ(vector<CString>({"", "eins"}), CString(";eins").split(';'));
	EXPECT_EQ(vector<CString>({"eins", ""}), CString("eins;").split(';'));
	EXPECT_EQ(vector<CString>({"", ""}), CString(";").split(';'));
	EXPECT_EQ(vector<CString>({""}), CString("").split(';'));
	EXPECT_EQ(vector<CString>({"this;should", "also;work;well"}), CString("this;should also;work;well").split(' '));
}



#ifdef NXCOMMON_UNICODE_ENABLED

TEST(StringTest, UStringTest)
{
	struct CaseConvTest {
		const char16_t* src;
		const char16_t* lower;
		const char16_t* upper;
	};

	CaseConvTest tests[] = {
			{UTF16_LIT("Erlk\U000000F6nigs T\U000000F6chter am d\U000000FCstern Ort"),
                UTF16_LIT("erlk\U000000F6nigs t\U000000F6chter am d\U000000FCstern ort"),
                UTF16_LIT("ERLK\U000000D6NIGS T\U000000D6CHTER AM D\U000000DCSTERN ORT")},
			{UTF16_LIT("S\U000000F6ren a\U000000DF s\U000000E4mtliche So\U000000DFen"),
                UTF16_LIT("s\U000000F6ren a\U000000DF s\U000000E4mtliche so\U000000DFen"),
                UTF16_LIT("S\U000000D6REN ASS S\U000000C4MTLICHE SOSSEN")},
			{UTF16_LIT("\U000000E4\U000000F6\U000000FC\U000000DF\U000000C4\U000000D6\U000000DC\U000000DF"),
                UTF16_LIT("\U000000E4\U000000F6\U000000FC\U000000DF\U000000E4\U000000F6\U000000FC\U000000DF"),
                UTF16_LIT("\U000000C4\U000000D6\U000000DCSS\U000000C4\U000000D6\U000000DCSS")}
	};
	
	for (CaseConvTest& test : tests) {
		UString str((const UChar*) test.src);
		UString lstr(str);
		UString ustr(str);
		lstr.lower("de");
		ustr.upper("de");

		EXPECT_EQ(UString((const UChar*) test.lower), lstr);
		EXPECT_EQ(UString((const UChar*) test.upper), ustr);
	}


	struct TrimTest
	{
		UString src;
		UString ltrimmed;
		UString rtrimmed;
        UString trimmed;
	};

	TrimTest ttests[] = {
			{UTF16_LIT(" \t  Ey du!\n"),			    UTF16_LIT("Ey du!\n"),			        UTF16_LIT(" \t  Ey du!"),			    UTF16_LIT("Ey du!")},
			{UTF16_LIT("\r\nHallo W\U000000E4lt! \t"),	UTF16_LIT("Hallo W\U000000E4lt! \t"),	UTF16_LIT("\r\nHallo W\U000000E4lt!"),	UTF16_LIT("Hallo W\U000000E4lt!")},
			{UTF16_LIT(" A  BC DE F "),				    UTF16_LIT("A  BC DE F "),		        UTF16_LIT(" A  BC DE F"),			    UTF16_LIT("A  BC DE F")},
			{UTF16_LIT("A"),						    UTF16_LIT("A"),					        UTF16_LIT("A"),						    UTF16_LIT("A")},
			{UTF16_LIT(""),							    UTF16_LIT(""),					        UTF16_LIT(""),						    UTF16_LIT("")},
			{UTF16_LIT("\r"),						    UTF16_LIT(""),					        UTF16_LIT(""),						    UTF16_LIT("")},
			{UString(),								    UTF16_LIT(""),					        UTF16_LIT(""),						    UTF16_LIT("")},
			{UTF16_LIT("\u3000H\U000000E4y du!\u3000"),	UTF16_LIT("H\U000000E4y du!\u3000"),    UTF16_LIT("\u3000H\U000000E4y du!"),	UTF16_LIT("H\U000000E4y du!")},
			{UTF16_LIT("\u3000"),					    UTF16_LIT(""),			                UTF16_LIT(""),						    UTF16_LIT("")}
	};

	for (TrimTest& test : ttests) {
		UString ltrimmed(test.src), rtrimmed(test.src), trimmed(test.src);
		ltrimmed.ltrim();
		rtrimmed.rtrim();
		trimmed.trim();

		EXPECT_EQ(test.ltrimmed, ltrimmed);
	    EXPECT_EQ(test.rtrimmed, rtrimmed);
		EXPECT_EQ(test.trimmed, trimmed);
	}

	EXPECT_TRUE(UString(UTF16_LIT("  \r\n\t ")).isWhitespaceOnly());
	EXPECT_TRUE(UString(UTF16_LIT("")).isWhitespaceOnly());
	EXPECT_TRUE(UString().isWhitespaceOnly());
	EXPECT_FALSE(UString(UTF16_LIT("\r  .\n ")).isWhitespaceOnly());
	EXPECT_FALSE(UString(UTF16_LIT(";")).isWhitespaceOnly());
	EXPECT_TRUE(UString(UTF16_LIT("   ")).isWhitespaceOnly());
	EXPECT_TRUE(UString(UTF16_LIT(" ")).isWhitespaceOnly());
	EXPECT_TRUE(IsWhitespaceOnlyUTF8(UString().toUTF8()));
	EXPECT_TRUE(IsWhitespaceOnlyUTF8(UString(UTF16_LIT("\r\t ")).toUTF8()));
	EXPECT_FALSE(IsWhitespaceOnlyUTF8(UString(UTF16_LIT("\r\t a")).toUTF8()));


	const unsigned char utf8[] = {
			0xE3, 0x80, 0x8C, 0xE6, 0xAC, 0xB2, 0xE5, 0xB7, 0xA3, 0xE3, 0x81, 0xAB, 0x44, 0x52, 0x45, 0x41,
			0x4D, 0x42, 0x4F, 0x58, 0xE3, 0x80, 0x8D, 0xE3, 0x81, 0x82, 0xE3, 0x82, 0x8B, 0xE3, 0x81, 0x84,
			0xE3, 0x81, 0xAF, 0xE6, 0x88, 0x90, 0xE7, 0x86, 0x9F, 0xE3, 0x81, 0xAE, 0xE7, 0x90, 0x86, 0xE5,
			0xBF, 0xB5, 0xE3, 0x81, 0xA8, 0xE5, 0x86, 0xB7, 0xE3, 0x81, 0x9F, 0xE3, 0x81, 0x84, 0xE9, 0x9B,
			0xA8, 0x00
	};

	const unsigned char utf16[] = {
			0x0C, 0x30, 0x32, 0x6B, 0xE3, 0x5D, 0x6B, 0x30, 0x44, 0x00, 0x52, 0x00, 0x45, 0x00, 0x41, 0x00,
			0x4D, 0x00, 0x42, 0x00, 0x4F, 0x00, 0x58, 0x00, 0x0D, 0x30, 0x42, 0x30, 0x8B, 0x30, 0x44, 0x30,
			0x6F, 0x30, 0x10, 0x62, 0x9F, 0x71, 0x6E, 0x30, 0x06, 0x74, 0xF5, 0x5F, 0x68, 0x30, 0xB7, 0x51,
			0x5F, 0x30, 0x44, 0x30, 0xE8, 0x96, 0x00, 0x00
	};

	CString realUtf8Str((const char*) utf8, sizeof(utf8)-1);
	UString utf16Str = UString::fromUTF8(realUtf8Str);
	UString realUtf16Str((const UChar*) utf16);

	EXPECT_EQ(realUtf16Str, utf16Str);

	CString utf8Str = realUtf16Str.toUTF8();

	EXPECT_EQ(realUtf8Str, utf8Str);
}

#endif
