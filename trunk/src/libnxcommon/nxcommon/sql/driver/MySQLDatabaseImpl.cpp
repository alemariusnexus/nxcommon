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
#include "mysqlutils.h"




MySQLDatabaseImpl::MySQLDatabaseImpl(const CString& host, const CString& user, const CString& pass,
			const CString& db, unsigned int port)
{
	mysql = mysql_init(NULL);
	if (!mysql_real_connect(mysql, host.get(), user.isNull() ? NULL : user.get(), pass.isNull() ? NULL : pass.get(),
			db.isNull() ? NULL : db.get(), port, NULL, 0)) {
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


uint64_t MySQLDatabaseImpl::getLastInsertID() const
{
	uint64_t id = mysql_insert_id(mysql);
	return id == 0 ? 0 : id + mysql_affected_rows(mysql) - 1;
}
