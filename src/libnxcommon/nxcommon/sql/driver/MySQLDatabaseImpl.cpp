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

#include "MySQLDatabaseImpl.h"
#include "MySQLPreparedStatementImpl.h"
#include "MySQLResultImpl.h"
#include "mysqlutils.h"




MySQLDatabaseImpl::MySQLDatabaseImpl(MYSQL* mysql, const CString& host, const CString& user, const CString& pass,
			const CString& db, unsigned int port)
{
	init(mysql, host, user, pass, db, port);
}


MySQLDatabaseImpl::MySQLDatabaseImpl(const CString& host, const CString& user, const CString& pass,
			const CString& db, unsigned int port)
{
	MYSQL* mysql = mysql = mysql_init(NULL);

	init(mysql, host, user, pass, db, port);
}


void MySQLDatabaseImpl::init(MYSQL* mysql, const CString& host, const CString& user, const CString& pass,
			const CString& db, unsigned int port)
{
	this->mysql = mysql;

	setCapabilities(SQL_CAP_LAST_INSERT_ID);

	if (!mysql_real_connect(mysql, host.get(), user.isNull() ? NULL : user.get(), pass.isNull() ? NULL : pass.get(),
			db.isNull() ? NULL : db.get(), port, NULL, CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS)) {
		mysql_close(mysql);
		ThrowMySQLException(mysql, "Error connecting to database", __FILE__, __LINE__);
	}

	mysql_set_character_set(mysql, "utf8mb4");
}


MySQLDatabaseImpl::~MySQLDatabaseImpl()
{
	mysql_close(mysql);
}


SQLPreparedStatementImpl* MySQLDatabaseImpl::createPreparedStatement()
{
	return new MySQLPreparedStatementImpl(this);
}


#ifdef NXCOMMON_UNICODE_ENABLED
SQLResultImpl* MySQLDatabaseImpl::sendQuery(const UString& query)
{
	return sendQueryUTF8(query.toUTF8());
}
#endif


SQLResultImpl* MySQLDatabaseImpl::sendQueryUTF8(const CString& query)
{
	if (mysql_real_query(mysql, query.get(), query.length()) != 0) {
		ThrowMySQLException(mysql, "Error executing query", __FILE__, __LINE__);
	}

	int status;

	while ((status = mysql_more_results(mysql))) {
		MYSQL_RES* res = mysql_use_result(mysql);

		if (res) {
			while (mysql_fetch_row(res));

			mysql_free_result(res);
		}

		mysql_next_result(mysql);
	}

	MYSQL_RES* res = mysql_store_result(mysql);

	return new MySQLResultImpl(this, res);
}


uint64_t MySQLDatabaseImpl::getLastInsertID() const
{
	uint64_t id = mysql_insert_id(mysql);
	return id == 0 ? 0 : id + mysql_affected_rows(mysql) - 1;
}


#ifdef NXCOMMON_UNICODE_ENABLED
UString MySQLDatabaseImpl::escapeString(const UString& str) const
{
	return UString::fromUTF8(escapeStringUTF8(str.toUTF8()));
}
#endif


CString MySQLDatabaseImpl::escapeStringUTF8(const CString& str) const
{
	size_t size = str.length()*2 + 1;
	char* outBuf = new char[size];

	unsigned int outLen = mysql_real_escape_string(mysql, outBuf, str.get(), str.length());

	return CString::from(outBuf, outLen, size);
}
