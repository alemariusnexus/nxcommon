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

#include "PSQLDriver.h"
#include "driver/psqlutils.h"
#include <utility>

using std::pair;




PSQLDriver PSQLDriver::inst;



PSQLDriver* PSQLDriver::getInstance()
{
	return &inst;
}


SQLDatabase PSQLDriver::openDatabase(const CString& host, const CString& user, const CString& pass,
			const CString& db, unsigned int port, unsigned int timeoutSecs)
{
	char* paramStr = new char[1024 + host.length() + user.length() + pass.length() + db.length()];

	char portStr[32];

	if (port != 0) {
		sprintf(portStr, "%u", port);
	} else {
		portStr[0] = '\0';
	}

	char timeoutStr[32];

	if (timeoutSecs != 0) {
		sprintf(timeoutStr, "%u", timeoutSecs);
	} else {
		timeoutStr[0] = '\0';
	}

	sprintf(paramStr,
			"host=%s user=%s%s%s dbname=%s%s%s client_encoding=UTF8%s%s",
			host.get(),
			user.get(),
			pass.isNull() ? "" : " password=", pass.isNull() ? "" : pass.get(),
			db.get(),
			port == 0 ? "" : " port=", portStr,
			timeoutSecs != 0 ? " connect_timeout=" : "", timeoutStr);

	PGconn* pq = PQconnectdb(paramStr);

	delete[] paramStr;

	int status = PQstatus(pq);
	if (status != CONNECTION_OK) {
		SQLException ex = CreatePSQLConnException(pq, "Error connecting to PostgreSQL database", __FILE__, __LINE__);
		PQfinish(pq);
		throw ex;
	}

	PQsetnonblocking(pq, true);

	PSQLDatabaseImpl* impl = new PSQLDatabaseImpl(pq);
	SQLDatabase sdb(impl);
	return sdb;
}


SQLDatabase PSQLDriver::openDatabase(const map<CString, CString>& params)
{
	const char** keys = new const char*[params.size() + 1];
	const char** values = new const char*[params.size() + 1];

	size_t i = 0;
	for (pair<CString, CString> p : params) {
		keys[i] = p.first.get();
		values[i] = p.second.get();
		i++;
	}

	keys[params.size()] = NULL;
	values[params.size()] = NULL;

	PGconn* pq = PQconnectdbParams(keys, values, 1);

	delete[] keys;
	delete[] values;

	int status = PQstatus(pq);
	if (status != CONNECTION_OK) {
		SQLException ex = CreatePSQLConnException(pq, "Error connecting to PostgreSQL database", __FILE__, __LINE__);
		PQfinish(pq);
		throw ex;
	}

	PQsetnonblocking(pq, true);

	PSQLDatabaseImpl* impl = new PSQLDatabaseImpl(pq);
	return SQLDatabase(impl);
}
