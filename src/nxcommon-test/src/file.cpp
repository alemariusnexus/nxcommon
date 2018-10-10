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
#include <nxcommon/file/File.h>
#include <nxcommon/file/FileException.h>






void TestFileIterator(const File& file, const CString* fnames)
{
	size_t numFnames = 0;

	while (!fnames[numFnames].isNull())
		numFnames++;

	bool* foundArr = new bool[numFnames];

	for (size_t i = 0 ; i < numFnames ; i++)
		foundArr[i] = false;

	for (File child : file.getChildren()) {
		CString fname = child.getPath().getFileName();

		bool found = false;

		for (size_t i = 0 ; i < numFnames ; i++) {
			if (fnames[i] == fname) {
				EXPECT_FALSE(foundArr[i]) << "File " << fname << " found twice in "
						<< file;

				foundArr[i] = true;
				found = true;
				break;
			}
		}

		EXPECT_TRUE(found) << "File " << fname << " found in " << file << ", but shouldn't be there!";
	}

	for (size_t i = 0 ; i < numFnames ; i++) {
		EXPECT_TRUE(foundArr[i]) << "File " << fnames[i] << " not found in " << file;
	}

	delete[] foundArr;
}



TEST(FileTest, ExistenceTest)
{
	if (!testRootPath.isNull()) {
		File testdir(testRootPath, "filetest");

		ASSERT_TRUE(testdir.exists()) << "Test directory " << testRootPath << " does not exist!";

		EXPECT_TRUE(File(testdir, "Datei 2").exists());
		EXPECT_TRUE(File(testdir, "subdir 1/fubar").exists());
		EXPECT_TRUE(File(testdir, "subdir2/sdir2/sdir3").exists());
		EXPECT_FALSE(File(testdir, "subdir2/sdir2/sdir3/sdir4").exists());
		EXPECT_FALSE(File(testdir, "subdir1").exists());

		File sdir2(testdir, "subdir2/sdir2");

		EXPECT_TRUE(sdir2.exists());
		EXPECT_TRUE(File(sdir2, "../..").exists());
		EXPECT_FALSE(File(sdir2, "../sdir3").exists());
	}
}


TEST(FileTest, HierarchyTest)
{
	if (!testRootPath.isNull()) {
		File testdir(testRootPath, "filetest");

		ASSERT_TRUE(testdir.exists()) << "Test directory " << testRootPath << " does not exist!";

		CString testdirConts[] = {"subdir 1", "file1", "Datei 2", "subdir2", "newlinetest", CString()};
		TestFileIterator(testdir, testdirConts);

		CString subdir2Conts[] = {"sdir2", "foobar", "foobar32", CString()};
		TestFileIterator(File(testdir, "subdir2"), subdir2Conts);

		EXPECT_TRUE(File(testdir, "file1").getParent() == testdir);
		EXPECT_TRUE(File(testdir, "subdir 1").getParent() == testdir);
		EXPECT_TRUE(File(testdir, "subdir 1/fubar").getParent() == File(testdir, "subdir 1"));
		EXPECT_FALSE(File(testdir, "subdir2/sdir2").getParent() == testdir);
		EXPECT_FALSE(testdir.getParent() == testdir);
		EXPECT_FALSE(testdir.getParent() == File(testdir, "subdir2"));

#ifdef _POSIX_VERSION
		EXPECT_TRUE(File("/home").getParent() == File("/"));
#endif
	}
}


TEST(FileTest, MiscTest)
{
	if (!testRootPath.isNull()) {
		File testdir(testRootPath, "filetest");

		ASSERT_TRUE(testdir.exists()) << "Test directory " << testRootPath << " does not exist!";

		struct CCTest
		{
			CString path;
			CString basePath;

			CString correctedPath;
		};

		CCTest tests[] = {
				{"SUbDiR 1",					"/",					"/subdir 1"},							// 0
				{"SUBdir 1/fUbAR",				"/",					"/subdir 1/fubar"},						// 1
				{"SDiR3/deadEnD",				"/subdir2/sdir2",		"/subdir2/sdir2/sdir3/deadend"},		// 2
				{"SDiR3/deadEnD",				"/suBDir2/sDir2",		"/subdir2/sdir2/sdir3/deadend"},		// 3
				{"SdIR2/SdIr3/doADenD",			"/subdir2",				CString()},								// 4
				{"sdir3/sdir3/deadend",			"/subdir2",				CString()}								// 5
		};

		size_t i = 0;
		for (CCTest& test : tests) {
			File base(testdir, test.basePath);
			File file(test.path);

			if (test.correctedPath.isNull()) {
				EXPECT_THROW(file.correctCase(base), FileException) << "correctCase() test #" << i
						<< " failed: Should have thrown a FileException!";
			} else {
				EXPECT_NO_THROW({
					File corrected = file.correctCase(base);

					File expFile(testdir, test.correctedPath);
					EXPECT_EQ(expFile, corrected) << "correctCase() test #" << i << " failed!";
				}) << "correctCase() test #" << i << " failed!";
			}

			i++;
		}
	}
}


TEST(FileTest, ReadWriteTest)
{
	if (!testRootPath.isNull()) {
		File testdir(testRootPath, "filetest");

		ASSERT_TRUE(testdir.exists()) << "Test directory " << testRootPath << " does not exist!";

		File newlinetestFile(testdir, "newlinetest");
		size_t newlinetestFileSize = 66;
		const char* newlinetestFileContent = "This\r\nfile\r\nuses\r\nCRLF\r\nnewlines\r\nand\r\nits\r\nlast\r\ncharacter\r\nis\r\nZ";

		EXPECT_EQ(newlinetestFileSize, newlinetestFile.getSize());

		ByteArray barr = newlinetestFile.readAll();

		// This failed in previous versions, because File::readAll() didn't take newline conversion into account
		EXPECT_EQ('Z', barr.get()[barr.length()-1]);

		// In binary mode, no CRLF conversion should happen, so the size of the read data should match the file size
		barr = newlinetestFile.readAll(ifstream::in | ifstream::binary);
		EXPECT_EQ(newlinetestFileSize, barr.length());
		EXPECT_EQ(ByteArray(newlinetestFileContent, strlen(newlinetestFileContent)), barr);
	}
}
