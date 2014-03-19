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
#include <nxcommon/ByteArray.h>




TEST(ByteArrayTest, MainByteArrayTest)
{
	const unsigned char data[] = {
			0x1F, 0x8B, 0x08, 0x00, 0xAC, 0x9B, 0x27, 0x53, 0x00, 0x03, 0x8B, 0xCA, 0xCC, 0x29, 0xC9, 0xCF, 0x4C, 0x51, 0x28, 0xC9, 0x48,
			0x55, 0xF0, 0xCF, 0xCD, 0xCB, 0x2C, 0x4E, 0xCE, 0x4C, 0xCD, 0x2B, 0x01, 0x00, 0x0F, 0xE4, 0xC8, 0xF1, 0x16, 0x00, 0x00, 0x00
	};

	ASSERT_EQ(42, sizeof(data));

	ByteArray barr((char*) data, 42);
	EXPECT_EQ(42, barr.getSize());
	EXPECT_EQ(42, barr.getCapacity());
	EXPECT_FALSE(barr.isNull());
	EXPECT_EQ(0, memcmp(data, barr.get(), 42));

	ByteArray barr2((char*) data, 42, 100);
	EXPECT_EQ(42, barr2.getSize());
	EXPECT_EQ(100, barr2.getCapacity());
	barr2.squeeze();
	EXPECT_EQ(42, barr2.getSize());
	EXPECT_EQ(42, barr2.getCapacity());
	EXPECT_EQ(barr, barr2);
}
