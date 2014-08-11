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

#include "mysqlutils.h"
#include <cstring>
#include <cstdio>



void ThrowMySQLException(MYSQL* mysql, const char* msg, const char* srcFile, int srcLine)
{
	size_t msgLen = msg ? strlen(msg) : 0;
	const char* mysqlErr = mysql_error(mysql);
	char* errmsg = new char[msgLen + strlen(mysqlErr) + 4];
	if (msg)
		sprintf(errmsg, "%s: %s", msg, mysqlErr);
	else
		sprintf(errmsg, "%s", mysqlErr);
	SQLException ex(errmsg, srcFile, srcLine);
	delete[] errmsg;
	throw ex;
}


void ThrowMySQLPreparedStatementException(MYSQL_STMT* stmt, const char* msg, const char* srcFile, int srcLine)
{
	size_t msgLen = msg ? strlen(msg) : 0;
	const char* mysqlErr = mysql_stmt_error(stmt);
	char* errmsg = new char[msgLen + strlen(mysqlErr) + 4];
	if (msg)
		sprintf(errmsg, "%s: %s", msg, mysqlErr);
	else
		sprintf(errmsg, "%s", mysqlErr);
	SQLException ex(errmsg, srcFile, srcLine);
	delete[] errmsg;
	throw ex;
}


int ConvertFromMySQLType(enum_field_types type, unsigned int flags)
{
	switch (type) {
	case MYSQL_TYPE_TINY:
	case MYSQL_TYPE_SHORT:
	case MYSQL_TYPE_INT24:
	case MYSQL_TYPE_LONG:
		if ((flags & UNSIGNED_FLAG) != 0) {
			return SQL_DATA_TYPE_UINT32;
		} else {
			return SQL_DATA_TYPE_INT32;
		}

	case MYSQL_TYPE_LONGLONG:
		if ((flags & UNSIGNED_FLAG) != 0) {
			return SQL_DATA_TYPE_UINT64;
		} else {
			return SQL_DATA_TYPE_INT64;
		}

	case MYSQL_TYPE_FLOAT:
		return SQL_DATA_TYPE_FLOAT;

	case MYSQL_TYPE_DOUBLE:
		return SQL_DATA_TYPE_DOUBLE;

	case MYSQL_TYPE_NEWDECIMAL:
	case MYSQL_TYPE_STRING:
	case MYSQL_TYPE_VAR_STRING:
	case MYSQL_TYPE_VARCHAR:
		return SQL_DATA_TYPE_STRING;

	case MYSQL_TYPE_TINY_BLOB:
	case MYSQL_TYPE_BLOB:
	case MYSQL_TYPE_MEDIUM_BLOB:
	case MYSQL_TYPE_LONG_BLOB:
		return SQL_DATA_TYPE_NULL;

	default:
		return SQL_DATA_TYPE_NULL;
	}
}
