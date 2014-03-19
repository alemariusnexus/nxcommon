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

#ifndef SQLITEPREPAREDSTATEMENTIMPL_H_
#define SQLITEPREPAREDSTATEMENTIMPL_H_

#include "SQLPreparedStatementImpl.h"
#include "SQLiteDatabaseImpl.h"



class SQLitePreparedStatementImpl : public SQLPreparedStatementImpl
{
public:
	virtual ~SQLitePreparedStatementImpl();

	virtual void bindUInt32(size_t index, uint32_t value);
	virtual void bindInt32(size_t index, int32_t value);
	virtual void bindUInt64(size_t index, uint64_t value);
	virtual void bindInt64(size_t index, int64_t value);
	virtual void bindFloat(size_t index, float value);
	virtual void bindDouble(size_t index, double value);
	virtual void bindString(size_t index, const UString& value);
	virtual void bindStringUTF8(size_t index, const ByteArray& value);
	virtual void bindBLOB(size_t index, const ByteArray& value);
	virtual void bindNull(size_t index);

	virtual void prepare(const UString& query);
	virtual void prepareUTF8(const ByteArray& query);
	virtual void execute();
	virtual void finalize();
	virtual void reset();

	virtual bool nextRecord();

	virtual uint32_t getUInt32(size_t index) const;
	virtual int32_t getInt32(size_t index) const;
	virtual uint64_t getUInt64(size_t index) const;
	virtual int64_t getInt64(size_t index) const;
	virtual float getFloat(size_t index) const;
	virtual double getDouble(size_t index) const;
	virtual ByteArray getBLOB(size_t index) const;
	virtual UString getString(size_t index) const;
	virtual ByteArray getStringUTF8(size_t index) const;

	virtual uint64_t getAffectedRowCount() const;

private:
	SQLitePreparedStatementImpl(SQLiteDatabaseImpl* db);

private:
	SQLiteDatabaseImpl* db;
	sqlite3_stmt* stmt;
	bool firstStep, firstStepHasData;

	friend class SQLiteDatabaseImpl;
};

#endif /* SQLITEQUERYIMPL_H_ */
