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

#ifndef MYSQLDATABASEIMPL_H_
#define MYSQLDATABASEIMPL_H_

#include <nxcommon/config.h>
#include "SQLDatabaseImpl.h"
#include "../../CString.h"
#include "../SQLException.h"
#include <mysql/mysql.h>



class MySQLDatabaseImpl : public SQLDatabaseImpl
{
public:
	MySQLDatabaseImpl(MYSQL* mysql, const CString& host, const CString& user, const CString& pass = CString(),
			const CString& db = CString(), unsigned int port = 0);
	MySQLDatabaseImpl(const CString& host, const CString& user, const CString& pass = CString(),
			const CString& db = CString(), unsigned int port = 0);
	virtual ~MySQLDatabaseImpl();
	virtual SQLPreparedStatementImpl* createPreparedStatement();
	virtual SQLResultImpl* sendQuery(const UString& query);
	virtual SQLResultImpl* sendQueryUTF8(const CString& query);
	virtual uint64_t getLastInsertID() const;
	virtual UString escapeString(const UString& str) const;
	virtual CString escapeStringUTF8(const CString& str) const;

	MYSQL* getMySQLHandle() { return mysql; }

private:
	void init(MYSQL* mysql, const CString& host, const CString& user, const CString& pass,
			const CString& db, unsigned int port);

private:
	MYSQL* mysql;
};

#endif /* MYSQLDATABASEIMPL_H_ */
