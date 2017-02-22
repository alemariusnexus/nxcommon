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

#include <nxcommon/config.h>
#include "driver/SQLDatabaseImpl.h"
#include "driver/sqlbase.h"
#include <memory>

using std::shared_ptr;



class SQLPreparedStatement;
class SQLResult;





class SQLDatabase
{
public:
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

	void close();
	bool isValid() const { return data.get() != NULL; }

	SQLPreparedStatement createPreparedStatement();
#ifdef NXCOMMON_UNICODE_ENABLED
	SQLPreparedStatement createPreparedStatement(const UString& query);
#endif
	SQLPreparedStatement createPreparedStatementUTF8(const CString& query);


#ifdef NXCOMMON_UNICODE_ENABLED
	SQLResult sendQuery(const UString& query);
#endif
	SQLResult sendQueryUTF8(const CString& query);

#ifdef NXCOMMON_UNICODE_ENABLED
	UString escapeString(const UString& str) const;
#endif
	CString escapeStringUTF8(const CString& str) const;

	uint64_t getCapabilities() const { return data->impl->getCapabilities(); }
	bool hasCapability(uint64_t cap) const { return data->impl->hasCapability(cap); }

	uint64_t getLastInsertID() const { return data->impl->getLastInsertID(); }

	void setTimeout(uint64_t timeoutMillis);

	SQLDatabaseImpl* getImplementation() { return data->impl.get(); }
	const SQLDatabaseImpl* getImplementation() const { return data->impl.get(); }

public:
	shared_ptr<Data> data;
};

#endif /* SQLDATABASE_H_ */
