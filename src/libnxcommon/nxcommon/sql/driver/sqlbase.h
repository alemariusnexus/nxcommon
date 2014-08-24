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

#ifndef SQLBASE_H_
#define SQLBASE_H_

#include <nxcommon/config.h>



#define SQL_DATA_TYPE_NULL 0
#define SQL_DATA_TYPE_INT32 1
#define SQL_DATA_TYPE_UINT32 2
#define SQL_DATA_TYPE_INT64 3
#define SQL_DATA_TYPE_UINT64 4
#define SQL_DATA_TYPE_FLOAT 5
#define SQL_DATA_TYPE_DOUBLE 6
#define SQL_DATA_TYPE_BLOB 7
#define SQL_DATA_TYPE_STRING 8




#define SQL_CAP_LAST_INSERT_ID				0x00000001
#define SQL_CAP_TIMEOUT						0x00000002


#endif /* SQLBASE_H_ */
