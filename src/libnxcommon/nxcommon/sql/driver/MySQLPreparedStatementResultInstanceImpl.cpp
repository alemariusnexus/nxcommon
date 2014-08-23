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

#include "MySQLPreparedStatementResultInstanceImpl.h"




MySQLPreparedStatementResultInstanceImpl::MySQLPreparedStatementResultInstanceImpl(MySQLPreparedStatementResultImpl* impl)
		: impl(impl)
{
}


bool MySQLPreparedStatementResultInstanceImpl::nextRecord()
{
	return impl->nextRecord();
}


uint32_t MySQLPreparedStatementResultInstanceImpl::getUInt32(int index, bool* success) const
{
	return impl->getUInt32(index, success);
}


int32_t MySQLPreparedStatementResultInstanceImpl::getInt32(int index, bool* success) const
{
	return impl->getInt32(index, success);
}


uint64_t MySQLPreparedStatementResultInstanceImpl::getUInt64(int index, bool* success) const
{
	return impl->getUInt64(index, success);
}


int64_t MySQLPreparedStatementResultInstanceImpl::getInt64(int index, bool* success) const
{
	return impl->getInt64(index, success);
}


float MySQLPreparedStatementResultInstanceImpl::getFloat(int index, bool* success) const
{
	return impl->getFloat(index, success);
}


double MySQLPreparedStatementResultInstanceImpl::getDouble(int index, bool* success) const
{
	return impl->getDouble(index, success);
}


ByteArray MySQLPreparedStatementResultInstanceImpl::getBLOB(int index, bool* success) const
{
	return impl->getBLOB(index, success);
}


UString MySQLPreparedStatementResultInstanceImpl::getString(int index, bool* success) const
{
	return impl->getString(index, success);
}


CString MySQLPreparedStatementResultInstanceImpl::getStringUTF8(int index, bool* success) const
{
	return impl->getStringUTF8(index, success);
}


bool MySQLPreparedStatementResultInstanceImpl::isNull(int index) const
{
	return impl->isNull(index);
}


bool MySQLPreparedStatementResultInstanceImpl::getBool(int index, bool* success) const
{
	return impl->getBool(index, success);
}


int MySQLPreparedStatementResultInstanceImpl::getType(int index) const
{
	return impl->getType(index);
}


CString MySQLPreparedStatementResultInstanceImpl::getColumnName(int index) const
{
	return impl->getColumnName(index);
}


int MySQLPreparedStatementResultInstanceImpl::getColumnIndex(const CString& name) const
{
	return impl->getColumnIndex(name);
}


unsigned int MySQLPreparedStatementResultInstanceImpl::getColumnCount() const
{
	return impl->getColumnCount();
}


uint64_t MySQLPreparedStatementResultInstanceImpl::getAffectedRowCount() const
{
	return impl->getAffectedRowCount();
}


