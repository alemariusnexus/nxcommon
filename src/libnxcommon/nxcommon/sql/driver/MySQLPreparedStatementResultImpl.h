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

#ifndef MYSQLPREPAREDSTATEMENTRESULTIMPL_H_
#define MYSQLPREPAREDSTATEMENTRESULTIMPL_H_

#include "SQLResultImpl.h"
#include "MySQLPreparedStatementImpl.h"
#include <memory>
#include <map>

using std::shared_ptr;
using std::map;



class MySQLPreparedStatementResultImpl : public SQLResultImpl
{
private:
	typedef VariantNum Num;

	struct OutField
	{
		CString name;

		enum_field_types type;
		unsigned int flags;
		my_bool iunsigned;
		Num num;
		void (VariantNum::*numSetFromMethod)();
		ByteArray barr;
		unsigned long barrLen;
		my_bool isnull;

		bool isInt;
		bool isFloat;
	};

public:
	virtual ~MySQLPreparedStatementResultImpl();

	virtual bool nextRecord();

	virtual uint32_t getUInt32(int index, bool* success = NULL) const;
	virtual int32_t getInt32(int index, bool* success = NULL) const;
	virtual uint64_t getUInt64(int index, bool* success = NULL) const;
	virtual int64_t getInt64(int index, bool* success = NULL) const;
	virtual float getFloat(int index, bool* success = NULL) const;
	virtual double getDouble(int index, bool* success = NULL) const;
	virtual ByteArray getBLOB(int index, bool* success = NULL) const;
	virtual UString getString(int index, bool* success = NULL) const;
	virtual CString getStringUTF8(int index, bool* success = NULL) const;
	virtual bool isNull(int index) const;
	virtual bool getBool(int index, bool* success = NULL) const;
	virtual int getType(int index) const;
	virtual CString getColumnName(int index) const;
	virtual int getColumnIndex(const CString& name) const;

	virtual unsigned int getColumnCount() const;

	virtual uint64_t getAffectedRowCount() const;

private:
	MySQLPreparedStatementResultImpl(MySQLPreparedStatementImpl* pstmt, MYSQL_STMT* stmt);

	void reset();
	void executed();

private:
	MySQLPreparedStatementImpl* pstmt;
	MySQLDatabaseImpl* db;

	MYSQL_STMT* stmt;

	unsigned int numFields;
	MYSQL_BIND* outBinds;
	OutField* outFields;

	map<CString, int> fieldIndexByNameMap;

	unsigned int* blobIndices;
	unsigned int numBlobIndices;

	uint64_t numAffectedRows;

	// Workaround for a bug in libmysql.c: Although recommended in the MySQL documentation for mysql_stmt_fetch, libmysql
	// does NOT seem to support binding NULL to a binding point buffer, nor a buffer length of zero. The library routines
	// crash in this case (or fail an assertion in debug mode), so we have to use a non-NULL dummy buffer here.
	char dummyBuffer[1];


	friend class MySQLPreparedStatementImpl;
	friend class MySQLDatabaseImpl;
};

#endif /* MYSQLPREPAREDSTATEMENTRESULTIMPL_H_ */
