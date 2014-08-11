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

#include "MySQLPreparedStatementResultImpl.h"
#include "mysqlutils.h"
#include "../../ByteArray.h"
#include <utility>

using std::pair;



#define MAX_BLOB_FETCH_IN_ADVANCE_SIZE 8192





MySQLPreparedStatementResultImpl::MySQLPreparedStatementResultImpl(
		MySQLPreparedStatementImpl* pstmt, MYSQL_STMT* stmt)
		: pstmt(pstmt), db(pstmt->db), stmt(stmt)
{
	numFields = mysql_stmt_field_count(stmt);

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
		vfield.name = field->name;
		vfield.type = field->type;
		vfield.flags = field->flags;
		vfield.isInt = false;
		vfield.isFloat = false;
		vfield.num.ui64 = 0;

		fieldIndexByNameMap.insert(pair<CString, int>(CString(vfield.name).lower(), i));

		// TODO: Maybe make BLOB the default for unknown data types
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

		case MYSQL_TYPE_FLOAT:
		case MYSQL_TYPE_DOUBLE:
			bind.length = &vfield.barrLen;
			vfield.isFloat = true;

			if (field->type == MYSQL_TYPE_FLOAT) {
				bind.buffer = &vfield.num.f;
				vfield.numSetFromMethod = &VariantNum::setFromFloat;
			} else {
				bind.buffer = &vfield.num.d;
				vfield.numSetFromMethod = &VariantNum::setFromDouble;
			}
			break;

		default:
			bind.length = &vfield.barrLen;
			bind.is_unsigned = (field->flags & UNSIGNED_FLAG) != 0;
			vfield.iunsigned = bind.is_unsigned;
			vfield.isInt = true;

			if ((field->flags & UNSIGNED_FLAG) != 0) {
				bind.buffer = &vfield.num.ui64;
				vfield.numSetFromMethod = &VariantNum::setFromUInt;
			} else if (field->type == MYSQL_TYPE_TINY) {
				bind.buffer = &vfield.num.i8;
				vfield.numSetFromMethod = &VariantNum::setFromInt8;
			} else if (field->type == MYSQL_TYPE_SHORT) {
				bind.buffer = &vfield.num.i16;
				vfield.numSetFromMethod = &VariantNum::setFromInt16;
			} else if (field->type == MYSQL_TYPE_LONG) {
				bind.buffer = &vfield.num.i32;
				vfield.numSetFromMethod = &VariantNum::setFromInt32;
			} else {
				bind.buffer = &vfield.num.i64;
				vfield.numSetFromMethod = &VariantNum::setFromInt64;
			}

			break;
		}
	}

	mysql_free_result(meta);
}

MySQLPreparedStatementResultImpl::~MySQLPreparedStatementResultImpl()
{
	delete[] blobIndices;
	delete[] outFields;
	delete[] outBinds;

	mysql_stmt_close(stmt);
}


void MySQLPreparedStatementResultImpl::reset()
{
	if (mysql_stmt_free_result(stmt) != 0) {
		ThrowMySQLPreparedStatementException(stmt, "Error freeing result of MySQL prepared statement", __FILE__, __LINE__);
	}
}


void MySQLPreparedStatementResultImpl::executed()
{
	numAffectedRows = mysql_stmt_affected_rows(stmt);

	if (numFields != 0) {
		if (mysql_stmt_bind_result(stmt, outBinds) != 0) {
			ThrowMySQLPreparedStatementException(stmt, "Error binding results of MySQL prepared statement", __FILE__, __LINE__);
		}

		if (mysql_stmt_store_result(stmt) != 0) {
			ThrowMySQLPreparedStatementException(stmt, "Error storing result of MySQL prepared statement", __FILE__, __LINE__);
		}
	}
}


bool MySQLPreparedStatementResultImpl::nextRecord()
{
	int res = mysql_stmt_fetch(stmt);

	if (res == MYSQL_NO_DATA) {
		return false;
	} else if (res == 0  ||  res == MYSQL_DATA_TRUNCATED) {
		for (unsigned int i = 0 ; i < numFields ; i++) {
			if (outFields[i].isInt  ||  outFields[i].isFloat) {
				OutField& field = outFields[i];

				void (VariantNum::*ptr)() = field.numSetFromMethod;
				(field.num.*ptr)();
			}
		}

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


uint32_t MySQLPreparedStatementResultImpl::getUInt32(int index, bool* success) const
{
	const OutField& of = outFields[index];

	if (of.isInt  ||  of.isFloat) {
		if (success) {
			*success = true;
		}

		return of.num.ui32;
	} else {
		return StringToUInt32(of.barr.get(), of.barrLen, 0, success);
	}
}


int32_t MySQLPreparedStatementResultImpl::getInt32(int index, bool* success) const
{
	const OutField& of = outFields[index];

	if (of.isInt  ||  of.isFloat) {
		if (success) {
			*success = true;
		}

		return of.num.i32;
	} else {
		return StringToInt32(of.barr.get(), of.barrLen, 0, success);
	}
}


uint64_t MySQLPreparedStatementResultImpl::getUInt64(int index, bool* success) const
{
	const OutField& of = outFields[index];

	if (of.isInt  ||  of.isFloat) {
		if (success) {
			*success = true;
		}

		return of.num.ui64;
	} else {
		return StringToUInt64(of.barr.get(), of.barrLen, 0, success);
	}
}


int64_t MySQLPreparedStatementResultImpl::getInt64(int index, bool* success) const
{
	const OutField& of = outFields[index];

	if (of.isInt  ||  of.isFloat) {
		if (success) {
			*success = true;
		}

		return of.num.i64;
	} else {
		return StringToInt64(of.barr.get(), of.barrLen, 0, success);
	}
}


float MySQLPreparedStatementResultImpl::getFloat(int index, bool* success) const
{
	const OutField& of = outFields[index];

	if (of.isFloat  ||  of.isInt) {
		if (success) {
			*success = true;
		}

		return of.num.f;
	} else {
		return StringToFloat(of.barr.get(), of.barrLen, success);
	}
}


double MySQLPreparedStatementResultImpl::getDouble(int index, bool* success) const
{
	const OutField& of = outFields[index];

	if (of.isFloat  ||  of.isInt) {
		if (success) {
			*success = true;
		}

		return of.num.d;
	} else {
		return StringToDouble(of.barr.get(), of.barrLen, success);
	}
}


ByteArray MySQLPreparedStatementResultImpl::getBLOB(int index, bool* success) const
{
	if (success) {
		*success = true;
	}
	return outFields[index].barr;
}


UString MySQLPreparedStatementResultImpl::getString(int index, bool* success) const
{
	return UString::fromUTF8(getStringUTF8(index, success));
}


ByteArray MySQLPreparedStatementResultImpl::getStringUTF8(int index, bool* success) const
{
	const OutField& of = outFields[index];

	if (success) {
		*success = true;
	}

	if (of.isInt) {
		char tmp[32];

		if (of.iunsigned) {
			sprintf(tmp, "%lu", of.num.ui64);
		} else {
			sprintf(tmp, "%ld", of.num.ui64);
		}

		return ByteArray(tmp, strlen(tmp));
	} else if (of.isFloat) {
		char tmp[64];

		if (of.barrLen == 4) {
			sprintf(tmp, "%f", (double) of.num.f);
		} else {
			sprintf(tmp, "%f", (double) of.num.d);
		}

		return ByteArray(tmp, strlen(tmp));
	} else {
		return outFields[index].barr;
	}
}


bool MySQLPreparedStatementResultImpl::isNull(int index) const
{
	return outFields[index].isnull;
}


bool MySQLPreparedStatementResultImpl::getBool(int index, bool* success) const
{
	return getUInt32(index, success) != 0;
}


int MySQLPreparedStatementResultImpl::getType(int index) const
{
	return ConvertFromMySQLType(outFields[index].type, outFields[index].flags);
}


CString MySQLPreparedStatementResultImpl::getColumnName(int index) const
{
	return outFields[index].name;
}


int MySQLPreparedStatementResultImpl::getColumnIndex(const CString& name) const
{
	map<CString, int>::const_iterator it = fieldIndexByNameMap.find(CString(name).lower());

	if (it == fieldIndexByNameMap.end()) {
		return -1;
	}

	return it->second;
}


unsigned int MySQLPreparedStatementResultImpl::getColumnCount() const
{
	return numFields;
}


uint64_t MySQLPreparedStatementResultImpl::getAffectedRowCount() const
{
	return numAffectedRows;
}
