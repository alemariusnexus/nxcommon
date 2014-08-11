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

#include "SQLDatabase.h"
#include "SQLPreparedStatement.h"
#include "SQLResult.h"






SQLDatabase::SQLDatabase(SQLDatabaseImpl* impl)
		: data(shared_ptr<Data>(new Data(shared_ptr<SQLDatabaseImpl>(impl))))
{
}


SQLDatabase::SQLDatabase(const SQLDatabase& other)
		: data(other.data)
{
}


void SQLDatabase::close()
{
	data = NULL;
}


SQLPreparedStatement SQLDatabase::createPreparedStatement()
{
	SQLPreparedStatementImpl* impl = data->impl->createPreparedStatement();
	return SQLPreparedStatement(*this, shared_ptr<SQLPreparedStatementImpl>(impl));
}


SQLPreparedStatement SQLDatabase::createPreparedStatement(const UString& query)
{
	SQLPreparedStatement stmt = createPreparedStatement();
	stmt.prepare(query);
	return stmt;
}


SQLPreparedStatement SQLDatabase::createPreparedStatementUTF8(const ByteArray& query)
{
	SQLPreparedStatement stmt = createPreparedStatement();
	stmt.prepareUTF8(query);
	return stmt;
}


SQLResult SQLDatabase::sendQuery(const UString& query)
{
	SQLResultImpl* impl = data->impl->sendQuery(query);
	return SQLResult(*this, SQLPreparedStatement(), shared_ptr<SQLResultImpl>(impl));
}


SQLResult SQLDatabase::sendQueryUTF8(const ByteArray& query)
{
	SQLResultImpl* impl = data->impl->sendQueryUTF8(query);
	return SQLResult(*this, SQLPreparedStatement(), shared_ptr<SQLResultImpl>(impl));
}


UString SQLDatabase::escapeString(const UString& str) const
{
	return data->impl->escapeString(str);
}


ByteArray SQLDatabase::escapeStringUTF8(const ByteArray& str) const
{
	return data->impl->escapeStringUTF8(str);
}


void SQLDatabase::setTimeout(uint64_t timeoutMillis)
{
	data->impl->setTimeout(timeoutMillis);
}
