/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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
#include <nxcommon/util.h>
#include <nxcommon/stream/RangedStream.h>
#include <fstream>

using std::ifstream;



TEST(UtilTest, TestVariantNum)
{
	VariantNum num;

	EXPECT_EQ(0, num.ui64);
	EXPECT_EQ(0, num.i64);
	EXPECT_EQ(0, num.ui32);
	EXPECT_EQ(0, num.i32);
	EXPECT_EQ(0, num.ui16);
	EXPECT_EQ(0, num.i16);
	EXPECT_EQ(0, num.ui8);
	EXPECT_EQ(0, num.i8);

	num.i32 = -1337;
	num.setFromInt32();

	EXPECT_EQ(-1337, num.i32);
	EXPECT_EQ(-1337, num.i64);
	EXPECT_EQ(-1337, num.i16);

	num.setInt16(1339);

	EXPECT_EQ(1339, num.ui64);
	EXPECT_EQ(1339, num.i64);
	EXPECT_EQ(1339, num.ui32);
	EXPECT_EQ(1339, num.i32);
	EXPECT_EQ(1339, num.ui16);
	EXPECT_EQ(1339, num.i16);
}


TEST(UtilTest, TestStreams)
{
	if (!testRootPath.isNull()) {
		ifstream fin(CString(testRootPath).append("/streamtest.bin").get(), ifstream::binary);
		fin.seekg(5, ifstream::beg);

		RangedStream<istream> in(&fin, 10);

		in.exceptions(ifstream::failbit | ifstream::badbit | ifstream::eofbit);

		EXPECT_EQ(0, in.tellg());
		EXPECT_EQ(5, fin.tellg());

		char buf[32];

		char expected1[] = {0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC};

		in.read(buf, 8);

		EXPECT_EQ(8, in.gcount());
		EXPECT_EQ(0, memcmp(buf, expected1, 8));
		EXPECT_EQ(8, in.tellg());
		EXPECT_EQ(13, fin.tellg());

		EXPECT_NO_THROW({ in.seekg(7, istream::beg); });
		EXPECT_EQ(7, in.tellg());

		EXPECT_NO_THROW({ in.seekg(2, istream::cur); });
		EXPECT_EQ(9, in.tellg());

		EXPECT_NO_THROW({ in.seekg(-4, istream::end); });
		EXPECT_EQ(6, in.tellg());

		EXPECT_NO_THROW({ in.seekg(4, istream::cur); });
		EXPECT_EQ(10, in.tellg());

		EXPECT_NO_THROW({ in.seekg(10, istream::beg); });
		EXPECT_EQ(10, in.tellg());

		EXPECT_NO_THROW({ in.seekg(0, istream::end); });
		EXPECT_EQ(10, in.tellg());

		EXPECT_NO_THROW({ in.seekg(-6, istream::cur); });
		EXPECT_EQ(4, in.tellg());

		char expected2[] = {0x9, 0xA, 0xB, 0xC, 0xD, 0xE};

		EXPECT_THROW({ in.read(buf, 15); }, ios_base::failure); // Should throw because of EOF
		EXPECT_NE(ios_base::goodbit, in.rdstate());

		in.clear();

		EXPECT_EQ(6, in.gcount());
		EXPECT_EQ(0, memcmp(buf, expected2, 6));
		EXPECT_EQ(10, in.tellg());
		EXPECT_EQ(15, fin.tellg());

	}
}
