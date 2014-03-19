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

#ifndef MYSQLPREPAREDSTATEMENTIMPL_H_
#define MYSQLPREPAREDSTATEMENTIMPL_H_

#include <nxcommon/config.h>
#include "SQLPreparedStatementImpl.h"
#include "MySQLDatabaseImpl.h"



#ifdef NXCOMMON_LITTLE_ENDIAN
#define _DECLNUM(n,t) struct { t n; char _p_##n[sizeof(uint64_t)-sizeof(t)]; }
#else
#define _DECLNUM(n,t) struct { char _p_##n[sizeof(uint64_t)-sizeof(t)]; t n; }
#endif



class MySQLPreparedStatementImpl : public SQLPreparedStatementImpl
{
private:
	union Num {
		_DECLNUM(ui64, uint64_t);
		_DECLNUM(i64, int64_t);

		_DECLNUM(ui32, uint32_t);
		_DECLNUM(i32, int32_t);

		_DECLNUM(ui16, uint16_t);
		_DECLNUM(i16, int16_t);

		_DECLNUM(ui8, uint8_t);
		_DECLNUM(i8, int8_t);

		_DECLNUM(f, float);
		_DECLNUM(d, double);
	};

	struct Field
	{
		Num num;
		ByteArray barr;
		unsigned long barrLen;
	};

	struct OutField
	{
		enum_field_types type;
		Num num;
		ByteArray barr;
		unsigned long barrLen;
		my_bool isnull;
	};

public:
	virtual ~MySQLPreparedStatementImpl();

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
	virtual void reset();
	virtual void finalize();

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
	MySQLPreparedStatementImpl(MySQLDatabaseImpl* db);

private:
	MySQLDatabaseImpl* db;
	MYSQL_STMT* stmt;

	MYSQL_BIND* inBinds;
	Field* inFields;

	unsigned int numFields;
	MYSQL_BIND* outBinds;
	OutField* outFields;

	unsigned int* blobIndices;
	unsigned int numBlobIndices;

	uint64_t numAffectedRows;

	// Workaround for a bug in libmysql.c: Although recommended in the MySQL documentation for mysql_stmt_fetch, libmysql
	// does NOT seem to support binding NULL to a binding point buffer, nor a buffer length of zero. The library routines
	// crash in this case (or fail an assertion in debug mode), so we have to use a non-NULL dummy buffer here.
	char dummyBuffer[1];

	friend class MySQLDatabaseImpl;
};

#endif /* MYSQLPREPAREDSTATEMENTIMPL_H_ */
