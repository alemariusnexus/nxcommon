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

#ifndef SQLRESULT_H_
#define SQLRESULT_H_

#include <nxcommon/config.h>
#include <memory>
#include "SQLDatabase.h"
#include "SQLPreparedStatement.h"

using std::shared_ptr;



class SQLResult
{
private:
	class Data
	{
	public:
		Data(const SQLDatabase& db, const SQLPreparedStatement& pstmt, const shared_ptr<SQLResultImpl>& impl)
				: db(db), pstmt(pstmt), impl(impl) {}
		Data(const Data& other) : db(other.db), pstmt(other.pstmt), impl(other.impl) {}
		~Data();

		// TODO: Add order note, check order
		SQLDatabase db;
		SQLPreparedStatement pstmt;
		shared_ptr<SQLResultImpl> impl;
	};

public:
	SQLResult() : data(NULL) {}
	SQLResult(const SQLResult& other);

	SQLDatabase getDatabase() const { return data->db; }

	bool isValid() const { return data.get() != NULL; }

	bool nextRecord();

	uint32_t getUInt32(size_t index, bool* success = NULL) const;
	int32_t getInt32(size_t index, bool* success = NULL) const;
	uint64_t getUInt64(size_t index, bool* success = NULL) const;
	int64_t getInt64(size_t index, bool* success = NULL) const;
	float getFloat(size_t index, bool* success = NULL) const;
	double getDouble(size_t index, bool* success = NULL) const;
	ByteArray getBLOB(size_t index, bool* success = NULL) const;
	UString getString(size_t index, bool* success = NULL) const;
	CString getStringUTF8(size_t index, bool* success = NULL) const;
	bool isNull(size_t index) const;
	bool getBool(size_t index, bool* success = NULL) const;
	int getType(size_t index) const;
	CString getColumnName(int index) const;
	int getColumnIndex(const CString& name) const;

	uint32_t getUInt32(const CString& colName, bool* success = NULL) const
			{ return getUInt32(getColumnIndex(colName), success); }
	int32_t getInt32(const CString& colName, bool* success = NULL) const
			{ return getInt32(getColumnIndex(colName), success); }
	uint64_t getUInt64(const CString& colName, bool* success = NULL) const
			{ return getUInt64(getColumnIndex(colName), success); }
	int64_t getInt64(const CString& colName, bool* success = NULL) const
			{ return getInt64(getColumnIndex(colName), success); }
	float getFloat(const CString& colName, bool* success = NULL) const
			{ return getFloat(getColumnIndex(colName), success); }
	double getDouble(const CString& colName, bool* success = NULL) const
			{ return getDouble(getColumnIndex(colName), success); }
	ByteArray getBLOB(const CString& colName, bool* success = NULL) const
			{ return getBLOB(getColumnIndex(colName), success); }
	UString getString(const CString& colName, bool* success = NULL) const
			{ return getString(getColumnIndex(colName), success); }
	CString getStringUTF8(const CString& colName, bool* success = NULL) const
			{ return getStringUTF8(getColumnIndex(colName), success); }
	bool isNull(const CString& colName) const
			{ return isNull(getColumnIndex(colName)); }
	bool getBool(const CString& colName, bool* success = NULL) const
			{ return getBool(getColumnIndex(colName), success); }
	int getType(const CString& colName) const
			{ return getType(getColumnIndex(colName)); }

	unsigned int getColumnCount() const;

	uint64_t getAffectedRowCount() const;

private:
	SQLResult(const SQLDatabase& db, const SQLPreparedStatement& pstmt, const shared_ptr<SQLResultImpl>& impl);

private:
	shared_ptr<Data> data;


	friend class SQLDatabase;
	friend class SQLPreparedStatement;
};

#endif /* SQLRESULT_H_ */
