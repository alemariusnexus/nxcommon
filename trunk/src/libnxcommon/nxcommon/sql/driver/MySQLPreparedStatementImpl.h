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

#ifndef MYSQLPREPAREDSTATEMENTIMPL_H_
#define MYSQLPREPAREDSTATEMENTIMPL_H_

#include <nxcommon/config.h>
#include "SQLPreparedStatementImpl.h"
#include "MySQLDatabaseImpl.h"
#include "sqlutils.h"


class MySQLPreparedStatementResultImpl;




class MySQLPreparedStatementImpl : public SQLPreparedStatementImpl
{
private:
	typedef VariantNum Num;

	struct Field
	{
		Num num;
		ByteArray barr;
		unsigned long barrLen;
	};

	struct OutField
	{
		enum_field_types type;
		my_bool iunsigned;
		Num num;
		ByteArray barr;
		unsigned long barrLen;
		my_bool isnull;

		bool isInt;
		bool isFloat;
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
	virtual void bindBool(size_t index, bool value);

	virtual void prepare(const UString& query);
	virtual void prepareUTF8(const ByteArray& query);
	virtual SQLResultImpl* execute();
	virtual void reset();
	virtual void finalize();

private:
	MySQLPreparedStatementImpl(MySQLDatabaseImpl* db);

private:
	MySQLDatabaseImpl* db;

	MYSQL_BIND* inBinds;
	Field* inFields;

	MySQLPreparedStatementResultImpl* curRes;


	friend class MySQLDatabaseImpl;
	friend class MySQLPreparedStatementResultImpl;
};

#endif /* MYSQLPREPAREDSTATEMENTIMPL_H_ */
