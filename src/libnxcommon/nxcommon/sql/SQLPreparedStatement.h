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

#ifndef SQLPREPAREDSTATEMENT_H_
#define SQLPREPAREDSTATEMENT_H_

#include "driver/SQLPreparedStatementImpl.h"
#include "SQLDatabase.h"
#include <unicode/unistr.h>
#include <memory>

using icu::UnicodeString;
using std::shared_ptr;




class SQLPreparedStatement
{
private:
	class Data
	{
	public:
		Data(const SQLDatabase& db, const shared_ptr<SQLPreparedStatementImpl>& impl)
				: db(db), prepared(false), executed(false), impl(impl) {}
		Data(const Data& other) : db(other.db), prepared(other.prepared), executed(other.executed), impl(other.impl) {}
		~Data();

		// NOTE: Do NOT change the order in which db/impl are listed here, because they HAVE to be destroyed in
		// this particular order (impl first, then db). It is an error to delete the statement only AFTER the
		// database has been destroyed!
		SQLDatabase db;
		bool prepared, executed;
		shared_ptr<SQLPreparedStatementImpl> impl;
	};

public:
	SQLPreparedStatement() : data(NULL) {}
	SQLPreparedStatement(const SQLPreparedStatement& other);

	SQLDatabase getDatabase() const { return data->db; }

	bool isValid() const { return data.get() != NULL; }
	bool isPrepared() const { return data->prepared; }

	SQLPreparedStatement& bindUInt32(size_t index, uint32_t value);
	SQLPreparedStatement& bindInt32(size_t index, int32_t value);
	SQLPreparedStatement& bindUInt64(size_t index, uint64_t value);
	SQLPreparedStatement& bindInt64(size_t index, int64_t value);
	SQLPreparedStatement& bindFloat(size_t index, float value);
	SQLPreparedStatement& bindDouble(size_t index, double value);
	SQLPreparedStatement& bindString(size_t index, const UString& value);
	SQLPreparedStatement& bindStringUTF8(size_t index, const CString& value);
	SQLPreparedStatement& bindBLOB(size_t index, const ByteArray& value);
	SQLPreparedStatement& bindNull(size_t index);
	SQLPreparedStatement& bindBool(size_t index, bool value);

	void prepare(const UString& query);
	void prepareUTF8(const CString& query);
	SQLResult execute();

	void reset();
	void finish();

	SQLPreparedStatementImpl* getImplementation() { return data->impl.get(); }
	const SQLPreparedStatementImpl* getImplementation() const { return data->impl.get(); }

private:
	SQLPreparedStatement(const SQLDatabase& db, const shared_ptr<SQLPreparedStatementImpl>& impl);

private:
	void ensureBindable();
	void ensurePrepared();

private:
	shared_ptr<Data> data;

	friend class SQLDatabase;
};

#endif /* SQLPREPAREDSTATEMENT_H_ */
