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

#include <nxcommon/config.h>
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
#ifdef NXCOMMON_UNICODE_ENABLED
	virtual void bindString(size_t index, const UString& value);
#endif
	virtual void bindStringUTF8(size_t index, const CString& value);
	virtual void bindBLOB(size_t index, const ByteArray& value);
	virtual void bindNull(size_t index);
	virtual void bindBool(size_t index, bool value);

#ifdef NXCOMMON_UNICODE_ENABLED
	virtual void prepare(const UString& query);
#endif
	virtual void prepareUTF8(const CString& query);
#ifdef NXCOMMON_UNICODE_ENABLED
	virtual void prepare(const UString& query, const UChar** codeLeft);
#endif
	virtual void prepareUTF8(const CString& query, const char** codeLeft);
	virtual SQLResultImpl* execute();
	virtual void finalize();
	virtual void reset();

private:
	SQLitePreparedStatementImpl(SQLiteDatabaseImpl* db);

private:
	SQLiteDatabaseImpl* db;
	sqlite3_stmt* stmt;


	friend class SQLiteDatabaseImpl;
};

#endif /* SQLITEQUERYIMPL_H_ */
