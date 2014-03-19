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

#ifndef SQLDATABASE_H_
#define SQLDATABASE_H_

#include "driver/SQLDatabaseImpl.h"
#include <memory>

using std::shared_ptr;



class SQLPreparedStatement;



class SQLDatabase
{
private:
	class Data
	{
	public:
		Data(const shared_ptr<SQLDatabaseImpl>& impl) : impl(impl) {}
		Data(const Data& other) : impl(other.impl) {}

		shared_ptr<SQLDatabaseImpl> impl;
	};

public:
	SQLDatabase() : data(NULL) {}
	SQLDatabase(SQLDatabaseImpl* impl);
	SQLDatabase(const SQLDatabase& other);

	bool isValid() const { return data.get() != NULL; }

	SQLPreparedStatement createPreparedStatement();
	SQLPreparedStatement createPreparedStatement(const UString& query);
	SQLPreparedStatement createPreparedStatementUTF8(const ByteArray& query);
	SQLPreparedStatement sendQuery(const UString& query);
	SQLPreparedStatement sendQueryUTF8(const ByteArray& query);

	uint64_t getLastInsertID() const { return data->impl->getLastInsertID(); }

private:
	shared_ptr<Data> data;
};

#endif /* SQLDATABASE_H_ */
