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

#include "MySQLResultImpl.h"
#include "MySQLDatabaseImpl.h"
#include <utility>

using std::pair;





MySQLResultImpl::MySQLResultImpl(MySQLDatabaseImpl* db, MYSQL_RES* res)
		: db(db), res(res), fields(NULL), fieldIndicesByNameAvailable(false), affectedRowCount(0), curRow(NULL),
		  curRowLens(NULL)
{
	affectedRowCount = mysql_affected_rows(db->getMySQLHandle());
}


MySQLResultImpl::~MySQLResultImpl()
{
	mysql_free_result(res);
}


bool MySQLResultImpl::nextRecord()
{
	if (!res) {
		return false;
	}

	curRow = mysql_fetch_row(res);
	curRowLens = mysql_fetch_lengths(res);

	return curRow != NULL;
}


uint32_t MySQLResultImpl::getUInt32(int index, bool* success) const
{
	return StringToUInt32(curRow[index], curRowLens[index], 0, success);
}


int32_t MySQLResultImpl::getInt32(int index, bool* success) const
{
	return StringToInt32(curRow[index], curRowLens[index], 0, success);
}


uint64_t MySQLResultImpl::getUInt64(int index, bool* success) const
{
	return StringToUInt64(curRow[index], curRowLens[index], 0, success);
}


int64_t MySQLResultImpl::getInt64(int index, bool* success) const
{
	return StringToInt64(curRow[index], curRowLens[index], 0, success);
}


float MySQLResultImpl::getFloat(int index, bool* success) const
{
	return StringToFloat(curRow[index], curRowLens[index], success);
}


double MySQLResultImpl::getDouble(int index, bool* success) const
{
	return StringToDouble(curRow[index], curRowLens[index], success);
}


ByteArray MySQLResultImpl::getBLOB(int index, bool* success) const
{
	return ByteArray::readAlias(curRow[index], curRowLens[index]);
}


UString MySQLResultImpl::getString(int index, bool* success) const
{
	return UString::fromUTF8(getStringUTF8(index, success));
}


ByteArray MySQLResultImpl::getStringUTF8(int index, bool* success) const
{
	return getBLOB(index, success);
}


bool MySQLResultImpl::isNull(int index) const
{
	return curRow[index] == NULL;
}


bool MySQLResultImpl::getBool(int index, bool* success) const
{
	return curRow[index]  &&  strcmp(curRow[index], "1") == 0;
}


int MySQLResultImpl::getType(int index) const
{
	MYSQL_FIELD* field = mysql_fetch_field_direct(res, index);
	return ConvertFromMySQLType(field->type, field->flags);
}


CString MySQLResultImpl::getColumnName(int index) const
{
	ensureFieldInfoAvailable();

	return fields[index].name;
}


int MySQLResultImpl::getColumnIndex(const CString& name) const
{
	ensureFieldNameIndexMapAvailable();

	map<CString, int>::const_iterator it = fieldIndicesByName.find(CString(name).lower());

	if (it == fieldIndicesByName.end()) {
		return -1;
	}

	return it->second;
}


unsigned int MySQLResultImpl::getColumnCount() const
{
	if (res) {
		return mysql_num_fields(res);
	} else {
		return 0;
	}
}


uint64_t MySQLResultImpl::getAffectedRowCount() const
{
	return affectedRowCount;
}


void MySQLResultImpl::ensureFieldInfoAvailable() const
{
	if (!fields) {
		const_cast<MySQLResultImpl*>(this)->fields = mysql_fetch_fields(res);
	}
}


void MySQLResultImpl::ensureFieldNameIndexMapAvailable() const
{
	ensureFieldInfoAvailable();

	if (!fieldIndicesByNameAvailable) {
		MySQLResultImpl* ncThis = const_cast<MySQLResultImpl*>(this);

		unsigned int cc = getColumnCount();

		for (unsigned int i = 0 ; i < cc ; i++) {
			ncThis->fieldIndicesByName.insert(pair<CString, int>(CString(fields[i].name).lower(), i));
		}

		ncThis->fieldIndicesByNameAvailable = true;
	}
}


