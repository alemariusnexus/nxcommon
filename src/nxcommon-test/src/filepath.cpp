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
#include <nxcommon/file/FilePath.h>



TEST(FilePathTest, NormalizationTest)
{
	struct NormTest
	{
		CString path;
		FilePath::Syntax syntax;
		CString normalized;
	};

	NormTest tests[] = {
			{"/home/alemariusnexus",					FilePath::Unix,		"/home/alemariusnexus"},					// 0
			{"///home//alemariusnexus///",				FilePath::Unix,		"/home/alemariusnexus"},					// 1
			{"/ ",										FilePath::Unix,		"/ "},										// 2
			{"///",										FilePath::Unix,		"/"},										// 3
			{"/",										FilePath::Unix,		"/"},										// 4
			{"/",										FilePath::Windows,	"."},										// 5
			{"",										FilePath::Windows,	"."},										// 6
			{"C:\\\\Prog Files\\test/hey///du\\bla\\",	FilePath::Windows,	"C:/Prog Files/test/hey/du/bla"},			// 7
			{"C:\\\\Prog Files\\test/hey///du\\bla\\",	FilePath::Unix,		"C:\\\\Prog Files\\test/hey/du\\bla\\"},	// 8
			{"C:\\",									FilePath::Windows,	"C:/"},										// 9
			{"C:\\",									FilePath::Unix,		"C:\\"},									// 10
			{"C:/",										FilePath::Unix,		"C:"},										// 11

#ifdef _POSIX_VERSION
			{"C:/",										FilePath::System,	"C:"},										// 12
			{"C:/hallo\\Welt//",						FilePath::System,	"C:/hallo\\Welt"},							// 13
			{"/",										FilePath::System,	"/"},										// 14
#elif defined(_WIN32)
			{"C:/",										FilePath::System,	"C:/"},										// 12
			{"C:/hallo\\Welt//",						FilePath::System,	"C:/hallo/Welt"},							// 13
			{"/",										FilePath::System,	"."},										// 14
#endif
	};

	for (size_t i = 0 ; i < sizeof(tests) / sizeof(NormTest) ; i++) {
		NormTest& test = tests[i];

		FilePath path(test.path, test.syntax);

		EXPECT_EQ(test.normalized, path.toString()) << "Normalization test #" << i << " failed!";
	}
}


TEST(FilePathTest, PathComponentsTest)
{
	struct ComponentTest
	{
		CString path;
		FilePath::Syntax syntax;

		CString ext;
		CString fullExt;
		CString fname;
		CString baseFname;
		CString fullBaseFname;
		CString dirpath;
	};

	ComponentTest tests[] = {
			{"/home/alemariusnexus/test.tar.gz",		FilePath::Unix,		"gz",	"tar.gz",	"test.tar.gz",	"test",		"test.tar",		"/home/alemariusnexus"},		// 0
			{"/home/alemariusnexus/test",				FilePath::Unix,		"",		"",			"test",			"test",		"test",			"/home/alemariusnexus"},		// 1
			{"/home/bla.tst.dir///",					FilePath::Unix,		"dir",	"tst.dir",	"bla.tst.dir",	"bla",		"bla.tst",		"/home"},						// 2
			{"test.mp3",								FilePath::Unix,		"mp3",	"mp3",		"test.mp3",		"test",		"test",			"."},							// 3
			{"C:/bla.tgz",								FilePath::Windows,	"tgz",	"tgz",		"bla.tgz",		"bla",		"bla",			"C:/"},							// 4
			{"C:/",										FilePath::Windows,	"",		"",			"",				"",			"",				"C:/"},							// 5
			{"/home/alemariusnexus/test.",				FilePath::Unix,		"",		"",			"test.",		"test",		"test",			"/home/alemariusnexus"},		// 6
			{"C:/Program Files/../.",					FilePath::Windows,	"",		"",			".",			"",			"",				"C:/Program Files/.."},			// 7
			{"C:/Program Files/.mp4",					FilePath::Windows,	"mp4",	"mp4",		".mp4",			"",			"",				"C:/Program Files"},			// 8
			{".S",										FilePath::Unix,		"S",	"S",		".S",			"",			"",				"."},							// 9
			{"C:/test.dir/file.tar.bz2",				FilePath::Windows,	"bz2",	"tar.bz2",	"file.tar.bz2",	"file",		"file.tar",		"C:/test.dir"},					// 10
			{"/",										FilePath::Unix,		"",		"",			"/",			"/",		"/",			"/"},							// 11
			{"/",										FilePath::Windows,	"",		"",			".",			"",			"",				"."},							// 12
			{"C:/tdir\\hi.gz",							FilePath::Windows,	"gz",	"gz",		"hi.gz",		"hi",		"hi",			"C:/tdir"},
			{"C:/tdir\\hi.gz",							FilePath::Unix,		"gz",	"gz",		"tdir\\hi.gz",	"tdir\\hi",	"tdir\\hi",		"C:"},
	};

	for (size_t i = 0 ; i < sizeof(tests) / sizeof(ComponentTest) ; i++) {
		ComponentTest& test = tests[i];

		FilePath path(test.path, test.syntax);

		EXPECT_EQ(test.ext, path.getExtension()) << "getExtension() test #" << i << " failed!";
		EXPECT_EQ(test.fullExt, path.getFullExtension()) << "getFullExtension() test #" << i << " failed!";
		EXPECT_EQ(test.fname, path.getFileName()) << "getFileName() test #" << i << " failed!";
		EXPECT_EQ(test.baseFname, path.getBaseFileName()) << "getBaseFileName() test #" << i << " failed!";
		EXPECT_EQ(test.fullBaseFname, path.getFullBaseFileName()) << "getFullBaseFileName() test #" << i << " failed!";
		EXPECT_EQ(test.dirpath, path.getDirectoryPath().toString()) << "getDirectoryPath() test #" << i << " failed!";
	}
}


TEST(FilePathTest, BoolPropsTest)
{
	struct BPTest
	{
		CString path;
		FilePath::Syntax syntax;

		bool absolute;
		bool root;
	};

	BPTest tests[] = {
			//	Test Path							Syntax				Abs		Root

			{"/home/alemariusnexus/workspace",		FilePath::Unix,		true,	false},		// 0
			{"/home/alemariusnexus/workspace",		FilePath::Windows,	false,	false},		// 1
			{"C:/Program Files",					FilePath::Windows,	true,	false},		// 2
			{"C:/Program Files",					FilePath::Unix,		false,	false},		// 3
			{"test/bla.tgz",						FilePath::Unix,		false,	false},		// 4
			{"/",									FilePath::Unix,		true,	true},		// 5
			{"/",									FilePath::Windows,	false,	false},		// 6
			{"C:\\",								FilePath::Windows,	true,	true},		// 7
			{"",									FilePath::Unix,		false,	false},		// 8
			{".",									FilePath::Windows,	false,	false}		// 9
	};

	for (size_t i = 0 ; i < sizeof(tests) / sizeof(BPTest) ; i++) {
		BPTest& test = tests[i];

		FilePath path(test.path, test.syntax);

		if (test.absolute) {
			EXPECT_TRUE(path.isAbsolute()) << "isAbsolute() test #" << i << " failed!";
			EXPECT_FALSE(path.isRelative()) << "isRelative() test #" << i << " failed!";
		} else {
			EXPECT_FALSE(path.isAbsolute()) << "isAbsolute() test #" << i << " failed!";
			EXPECT_TRUE(path.isRelative()) << "isRelative() test #" << i << " failed!";
		}

		if (test.root)
			EXPECT_TRUE(path.isRoot()) << "isRoot() test #" << i << " failed!";
		else
			EXPECT_FALSE(path.isRoot()) << "isRoot() test #" << i << " failed!";
	}
}


TEST(FilePathTest, GetAbsolutePathTest)
{
	struct APTest
	{
		CString path;
		FilePath::Syntax syntax;

		CString cdir;
		CString absPath;
	};

	APTest tests[] = {
			{"data/maps//",					FilePath::Unix,		"/home/alemariusnexus",				"/home/alemariusnexus/data/maps"},		// 0
			{"/data",						FilePath::Unix,		"/home",							"/data"},								// 1
			{"..",							FilePath::Windows,	"C:\\blabla",						"C:/blabla/.."},						// 2
			{"data",						FilePath::Windows,	"Rockstar Games//",					"Rockstar Games/data"},					// 3
			{"C:\\Program Files",			FilePath::Windows,	"D:/root",							"C:/Program Files"}						// 4
	};

	for (size_t i = 0 ; i < sizeof(tests) / sizeof(APTest) ; i++) {
		APTest& test = tests[i];

		FilePath path(test.path, test.syntax);

		FilePath apath = path.getAbsolutePath(FilePath(test.cdir, test.syntax));

		EXPECT_EQ(FilePath(test.absPath, test.syntax), apath) << "getAbsolutePath() test #" << i << " failed!";
	}
}


TEST(FilePathTest, MiscTest)
{
	FilePath fpath("/home/alemariusnexus");

#ifdef _POSIX_VERSION
	EXPECT_EQ(FilePath::Unix, fpath.getSyntax());
#elif defined(_WIN32)
	EXPECT_EQ(FilePath::Windows, fpath.getSyntax());
#endif
}

