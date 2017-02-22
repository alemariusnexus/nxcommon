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

#include "SQLiteDatabaseImpl.h"
#include "SQLitePreparedStatementImpl.h"
#include "SQLiteResultImpl.h"





SQLiteDatabaseImpl::SQLiteDatabaseImpl(const File& file, int flags)
{
	setCapabilities(SQL_CAP_LAST_INSERT_ID);

	sqlite3_open_v2(file.toString().get(), &sqlite, flags, NULL);
}


SQLiteDatabaseImpl::~SQLiteDatabaseImpl()
{
	int status;
	if ((status = sqlite3_close(sqlite)) != SQLITE_OK) {
		fprintf(stderr, "ERROR: sqlite3_close() returned error code in SQLiteDatabaseImpl::~SQLiteDatabaseImpl(): %d\n",
				status);
	}
}


SQLPreparedStatementImpl* SQLiteDatabaseImpl::createPreparedStatement()
{
	return new SQLitePreparedStatementImpl(this);
}


#ifdef NXCOMMON_UNICODE_ENABLED
SQLResultImpl* SQLiteDatabaseImpl::sendQuery(const UString& query)
{
	SQLitePreparedStatementImpl* stmt = (SQLitePreparedStatementImpl*) createPreparedStatement();

	const UChar* codeLeft = NULL;

	stmt->prepare(query, &codeLeft);
	SQLiteResultImpl* res = (SQLiteResultImpl*) stmt->execute();

	res->ownedStmt = stmt;

	while (codeLeft) {

		UString nextQuery = UString::readAlias(codeLeft, query.length() - (codeLeft-query.get()));

		if (nextQuery.isEmpty()  ||  nextQuery.isWhitespaceOnly()) {
			break;
		}

		delete res;

		stmt->prepare(nextQuery, &codeLeft);
		res = (SQLiteResultImpl*) stmt->execute();
	}

	return res;
}
#endif


SQLResultImpl* SQLiteDatabaseImpl::sendQueryUTF8(const CString& query)
{
	SQLitePreparedStatementImpl* stmt = (SQLitePreparedStatementImpl*) createPreparedStatement();

	const char* codeLeft;

	stmt->prepareUTF8(query, &codeLeft);
	SQLiteResultImpl* res = (SQLiteResultImpl*) stmt->execute();

	res->ownedStmt = stmt;

	while (codeLeft) {
		delete res;

		stmt->prepareUTF8(query, &codeLeft);
		res = (SQLiteResultImpl*) stmt->execute();
	}

	return res;
}


uint64_t SQLiteDatabaseImpl::getLastInsertID() const
{
	return sqlite3_last_insert_rowid(sqlite);
}


#ifdef NXCOMMON_UNICODE_ENABLED
UString SQLiteDatabaseImpl::escapeString(const UString& str) const
{
	return UString::fromUTF8(escapeStringUTF8(str.toUTF8()));
}
#ifdef NXCOMMON_UNICODE_ENABLED


CString SQLiteDatabaseImpl::escapeStringUTF8(const CString& str) const
{
	size_t size = str.length()*2 + 1;
	char* outBuf = new char[size];

	sqlite3_snprintf(size, outBuf, "%q", str.get());

	return CString::from(outBuf, strlen(outBuf), size);
}
