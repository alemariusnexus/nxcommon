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

#include "MySQLPreparedStatementImpl.h"
#include "MySQLPreparedStatementResultImpl.h"
#include "MySQLPreparedStatementResultInstanceImpl.h"
#include "sqlbase.h"
#include "mysqlutils.h"







MySQLPreparedStatementImpl::MySQLPreparedStatementImpl(MySQLDatabaseImpl* db)
		: db(db), curRes(NULL)
{
}


MySQLPreparedStatementImpl::~MySQLPreparedStatementImpl()
{
	delete curRes;
}


void MySQLPreparedStatementImpl::bindUInt32(size_t index, uint32_t value)
{
	inFields[index].num.setUInt32(value);
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_LONG;
	b->buffer = &inFields[index].num.ui32;
	b->is_unsigned = 1;
}


void MySQLPreparedStatementImpl::bindInt32(size_t index, int32_t value)
{
	inFields[index].num.setInt32(value);
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_LONG;
	b->buffer = &inFields[index].num.i32;
	b->is_unsigned = 0;
}


void MySQLPreparedStatementImpl::bindUInt64(size_t index, uint64_t value)
{
	inFields[index].num.setUInt64(value);
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_LONGLONG;
	b->buffer = &inFields[index].num.ui64;
	b->is_unsigned = 1;
}


void MySQLPreparedStatementImpl::bindInt64(size_t index, int64_t value)
{
	inFields[index].num.setInt64(value);
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_LONGLONG;
	b->buffer = &inFields[index].num.i64;
	b->is_unsigned = 0;
}


void MySQLPreparedStatementImpl::bindFloat(size_t index, float value)
{
	inFields[index].num.setFloat(value);
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_FLOAT;
	b->buffer = &inFields[index].num.f;
	b->is_unsigned = 0;
}


void MySQLPreparedStatementImpl::bindDouble(size_t index, double value)
{
	inFields[index].num.setDouble(value);
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_DOUBLE;
	b->buffer = &inFields[index].num.d;
	b->is_unsigned = 0;
}


void MySQLPreparedStatementImpl::bindString(size_t index, const UString& value)
{
	bindStringUTF8(index, value.toUTF8());
}


void MySQLPreparedStatementImpl::bindStringUTF8(size_t index, const CString& value)
{
	inFields[index].barr = value;
	inFields[index].barrLen = value.length();
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_STRING;
	b->buffer = (char*) value.get();
	b->buffer_length = value.length();
	b->length = &inFields[index].barrLen;
}


void MySQLPreparedStatementImpl::bindBLOB(size_t index, const ByteArray& value)
{
	inFields[index].barr = value;
	inFields[index].barrLen = value.length();
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_BLOB;
	b->buffer = (char*) value.get();
	b->buffer_length = value.length();
	b->length = &inFields[index].barrLen;
}


void MySQLPreparedStatementImpl::bindNull(size_t index)
{
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_NULL;
}


void MySQLPreparedStatementImpl::bindBool(size_t index, bool value)
{
	inFields[index].num.setUInt8(value ? 1 : 0);
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_TINY;
	b->buffer = &inFields[index].num.ui8;
	b->is_unsigned = 1;
}


void MySQLPreparedStatementImpl::prepare(const UString& query)
{
	prepareUTF8(query.toUTF8());
}


void MySQLPreparedStatementImpl::prepareUTF8(const CString& query)
{
	delete curRes;
	curRes = NULL;

	MYSQL_STMT* stmt = mysql_stmt_init(db->getMySQLHandle());

	if (mysql_stmt_prepare(stmt, query.get(), query.length()) != 0) {
		ThrowMySQLException(db->getMySQLHandle(), "Error preparing MySQL statement", __FILE__, __LINE__);
	}

	unsigned int numParams = mysql_stmt_param_count(stmt);

	inBinds = new MYSQL_BIND[numParams];
	memset(inBinds, 0, numParams*sizeof(MYSQL_BIND));

	inFields = new Field[numParams];

	curRes = new MySQLPreparedStatementResultImpl(this, stmt);
}


SQLResultImpl* MySQLPreparedStatementImpl::execute()
{
	if (mysql_stmt_bind_param(curRes->stmt, inBinds) != 0) {
		ThrowMySQLPreparedStatementException(curRes->stmt, "Error binding parameters of MySQL prepared statement", __FILE__, __LINE__);
	}

	if (mysql_stmt_execute(curRes->stmt) != 0) {
		ThrowMySQLPreparedStatementException(curRes->stmt, "Error executing MySQL prepared statement", __FILE__, __LINE__);
	}

	curRes->executed();

	return new MySQLPreparedStatementResultInstanceImpl(curRes);
}


void MySQLPreparedStatementImpl::reset()
{
	curRes->reset();
}


void MySQLPreparedStatementImpl::finalize()
{
	delete[] inFields;
	delete[] inBinds;
}


