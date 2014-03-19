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

#ifndef SQLPREPAREDSTATEMENTIMPL_H_
#define SQLPREPAREDSTATEMENTIMPL_H_

#include <unicode/unistr.h>
#include "../../CString.h"
#include "../../UString.h"
#include "../../ByteArray.h"




class SQLPreparedStatementImpl
{
public:
	virtual ~SQLPreparedStatementImpl() {}

	virtual void bindUInt32(size_t index, uint32_t value) = 0;
	virtual void bindInt32(size_t index, int32_t value) = 0;
	virtual void bindUInt64(size_t index, uint64_t value) = 0;
	virtual void bindInt64(size_t index, int64_t value) = 0;
	virtual void bindFloat(size_t index, float value) = 0;
	virtual void bindDouble(size_t index, double value) = 0;
	virtual void bindString(size_t index, const UString& value) = 0;
	virtual void bindStringUTF8(size_t index, const ByteArray& value) = 0;
	virtual void bindBLOB(size_t index, const ByteArray& value) = 0;
	virtual void bindNull(size_t index) = 0;

	virtual void prepare(const UString& query) = 0;
	virtual void prepareUTF8(const ByteArray& query) = 0;
	virtual void execute() = 0;
	virtual void reset() = 0;
	virtual void finalize() = 0;

	virtual bool nextRecord() = 0;

	virtual uint32_t getUInt32(size_t index) const = 0;
	virtual int32_t getInt32(size_t index) const = 0;
	virtual uint64_t getUInt64(size_t index) const = 0;
	virtual int64_t getInt64(size_t index) const = 0;
	virtual float getFloat(size_t index) const = 0;
	virtual double getDouble(size_t index) const = 0;
	virtual ByteArray getBLOB(size_t index) const = 0;
	virtual UString getString(size_t index) const = 0;
	virtual ByteArray getStringUTF8(size_t index) const = 0;

	virtual uint64_t getAffectedRowCount() const = 0;
};

#endif /* SQLPREPAREDSTATEMENTIMPL_H_ */
