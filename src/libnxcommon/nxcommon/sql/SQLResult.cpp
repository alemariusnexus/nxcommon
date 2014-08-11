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

#include "SQLResult.h"





SQLResult::SQLResult(const SQLDatabase& db, const SQLPreparedStatement& pstmt, const shared_ptr<SQLResultImpl>& impl)
		: data(new Data(db, pstmt, impl))
{
}


SQLResult::SQLResult(const SQLResult& other)
		: data(other.data)
{
}


SQLResult::Data::~Data()
{
}


bool SQLResult::nextRecord()
{
	return data->impl->nextRecord();
}


uint32_t SQLResult::getUInt32(size_t index, bool* success) const
{
	return data->impl->getUInt32(index, success);
}


int32_t SQLResult::getInt32(size_t index, bool* success) const
{
	return data->impl->getInt32(index, success);
}


uint64_t SQLResult::getUInt64(size_t index, bool* success) const
{
	return data->impl->getUInt64(index, success);
}


int64_t SQLResult::getInt64(size_t index, bool* success) const
{
	return data->impl->getInt64(index, success);
}


float SQLResult::getFloat(size_t index, bool* success) const
{
	return data->impl->getFloat(index, success);
}


double SQLResult::getDouble(size_t index, bool* success) const
{
	return data->impl->getDouble(index, success);
}


ByteArray SQLResult::getBLOB(size_t index, bool* success) const
{
	return data->impl->getBLOB(index, success);
}


UString SQLResult::getString(size_t index, bool* success) const
{
	return data->impl->getString(index, success);
}


ByteArray SQLResult::getStringUTF8(size_t index, bool* success) const
{
	return data->impl->getStringUTF8(index, success);
}


bool SQLResult::isNull(size_t index) const
{
	return data->impl->isNull(index);
}


bool SQLResult::getBool(size_t index, bool* success) const
{
	return data->impl->getBool(index, success);
}


int SQLResult::getType(size_t index) const
{
	return data->impl->getType(index);
}


CString SQLResult::getColumnName(int index) const
{
	return data->impl->getColumnName(index);
}


int SQLResult::getColumnIndex(const CString& name) const
{
	return data->impl->getColumnIndex(name);
}


unsigned int SQLResult::getColumnCount() const
{
	return data->impl->getColumnCount();
}


uint64_t SQLResult::getAffectedRowCount() const
{
	return data->impl->getAffectedRowCount();
}
