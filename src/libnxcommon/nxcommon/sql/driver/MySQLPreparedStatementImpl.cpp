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
#include "mysqlutils.h"


#define MAX_BLOB_FETCH_IN_ADVANCE_SIZE 8192






MySQLPreparedStatementImpl::MySQLPreparedStatementImpl(MySQLDatabaseImpl* db)
		: db(db)
{
}


MySQLPreparedStatementImpl::~MySQLPreparedStatementImpl()
{

}


void MySQLPreparedStatementImpl::bindUInt32(size_t index, uint32_t value)
{
	inFields[index].num.ui32 = value;
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_LONG;
	b->buffer = &inFields[index].num.ui32;
	b->is_unsigned = 1;
}


void MySQLPreparedStatementImpl::bindInt32(size_t index, int32_t value)
{
	inFields[index].num.i32 = value;
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_LONG;
	b->buffer = &inFields[index].num.i32;
	b->is_unsigned = 0;
}


void MySQLPreparedStatementImpl::bindUInt64(size_t index, uint64_t value)
{
	inFields[index].num.ui64 = value;
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_LONGLONG;
	b->buffer = &inFields[index].num.ui64;
	b->is_unsigned = 1;
}


void MySQLPreparedStatementImpl::bindInt64(size_t index, int64_t value)
{
	inFields[index].num.i64 = value;
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_LONGLONG;
	b->buffer = &inFields[index].num.i64;
	b->is_unsigned = 0;
}


void MySQLPreparedStatementImpl::bindFloat(size_t index, float value)
{
	inFields[index].num.f = value;
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_FLOAT;
	b->buffer = &inFields[index].num.f;
	b->is_unsigned = 0;
}


void MySQLPreparedStatementImpl::bindDouble(size_t index, double value)
{
	inFields[index].num.d = value;
	MYSQL_BIND* b = inBinds+index;
	b->buffer_type = MYSQL_TYPE_DOUBLE;
	b->buffer = &inFields[index].num.d;
	b->is_unsigned = 0;
}


void MySQLPreparedStatementImpl::bindString(size_t index, const UString& value)
{
	bindStringUTF8(index, value.toUTF8());
}


void MySQLPreparedStatementImpl::bindStringUTF8(size_t index, const ByteArray& value)
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


void MySQLPreparedStatementImpl::prepare(const UString& query)
{
	prepareUTF8(query.toUTF8());
}


void MySQLPreparedStatementImpl::prepareUTF8(const ByteArray& query)
{
	stmt = mysql_stmt_init(db->getMySQLHandle());

	if (mysql_stmt_prepare(stmt, query.get(), query.length()) != 0) {
		ThrowMySQLException(db->getMySQLHandle(), "Error preparing MySQL statement", __FILE__, __LINE__);
	}

	unsigned int numParams = mysql_stmt_param_count(stmt);
	numFields = mysql_stmt_field_count(stmt);

	inBinds = new MYSQL_BIND[numParams];
	memset(inBinds, 0, numParams*sizeof(MYSQL_BIND));

	inFields = new Field[numParams];

	outBinds = new MYSQL_BIND[numFields];
	memset(outBinds, 0, numFields*sizeof(MYSQL_BIND));

	outFields = new OutField[numFields];

	blobIndices = new unsigned int[numFields];
	numBlobIndices = 0;

	MYSQL_RES* meta = mysql_stmt_result_metadata(stmt);

	for (unsigned int i = 0 ; i < numFields ; i++) {
		MYSQL_FIELD* field = mysql_fetch_field(meta);
		MYSQL_BIND& bind = outBinds[i];
		OutField& vfield = outFields[i];

		bind.buffer_type = field->type;
		bind.is_null = &vfield.isnull;
		vfield.type = field->type;

		switch (field->type) {
		case MYSQL_TYPE_TINY_BLOB:
			vfield.barr = ByteArray::from(new char[256], 256);
			bind.buffer = vfield.barr.getMutable();
			bind.buffer_length = 256;
			bind.length = &vfield.barrLen;
			blobIndices[numBlobIndices++] = i;
			break;

		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VAR_STRING:
		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_BIT:
			if (field->length <= MAX_BLOB_FETCH_IN_ADVANCE_SIZE) {
				vfield.barrLen = field->length;
				vfield.barr = ByteArray::from(new char[field->length], field->length);
				bind.buffer = vfield.barr.getMutable();
				bind.buffer_length = field->length;
				bind.length = &vfield.barrLen;
			} else {
				// See comment at dummyBuffer member.
				bind.buffer = dummyBuffer;
				bind.buffer_length = 1;
				bind.length = &vfield.barrLen;
			}
			blobIndices[numBlobIndices++] = i;
			break;

		default:
			bind.buffer = &vfield.num;
			bind.length = &vfield.barrLen;
			break;
		}
	}

	mysql_free_result(meta);
}


void MySQLPreparedStatementImpl::execute()
{
	if (mysql_stmt_bind_param(stmt, inBinds) != 0) {
		ThrowMySQLPreparedStatementException(stmt, "Error binding parameters of MySQL prepared statement", __FILE__, __LINE__);
	}

	if (mysql_stmt_execute(stmt) != 0) {
		ThrowMySQLPreparedStatementException(stmt, "Error executing MySQL prepared statement", __FILE__, __LINE__);
	}

	numAffectedRows = mysql_stmt_affected_rows(stmt);

	if (numFields != 0) {
		if (mysql_stmt_bind_result(stmt, outBinds) != 0) {
			ThrowMySQLPreparedStatementException(stmt, "Error binding results of MySQL prepared statement", __FILE__, __LINE__);
		}
	}
}


void MySQLPreparedStatementImpl::reset()
{

}


void MySQLPreparedStatementImpl::finalize()
{
	delete[] blobIndices;
	delete[] outFields;
	delete[] outBinds;
	delete[] inFields;
	delete[] inBinds;

	mysql_stmt_close(stmt);
}


bool MySQLPreparedStatementImpl::nextRecord()
{
	int res = mysql_stmt_fetch(stmt);

	if (res == MYSQL_NO_DATA) {
		return false;
	} else if (res == 0  ||  res == MYSQL_DATA_TRUNCATED) {
		for (unsigned int i = 0 ; i < numBlobIndices ; i++) {
			unsigned int idx = blobIndices[i];
			MYSQL_BIND* bind = outBinds+idx;
			OutField& field = outFields[idx];

			if (bind->buffer != dummyBuffer) {
				// A buffer with a guessed size.

				field.barr.setSize(*bind->length);
			} else {
				// A variable-length buffer with a length range too big to just allocate on spec.

				bind->buffer = new char[*bind->length];
				bind->buffer_length = *bind->length;
				field.barr = ByteArray::from((char*) bind->buffer, *bind->length);

				if (mysql_stmt_fetch_column(stmt, bind, idx, 0) != 0) {
					ThrowMySQLException(db->getMySQLHandle(), "Error fetching result of MySQL prepared statement", __FILE__, __LINE__);
				}

				// See comment at dummyBuffer member.
				bind->buffer = dummyBuffer;
				bind->buffer_length = 1;
			}
		}

		return true;
	} else {
		printf("Res: %d\n", res);
		ThrowMySQLException(db->getMySQLHandle(), "Error fetching from MySQL prepared statement", __FILE__, __LINE__);
	}

	return false;
}


uint32_t MySQLPreparedStatementImpl::getUInt32(size_t index) const
{
	return outFields[index].num.ui32;
}


int32_t MySQLPreparedStatementImpl::getInt32(size_t index) const
{
	return outFields[index].num.i32;
}


uint64_t MySQLPreparedStatementImpl::getUInt64(size_t index) const
{
	return outFields[index].num.ui64;
}


int64_t MySQLPreparedStatementImpl::getInt64(size_t index) const
{
	return outFields[index].num.i64;
}


float MySQLPreparedStatementImpl::getFloat(size_t index) const
{
	return outFields[index].num.f;
}


double MySQLPreparedStatementImpl::getDouble(size_t index) const
{
	return outFields[index].num.d;
}


ByteArray MySQLPreparedStatementImpl::getBLOB(size_t index) const
{
	return outFields[index].barr;
}


UString MySQLPreparedStatementImpl::getString(size_t index) const
{
	return UString::fromUTF8(getStringUTF8(index));
}


ByteArray MySQLPreparedStatementImpl::getStringUTF8(size_t index) const
{
	return outFields[index].barr;
}


uint64_t MySQLPreparedStatementImpl::getAffectedRowCount() const
{
	return numAffectedRows;
}


