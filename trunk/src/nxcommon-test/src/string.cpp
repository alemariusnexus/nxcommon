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
#include <nxcommon/UString.h>
#include <nxcommon/strutil.h>
#include <nxcommon/unicodeutil.h>




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
	};

	IndentTest indents[] = {
			{	"  Hallo Welt!\nDieser Text sollte\n  eingerueckt werden,\n\ndenke ich!",
				"  Hallo Welt!\n  Dieser Text sollte\n    eingerueckt werden,\n  \n  denke ich!",
				"  "
			},
			{	"",
				"",
				"Franz"
			},
			{	"\n",
				"\nHallo Welt!",
				"Hallo Welt!"
			}
	};

	for (unsigned int i = 0 ; i < sizeof(indents) / sizeof(IndentTest) ; i++) {
		IndentTest& t = indents[i];
		char* indRes = indent(t.src, t.ind);
		EXPECT_STREQ(t.dst, indRes) << "indent() test #" << i << " failed!";
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
	strcpy(tstr1, "Hallo grausame Welt!");
	EXPECT_EQ(CString("Hallo schoene Welt!"), cstr1);
	delete[] tstr1;

	char* tstr2 = new char[64];
	strcpy(tstr2, "Hallo schoene Welt!");
	CString cstr2 = CString::from(tstr2);
	EXPECT_EQ(CString("Hallo schoene Welt!"), cstr2);
	strcpy(tstr2, "Hallo grausame Welt!");
	EXPECT_EQ(CString("Hallo grausame Welt!"), cstr2);

	EXPECT_EQ(CString(NULL), CString());
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
	cstr2[0] = 'H';
	cstr2[6] = 'W';
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
	cstr1[6] = 'G';
	cstr1[9] = 'd';
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
	wastr.append(CString(" Its tail is gleaming in the morning sun."));
	EXPECT_NE(writeAliasedBuf, wastr.get());
	EXPECT_EQ(CString("The quick brown fox. It is running over the fields. Its tail is gleaming in the morning sun."), wastr);
	EXPECT_NE(wastr, CString(writeAliasedBuf));
	EXPECT_EQ(CString("The quick brown fox. It is running over the fields."), CString(writeAliasedBuf));

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
}



TEST(StringTest, UStringTest)
{
	struct CaseConvTest {
		const char16_t* src;
		const char16_t* lower;
		const char16_t* upper;
	};

	CaseConvTest tests[] = {
			{u"Erlkönigs Töchter am düstern Ort",		u"erlkönigs töchter am düstern ort",	u"ERLKÖNIGS TÖCHTER AM DÜSTERN ORT"},
			{u"Sören aß sämtliche Soßen",				u"sören aß sämtliche soßen",			u"SÖREN ASS SÄMTLICHE SOSSEN"},
			{u"äöüßÄÖÜß",								u"äöüßäöüß",							u"ÄÖÜSSÄÖÜSS"}
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
			{u" \t  Ey du!\n",					u"Ey du!\n",					u" \t  Ey du!",				u"Ey du!"},
			{u"\r\nHallo Wält! \t",				u"Hallo Wält! \t",				u"\r\nHallo Wält!",			u"Hallo Wält!"},
			{u" A  BC DE F ",					u"A  BC DE F ",					u" A  BC DE F",			u"A  BC DE F"},
			{u"A",								u"A",							u"A",						u"A"},
			{u"",								u"",							u"",						u""},
			{u"\r",								u"",							u"",						u""},
			{UString(),							u"",							u"",						u""},
			{u"\u3000Häy du!\u3000",			u"Häy du!\u3000",				u"\u3000Häy du!",			u"Häy du!"},
			{u"\u3000",							u"",							u"",						u""}
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

	EXPECT_TRUE(UString(u"  \r\n\t ").isWhitespaceOnly());
	EXPECT_TRUE(UString(u"").isWhitespaceOnly());
	EXPECT_TRUE(UString().isWhitespaceOnly());
	EXPECT_FALSE(UString(u"\r  .\n ").isWhitespaceOnly());
	EXPECT_FALSE(UString(u";").isWhitespaceOnly());
	EXPECT_TRUE(UString(u"   ").isWhitespaceOnly());
	EXPECT_TRUE(UString(u" ").isWhitespaceOnly());
	EXPECT_TRUE(IsWhitespaceOnlyUTF8(UString().toUTF8()));
	EXPECT_TRUE(IsWhitespaceOnlyUTF8(UString(u"\r\t ").toUTF8()));
	EXPECT_FALSE(IsWhitespaceOnlyUTF8(UString(u"\r\t a").toUTF8()));


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

	ByteArray realUtf8Str((const char*) utf8, sizeof(utf8)-1);
	UString utf16Str = UString::fromUTF8(realUtf8Str);
	UString realUtf16Str((const UChar*) utf16);

	EXPECT_EQ(realUtf16Str, utf16Str);

	ByteArray utf8Str = realUtf16Str.toUTF8();

	EXPECT_EQ(realUtf8Str, utf8Str);
}