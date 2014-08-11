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
#include "SQLResult.h"
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
	if (prepared) {
		// We MAY NOT THROW an exception in here, because C++ prohibits throwing exceptions from destructors
		// during stack unwinding, so better be safe than sorry here...

		// TODO: Maybe find a better way to handle this
		try {
			if (executed) {
				impl->reset();
			}

			if (prepared) {
				impl->finalize();
			}
		} catch (Exception& ex) {
			fprintf(stderr, "ERROR: Exception caught in SQLPreparedStatement::Data::~Data(): %s\n",
					ex.what());
		}
	}
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


SQLPreparedStatement& SQLPreparedStatement::bindUInt32(size_t index, uint32_t value)
{
	ensureBindable();
	data->impl->bindUInt32(index, value);
	return *this;
}


SQLPreparedStatement& SQLPreparedStatement::bindInt32(size_t index, int32_t value)
{
	ensureBindable();
	data->impl->bindInt32(index, value);
	return *this;
}


SQLPreparedStatement& SQLPreparedStatement::bindUInt64(size_t index, uint64_t value)
{
	ensureBindable();
	data->impl->bindUInt64(index, value);
	return *this;
}


SQLPreparedStatement& SQLPreparedStatement::bindInt64(size_t index, int64_t value)
{
	ensureBindable();
	data->impl->bindInt64(index, value);
	return *this;
}


SQLPreparedStatement& SQLPreparedStatement::bindFloat(size_t index, float value)
{
	ensureBindable();
	data->impl->bindFloat(index, value);
	return *this;
}


SQLPreparedStatement& SQLPreparedStatement::bindDouble(size_t index, double value)
{
	ensureBindable();
	data->impl->bindDouble(index, value);
	return *this;
}


SQLPreparedStatement& SQLPreparedStatement::bindString(size_t index, const UString& value)
{
	ensureBindable();
	data->impl->bindString(index, value);
	return *this;
}


SQLPreparedStatement& SQLPreparedStatement::bindStringUTF8(size_t index, const ByteArray& value)
{
	ensureBindable();
	data->impl->bindStringUTF8(index, value);
	return *this;
}


SQLPreparedStatement& SQLPreparedStatement::bindBLOB(size_t index, const ByteArray& value)
{
	ensureBindable();
	this->data->impl->bindBLOB(index, value);
	return *this;
}


SQLPreparedStatement& SQLPreparedStatement::bindNull(size_t index)
{
	ensureBindable();
	data->impl->bindNull(index);
	return *this;
}


SQLPreparedStatement& SQLPreparedStatement::bindBool(size_t index, bool value)
{
	ensureBindable();
	data->impl->bindBool(index, value);
	return *this;
}


SQLResult SQLPreparedStatement::execute()
{
	if (data->executed) {
		reset();
	}

	SQLResultImpl* impl = data->impl->execute();
	data->executed = true;

	return SQLResult(data->db, *this, shared_ptr<SQLResultImpl>(impl));
}


void SQLPreparedStatement::reset()
{
	if (!data->prepared  ||  !data->executed)
		return;

	data->executed = false;
	data->impl->reset();
}


void SQLPreparedStatement::finish()
{
	if (!data->prepared)
		return;

	if (data->executed) {
		data->impl->reset();
	}

	data->prepared = false;
	data->executed = false;
	data->impl->finalize();
}


