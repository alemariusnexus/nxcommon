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

#include "SQLPreparedStatement.h"
#include "SQLDatabase.h"
#include "SQLException.h"





SQLPreparedStatement::SQLPreparedStatement(const SQLDatabase& db, const shared_ptr<SQLPreparedStatementImpl>& impl)
		: data(new Data(db, impl))
{
}


SQLPreparedStatement::SQLPreparedStatement(const SQLPreparedStatement& other)
		: data(other.data)
{

}


SQLPreparedStatement::Data::~Data()
{
	if (prepared)
		impl->finalize();
}


void SQLPreparedStatement::prepare(const UString& query)
{
	if (data->prepared)
		finish();

	data->impl->prepare(query);
	data->prepared = true;
}


void SQLPreparedStatement::prepareUTF8(const ByteArray& query)
{
	if (data->prepared)
		finish();

	data->impl->prepareUTF8(query);
	data->prepared = true;
}


void SQLPreparedStatement::ensureBindable()
{
	ensurePrepared();

	if (data->executed)
		reset();
}


void SQLPreparedStatement::ensurePrepared()
{
	if (!data->prepared) {
		throw SQLException("Attempt to bind values to an unprepared SQLPreparedStatement!", __FILE__, __LINE__);
	}
}


void SQLPreparedStatement::bindUInt32(size_t index, uint32_t value)
{
	ensureBindable();
	data->impl->bindUInt32(index, value);
}


void SQLPreparedStatement::bindInt32(size_t index, int32_t value)
{
	ensureBindable();
	data->impl->bindInt32(index, value);
}


void SQLPreparedStatement::bindUInt64(size_t index, uint64_t value)
{
	ensureBindable();
	data->impl->bindUInt64(index, value);
}


void SQLPreparedStatement::bindInt64(size_t index, int64_t value)
{
	ensureBindable();
	data->impl->bindInt64(index, value);
}


void SQLPreparedStatement::bindFloat(size_t index, float value)
{
	ensureBindable();
	data->impl->bindFloat(index, value);
}


void SQLPreparedStatement::bindDouble(size_t index, double value)
{
	ensureBindable();
	data->impl->bindDouble(index, value);
}


void SQLPreparedStatement::bindString(size_t index, const UString& value)
{
	ensureBindable();
	data->impl->bindString(index, value);
}


void SQLPreparedStatement::bindStringUTF8(size_t index, const ByteArray& value)
{
	ensureBindable();
	data->impl->bindStringUTF8(index, value);
}


void SQLPreparedStatement::bindBLOB(size_t index, const ByteArray& value)
{
	ensureBindable();
	this->data->impl->bindBLOB(index, value);
}


void SQLPreparedStatement::bindNull(size_t index)
{
	ensureBindable();
	data->impl->bindNull(index);
}


void SQLPreparedStatement::execute()
{
	data->impl->execute();
	data->executed = true;
}


bool SQLPreparedStatement::nextRecord()
{
	return data->impl->nextRecord();
}


void SQLPreparedStatement::reset()
{
	if (!data->prepared)
		return;

	data->executed = false;
	data->impl->reset();
}


void SQLPreparedStatement::finish()
{
	if (!data->prepared)
		return;

	data->prepared = false;
	data->executed = false;
	data->impl->finalize();
}


uint32_t SQLPreparedStatement::getUInt32(size_t index) const
{
	return data->impl->getUInt32(index);
}


int32_t SQLPreparedStatement::getInt32(size_t index) const
{
	return data->impl->getInt32(index);
}


uint64_t SQLPreparedStatement::getUInt64(size_t index) const
{
	return data->impl->getUInt64(index);
}


int64_t SQLPreparedStatement::getInt64(size_t index) const
{
	return data->impl->getInt64(index);
}


float SQLPreparedStatement::getFloat(size_t index) const
{
	return data->impl->getFloat(index);
}


double SQLPreparedStatement::getDouble(size_t index) const
{
	return data->impl->getDouble(index);
}


ByteArray SQLPreparedStatement::getBLOB(size_t index) const
{
	return data->impl->getBLOB(index);
}


UString SQLPreparedStatement::getString(size_t index) const
{
	return data->impl->getString(index);
}


ByteArray SQLPreparedStatement::getStringUTF8(size_t index) const
{
	return data->impl->getStringUTF8(index);
}


uint64_t SQLPreparedStatement::getAffectedRowCount() const
{
	return data->impl->getAffectedRowCount();
}


