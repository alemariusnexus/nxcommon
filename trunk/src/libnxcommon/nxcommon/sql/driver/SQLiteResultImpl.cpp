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

#include "SQLiteResultImpl.h"
#include "sqlbase.h"
#include <utility>

using std::pair;





SQLiteResultImpl::SQLiteResultImpl(sqlite3_stmt* stmt, bool firstStepHasData, uint64_t affectedRowCount)
		: stmt(stmt), firstStep(true), firstStepHasData(firstStepHasData), affectedRowCount(affectedRowCount),
		  columnIndexByNameMapValid(false)
{
}


SQLiteResultImpl::~SQLiteResultImpl()
{
}


bool SQLiteResultImpl::nextRecord()
{
	if (firstStep) {
		firstStep = false;

		if (firstStepHasData)
			return true;
		else
			return false;
	}
	int res = sqlite3_step(stmt);

	if (res == SQLITE_DONE)
		return false;

	return true;
}


uint32_t SQLiteResultImpl::getUInt32(int index, bool* success) const
{
	if (success) {
		// TODO: There seems to be no easy way to see if the conversion was successful.
		*success = true;
	}
	return sqlite3_column_int64(stmt, index);
}


int32_t SQLiteResultImpl::getInt32(int index, bool* success) const
{
	if (success) {
		// TODO: There seems to be no easy way to see if the conversion was successful.
		*success = true;
	}
	return sqlite3_column_int(stmt, index);
}


uint64_t SQLiteResultImpl::getUInt64(int index, bool* success) const
{
	if (success) {
		// TODO: There seems to be no easy way to see if the conversion was successful.
		*success = true;
	}
	return sqlite3_column_int64(stmt, index);
}


int64_t SQLiteResultImpl::getInt64(int index, bool* success) const
{
	if (success) {
		// TODO: There seems to be no easy way to see if the conversion was successful.
		*success = true;
	}
	return sqlite3_column_int64(stmt, index);
}


float SQLiteResultImpl::getFloat(int index, bool* success) const
{
	return getDouble(index, success);
}


double SQLiteResultImpl::getDouble(int index, bool* success) const
{
	if (success) {
		// TODO: There seems to be no easy way to see if the conversion was successful.
		*success = true;
	}
	return sqlite3_column_double(stmt, index);
}


ByteArray SQLiteResultImpl::getBLOB(int index, bool* success) const
{
	if (success) {
		// TODO: There seems to be no easy way to see if the conversion was successful.
		*success = true;
	}
	return ByteArray::readAlias((const char*) sqlite3_column_blob(stmt, index), sqlite3_column_bytes(stmt, index));
}


UString SQLiteResultImpl::getString(int index, bool* success) const
{
	if (success) {
		// TODO: There seems to be no easy way to see if the conversion was successful.
		*success = true;
	}
	return UString::readAlias((const UChar*) sqlite3_column_text16(stmt, index), sqlite3_column_bytes16(stmt, index) / 2);
}


ByteArray SQLiteResultImpl::getStringUTF8(int index, bool* success) const
{
	if (success) {
		// TODO: There seems to be no easy way to see if the conversion was successful.
		*success = true;
	}
	return ByteArray::readAlias((const char*) sqlite3_column_text(stmt, index), sqlite3_column_bytes(stmt, index));
}


bool SQLiteResultImpl::isNull(int index) const
{
	return sqlite3_column_type(stmt, index) == SQLITE_NULL;
}


bool SQLiteResultImpl::getBool(int index, bool* success) const
{
	if (success) {
		// TODO: There seems to be no easy way to see if the conversion was successful.
		*success = true;
	}
	return getInt32(index) != 0;
}


int SQLiteResultImpl::getType(int index) const
{
	switch (sqlite3_column_type(stmt, index)) {
	case SQLITE_INTEGER:
		return SQL_DATA_TYPE_INT64;

	case SQLITE_FLOAT:
		return SQL_DATA_TYPE_DOUBLE;

	case SQLITE_BLOB:
		return SQL_DATA_TYPE_BLOB;

	case SQLITE_TEXT:
		return SQL_DATA_TYPE_STRING;

	case SQLITE_NULL:
		return SQL_DATA_TYPE_NULL;

	default:
		return SQL_DATA_TYPE_NULL;
	}
}


CString SQLiteResultImpl::getColumnName(int index) const
{
	return sqlite3_column_name(stmt, index);
}


int SQLiteResultImpl::getColumnIndex(const CString& name) const
{
	ensureColumnIndexByNameMapValid();

	map<CString, int>::const_iterator it = columnIndexByNameMap.find(CString(name).lower());

	if (it == columnIndexByNameMap.end()) {
		return -1;
	}

	return it->second;
}


unsigned int SQLiteResultImpl::getColumnCount() const
{
	return sqlite3_column_count(stmt);
}


uint64_t SQLiteResultImpl::getAffectedRowCount() const
{
	return affectedRowCount;
}


void SQLiteResultImpl::ensureColumnIndexByNameMapValid() const
{
	if (!columnIndexByNameMapValid) {
		SQLiteResultImpl* ncThis = const_cast<SQLiteResultImpl*>(this);

		unsigned int cc = getColumnCount();

		for (unsigned int i = 0 ; i < cc ; i++) {
			ncThis->columnIndexByNameMap.insert(pair<CString, int>(getColumnName(i).lower(), i));
		}

		ncThis->columnIndexByNameMapValid = true;
	}
}
