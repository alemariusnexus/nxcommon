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





SQLiteDatabaseImpl::SQLiteDatabaseImpl(const File& file, int flags)
{
	setCapabilities(SQL_CAP_LAST_INSERT_ID);

	sqlite3_open_v2(file.toString().get(), &sqlite, flags, NULL);
}


SQLiteDatabaseImpl::~SQLiteDatabaseImpl()
{
	sqlite3_close(sqlite);
}


SQLPreparedStatementImpl* SQLiteDatabaseImpl::createPreparedStatement()
{
	return new SQLitePreparedStatementImpl(this);
}


SQLResultImpl* SQLiteDatabaseImpl::sendQuery(const UString& query)
{
	SQLitePreparedStatementImpl* stmt = (SQLitePreparedStatementImpl*) createPreparedStatement();

	const UChar* codeLeft = NULL;

	stmt->prepare(query, &codeLeft);
	SQLResultImpl* res = stmt->execute();

	while (codeLeft) {

		UString nextQuery = UString::readAlias(codeLeft, query.length() - (codeLeft-query.get()));

		if (nextQuery.isEmpty()  ||  nextQuery.isWhitespaceOnly()) {
			break;
		}

		delete res;

		stmt->prepare(nextQuery, &codeLeft);
		res = stmt->execute();
	}

	return res;
}


SQLResultImpl* SQLiteDatabaseImpl::sendQueryUTF8(const ByteArray& query)
{
	SQLitePreparedStatementImpl* stmt = (SQLitePreparedStatementImpl*) createPreparedStatement();

	const char* codeLeft;

	stmt->prepareUTF8(query, &codeLeft);
	SQLResultImpl* res = stmt->execute();

	while (codeLeft) {
		delete res;

		stmt->prepareUTF8(query, &codeLeft);
		res = stmt->execute();
	}

	return res;
}


uint64_t SQLiteDatabaseImpl::getLastInsertID() const
{
	return sqlite3_last_insert_rowid(sqlite);
}


UString SQLiteDatabaseImpl::escapeString(const UString& str) const
{
	return UString::fromUTF8(escapeStringUTF8(str.toUTF8()));
}


ByteArray SQLiteDatabaseImpl::escapeStringUTF8(const ByteArray& str) const
{
	size_t size = str.length()*2 + 1;
	char* outBuf = new char[size];

	sqlite3_snprintf(size, outBuf, "%q", str.get());

	return ByteArray::from(outBuf, strlen(outBuf), size);
}
