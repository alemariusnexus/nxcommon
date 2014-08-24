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

#ifndef SQLRESULTIMPL_H_
#define SQLRESULTIMPL_H_

#include <nxcommon/config.h>
#include "../../UString.h"
#include "../../ByteArray.h"
#include "../../CString.h"



class SQLResultImpl
{
public:
	virtual ~SQLResultImpl() {}

	virtual bool nextRecord() = 0;

	virtual uint32_t getUInt32(int index, bool* success = NULL) const = 0;
	virtual int32_t getInt32(int index, bool* success = NULL) const = 0;
	virtual uint64_t getUInt64(int index, bool* success = NULL) const = 0;
	virtual int64_t getInt64(int index, bool* success = NULL) const = 0;
	virtual float getFloat(int index, bool* success = NULL) const = 0;
	virtual double getDouble(int index, bool* success = NULL) const = 0;
	virtual ByteArray getBLOB(int index, bool* success = NULL) const = 0;
	virtual UString getString(int index, bool* success = NULL) const = 0;
	virtual CString getStringUTF8(int index, bool* success = NULL) const = 0;
	virtual bool isNull(int index) const = 0;
	virtual bool getBool(int index, bool* success = NULL) const = 0;
	virtual int getType(int index) const = 0;
	virtual CString getColumnName(int index) const = 0;
	virtual int getColumnIndex(const CString& name) const = 0;

	virtual unsigned int getColumnCount() const = 0;

	virtual uint64_t getAffectedRowCount() const = 0;
};

#endif /* SQLRESULTIMPL_H_ */
