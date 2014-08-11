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

#ifndef PSQLRESULTIMPL_H_
#define PSQLRESULTIMPL_H_

#include "SQLResultImpl.h"
#include "psqlutils.h"



class PSQLResultImpl : public SQLResultImpl
{
public:
	virtual ~PSQLResultImpl();

	virtual bool nextRecord();

	virtual uint32_t getUInt32(int index, bool* success = NULL) const;
	virtual int32_t getInt32(int index, bool* success = NULL) const;
	virtual uint64_t getUInt64(int index, bool* success = NULL) const;
	virtual int64_t getInt64(int index, bool* success = NULL) const;
	virtual float getFloat(int index, bool* success = NULL) const;
	virtual double getDouble(int index, bool* success = NULL) const;
	virtual ByteArray getBLOB(int index, bool* success = NULL) const;
	virtual UString getString(int index, bool* success = NULL) const;
	virtual ByteArray getStringUTF8(int index, bool* success = NULL) const;
	virtual bool isNull(int index) const;
	virtual bool getBool(int index, bool* success = NULL) const;
	virtual int getType(int index) const;
	virtual CString getColumnName(int index) const;
	virtual int getColumnIndex(const CString& name) const;

	virtual unsigned int getColumnCount() const;

	virtual uint64_t getAffectedRowCount() const;

private:
	PSQLResultImpl(PGresult* res);

private:
	PGresult* res;
	int curTuple;


	friend class PSQLPreparedStatementImpl;
	friend class PSQLDatabaseImpl;
};

#endif /* PSQLRESULTIMPL_H_ */
