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

#ifndef PSQLDATABASEIMPL_H_
#define PSQLDATABASEIMPL_H_

#include "SQLDatabaseImpl.h"
#include <postgresql/libpq-fe.h>



class PSQLDatabaseImpl : public SQLDatabaseImpl
{
public:
	PSQLDatabaseImpl(PGconn* pq);
	virtual ~PSQLDatabaseImpl();
	virtual SQLPreparedStatementImpl* createPreparedStatement();
	virtual SQLResultImpl* sendQuery(const UString& query);
	virtual SQLResultImpl* sendQueryUTF8(const CString& query);
	virtual uint64_t getLastInsertID() const;
	virtual UString escapeString(const UString& str) const;
	virtual CString escapeStringUTF8(const CString& str) const;
	virtual void setTimeout(uint64_t timeoutMillis);

	PGresult* sendQueryRaw(const char* query);

	timeval* getTimevalTimeout() { return timeoutPtr; }

	PGconn* getPGconn() { return pq; }
	//PSQLDataTypeHandlerRegistry getDataTypeHandlerRegistry() { return dtypeReg; }

private:
	PGconn* pq;
	//PSQLDataTypeHandlerRegistry dtypeReg;
	timeval* timeoutPtr;
	timeval timeout;
};

#endif /* PSQLDATABASEIMPL_H_ */
