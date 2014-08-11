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
