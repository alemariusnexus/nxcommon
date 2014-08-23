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

#ifndef PSQLUTILS_H_
#define PSQLUTILS_H_

#include <postgresql/libpq-fe.h>
#include "../SQLException.h"
#include "../../UString.h"


#define NXPSQLOID_BOOL 16
#define NXPSQLOID_BYTEA 17
#define NXPSQLOID_INT8 20
#define NXPSQLOID_INT2 21
#define NXPSQLOID_INT4 23
#define NXPSQLOID_TEXT 25
#define NXPSQLOID_FLOAT 700
#define NXPSQLOID_DOUBLE 701
#define NXPSQLOID_NUMERIC 1700


SQLException CreatePSQLConnException(PGconn* conn, const char* msg, const char* srcFile = NULL, int srcLine = -1);
SQLException CreatePSQLResultException(PGresult* res, const char* msg, const char* srcFile = NULL, int srcLine = -1);

void ThrowPSQLConnException(PGconn* conn, const char* msg, const char* srcFile = NULL, int srcLine = -1);
void ThrowPSQLResultException(PGresult* res, const char* msg, const char* srcFile = NULL, int srcLine = -1);

void FlushPSQL(PGconn* pq, timeval* timeout = NULL);
PGresult* FetchAsyncPSQLResult(PGconn* pq, timeval* timeout = NULL);


UString ConvertPlaceholdersToPSQL(const UString& query);
CString ConvertPlaceholdersToPSQLUTF8(const CString& query);

#endif /* PSQLUTILS_H_ */
