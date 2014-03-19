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

	bool isValid() const { return data.get() != NULL; }
	bool isPrepared() const { return data->prepared; }

	void bindUInt32(size_t index, uint32_t value);
	void bindInt32(size_t index, int32_t value);
	void bindUInt64(size_t index, uint64_t value);
	void bindInt64(size_t index, int64_t value);
	void bindFloat(size_t index, float value);
	void bindDouble(size_t index, double value);
	void bindString(size_t index, const UString& value);
	void bindStringUTF8(size_t index, const ByteArray& value);
	void bindBLOB(size_t index, const ByteArray& value);
	void bindNull(size_t index);

	void prepare(const UString& query);
	void prepareUTF8(const ByteArray& query);
	void execute();

	bool nextRecord();
	void reset();
	void finish();

	uint32_t getUInt32(size_t index) const;
	int32_t getInt32(size_t index) const;
	uint64_t getUInt64(size_t index) const;
	int64_t getInt64(size_t index) const;
	float getFloat(size_t index) const;
	double getDouble(size_t index) const;
	ByteArray getBLOB(size_t index) const;
	UString getString(size_t index) const;
	ByteArray getStringUTF8(size_t index) const;

	uint64_t getAffectedRowCount() const;

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
