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

#include "PSQLDatabaseImpl.h"
#include "PSQLPreparedStatementImpl.h"
#include "PSQLResultImpl.h"
#include "../SQLException.h"




PSQLDatabaseImpl::PSQLDatabaseImpl(PGconn* pq)
		: pq(pq), timeoutPtr(NULL)
{
	setCapabilities(SQL_CAP_TIMEOUT);
}


PSQLDatabaseImpl::~PSQLDatabaseImpl()
{
	PQfinish(pq);
}


SQLPreparedStatementImpl* PSQLDatabaseImpl::createPreparedStatement()
{
	return new PSQLPreparedStatementImpl(this);
}


SQLResultImpl* PSQLDatabaseImpl::sendQuery(const UString& query)
{
	return sendQueryUTF8(query.toUTF8());
}


SQLResultImpl* PSQLDatabaseImpl::sendQueryUTF8(const CString& query)
{
	PGresult* res = sendQueryRaw(query.get());

	int status = PQresultStatus(res);
	if (status != PGRES_COMMAND_OK  &&  status != PGRES_TUPLES_OK  &&  status != PGRES_EMPTY_QUERY) {
		SQLException ex = CreatePSQLResultException(res, "Error executing prepared statement", __FILE__, __LINE__);
		PQclear(res);
		throw ex;
	}

	return new PSQLResultImpl(res);
}


PGresult* PSQLDatabaseImpl::sendQueryRaw(const char* query)
{
	if (!PQsendQuery(pq, query)) {
		throw CreatePSQLConnException(pq, "Error sending query", __FILE__, __LINE__);
	}

	FlushPSQL(pq, timeoutPtr);

	PGresult* res = FetchAsyncPSQLResult(pq, timeoutPtr);

	return res;
}


uint64_t PSQLDatabaseImpl::getLastInsertID() const
{
	throw SQLException("SQLDatabaseImpl::getLastInsertID() is not implemented in PostgreSQL. Use an 'INSERT ... RETURNING id'"
			" query instead!", __FILE__, __LINE__);
}


UString PSQLDatabaseImpl::escapeString(const UString& str) const
{
	return UString::fromUTF8(escapeStringUTF8(str.toUTF8()));
}


CString PSQLDatabaseImpl::escapeStringUTF8(const CString& str) const
{
	size_t size = str.length()*2 + 1;
	char* outBuf = new char[size];

	size_t outLen = PQescapeStringConn(pq, outBuf, str.get(), str.length(), NULL);

	return CString::from(outBuf, outLen, size);
}


void PSQLDatabaseImpl::setTimeout(uint64_t timeoutMillis)
{
	timeout.tv_sec = timeoutMillis/1000;
	timeout.tv_usec = (timeoutMillis%1000) * 1000;

	if (timeoutMillis != 0) {
		timeoutPtr = &timeout;
	} else {
		timeoutPtr = NULL;
	}
}
