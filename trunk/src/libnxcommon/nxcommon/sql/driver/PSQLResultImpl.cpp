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

#include "PSQLResultImpl.h"
#include "sqlbase.h"
#include "../../strutil.h"




PSQLResultImpl::PSQLResultImpl(PGresult* res)
		: res(res), curTuple(-1)
{
}


PSQLResultImpl::~PSQLResultImpl()
{
	PQclear(res);
}


bool PSQLResultImpl::nextRecord()
{
	if (curTuple+1 >= PQntuples(res))
		return false;

	curTuple++;
	return true;
}


uint32_t PSQLResultImpl::getUInt32(int index, bool* success) const
{
	const char* sVal = PQgetvalue(res, curTuple, index);
	return StringToUInt32(sVal, PQgetlength(res, curTuple, index), 0, success);
}


int32_t PSQLResultImpl::getInt32(int index, bool* success) const
{
	const char* sVal = PQgetvalue(res, curTuple, index);
	return StringToInt32(sVal, PQgetlength(res, curTuple, index), 0, success);
}


uint64_t PSQLResultImpl::getUInt64(int index, bool* success) const
{
	const char* sVal = PQgetvalue(res, curTuple, index);
	return StringToUInt64(sVal, PQgetlength(res, curTuple, index), 0, success);
}


int64_t PSQLResultImpl::getInt64(int index, bool* success) const
{
	const char* sVal = PQgetvalue(res, curTuple, index);
	return StringToInt64(sVal, PQgetlength(res, curTuple, index), 0, success);
}


float PSQLResultImpl::getFloat(int index, bool* success) const
{
	const char* sVal = PQgetvalue(res, curTuple, index);
	return StringToFloat(sVal, success);
}


double PSQLResultImpl::getDouble(int index, bool* success) const
{
	const char* sVal = PQgetvalue(res, curTuple, index);
	return StringToDouble(sVal, success);
}


ByteArray PSQLResultImpl::getBLOB(int index, bool* success) const
{
	char* buf = PQgetvalue(res, curTuple, index);
	int len = PQgetlength(res, curTuple, index);
	int format = PQfformat(res, index);

	if (format == 0) {
		// Text format

		Oid type = PQftype(res, index);

		if (type == NXPSQLOID_BYTEA) {
			// TODO: This is horrible. We have to unescape first and then copy it all over again, because libpq
			// allocates storage for the buffer using malloc, but ByteArray uses delete[] instead of free()

			size_t sz;
			char* binData = (char*) PQunescapeBytea((const unsigned char*) buf, &sz);

			ByteArray barr(binData, sz);

			PQfreemem(binData);

			if (success) {
				*success = true;
			}

			return barr;
		} else {
			// It's a regular string

			if (success) {
				*success = true;
			}

			return ByteArray::readAlias(buf, len);
		}
	} else {
		// Binary format

		if (success) {
			*success = true;
		}

		return ByteArray::readAlias(buf, len);
	}
}


UString PSQLResultImpl::getString(int index, bool* success) const
{
	return UString::fromUTF8(getStringUTF8(index, success));
}


ByteArray PSQLResultImpl::getStringUTF8(int index, bool* success) const
{
	if (success) {
		*success = true;
	}

	const char* data = PQgetvalue(res, curTuple, index);
	int len = PQgetlength(res, curTuple, index);
	return ByteArray::readAlias(data, len);
}


bool PSQLResultImpl::isNull(int index) const
{
	return PQgetisnull(res, curTuple, index);
}


bool PSQLResultImpl::getBool(int index, bool* success) const
{
	if (success) {
		*success = true;
	}

	const char* data = PQgetvalue(res, curTuple, index);
	int len = PQgetlength(res, curTuple, index);
	return len == 1  &&  data[0] == 't';
}


int PSQLResultImpl::getType(int index) const
{
	Oid type = PQftype(res, index);

	switch (type) {
	case NXPSQLOID_INT2:
	case NXPSQLOID_INT4:
		return SQL_DATA_TYPE_INT32;

	case NXPSQLOID_INT8:
		return SQL_DATA_TYPE_INT64;

	case NXPSQLOID_FLOAT:
		return SQL_DATA_TYPE_FLOAT;

	case NXPSQLOID_DOUBLE:
		return SQL_DATA_TYPE_DOUBLE;

	case NXPSQLOID_BYTEA:
		return SQL_DATA_TYPE_BLOB;

	default:
		return SQL_DATA_TYPE_STRING;
	}
}


CString PSQLResultImpl::getColumnName(int index) const
{
	return PQfname(res, index);
}


int PSQLResultImpl::getColumnIndex(const CString& name) const
{
	return PQfnumber(res, name.get());
}


unsigned int PSQLResultImpl::getColumnCount() const
{
	return PQnfields(res);
}


uint64_t PSQLResultImpl::getAffectedRowCount() const
{
	char* rowCountStr = PQcmdTuples(res);

#if ULONG_SIZE >= 8
	return strtoul(rowCountStr, NULL, 0);
#else
	return strtoull(rowCountStr, NULL, 0);
#endif
}
