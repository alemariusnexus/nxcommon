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

#include "SQLitePreparedStatementImpl.h"
#include "sqlbase.h"
#include "../SQLException.h"
#include "sqliteutils.h"
#include "SQLiteResultImpl.h"
#include <unicode/schriter.h>
#include <unicode/uchar.h>
#include <unicode/utf8.h>
#include "../../unicodeutil.h"

using icu::StringCharacterIterator;




SQLitePreparedStatementImpl::SQLitePreparedStatementImpl(SQLiteDatabaseImpl* db)
		: db(db), stmt(NULL)
{
}


SQLitePreparedStatementImpl::~SQLitePreparedStatementImpl()
{
}


void SQLitePreparedStatementImpl::prepare(const UString& query, const UChar** codeLeft)
{
	if (sqlite3_prepare16_v2(db->getSQLiteHandle(), query.get(), query.length()*2, &stmt, (const void**) codeLeft) != SQLITE_OK) {
		ThrowSQLiteException(db->getSQLiteHandle(), "Error preparing SQLite statement", __FILE__, __LINE__);
	}
}


void SQLitePreparedStatementImpl::prepareUTF8(const ByteArray& query, const char** codeLeft)
{
	if (sqlite3_prepare_v2(db->getSQLiteHandle(), query.get(), query.length(), &stmt, codeLeft) != SQLITE_OK) {
		ThrowSQLiteException(db->getSQLiteHandle(), "Error preparing SQLite statement", __FILE__, __LINE__);
	}
}


void SQLitePreparedStatementImpl::prepare(const UString& query)
{
	const UChar* codeLeft = NULL;

	prepare(query, &codeLeft);

	UString leftStr = UString::readAlias(codeLeft);

	if (!leftStr.isEmpty()  &&  !leftStr.isWhitespaceOnly()) {
		sqlite3_finalize(stmt);
		stmt = NULL;

		throw SQLException("Attempt to send multiple statements in a single prepared statement. This is not supported!",
				__FILE__, __LINE__);
	}
}


void SQLitePreparedStatementImpl::prepareUTF8(const ByteArray& query)
{
	const char* codeLeft = NULL;

	prepareUTF8(query, &codeLeft);

	if (!IsWhitespaceOnlyUTF8(ByteArray::readAlias(codeLeft, strlen(codeLeft)))) {
		sqlite3_finalize(stmt);
		stmt = NULL;

		throw SQLException("Attempt to send multiple statements in a single prepared statement. This is not supported!",
				__FILE__, __LINE__);
	}
}


void SQLitePreparedStatementImpl::bindUInt32(size_t index, uint32_t value)
{
	if (sqlite3_bind_int64(stmt, index+1, value) != SQLITE_OK) {
		ThrowSQLiteException(db->getSQLiteHandle(), NULL, __FILE__, __LINE__);
	}
}


void SQLitePreparedStatementImpl::bindInt32(size_t index, int32_t value)
{
	if (sqlite3_bind_int(stmt, index+1, value) != SQLITE_OK) {
		ThrowSQLiteException(db->getSQLiteHandle(), NULL, __FILE__, __LINE__);
	}
}


void SQLitePreparedStatementImpl::bindUInt64(size_t index, uint64_t value)
{
	if (sqlite3_bind_int64(stmt, index+1, (int64_t) value) != SQLITE_OK) {
		ThrowSQLiteException(db->getSQLiteHandle(), NULL, __FILE__, __LINE__);
	}
}


void SQLitePreparedStatementImpl::bindInt64(size_t index, int64_t value)
{
	if (sqlite3_bind_int64(stmt, index+1, value) != SQLITE_OK) {
		ThrowSQLiteException(db->getSQLiteHandle(), NULL, __FILE__, __LINE__);
	}
}


void SQLitePreparedStatementImpl::bindFloat(size_t index, float value)
{
	bindDouble(index, value);
}


void SQLitePreparedStatementImpl::bindDouble(size_t index, double value)
{
	if (sqlite3_bind_double(stmt, index+1, value) != SQLITE_OK) {
		ThrowSQLiteException(db->getSQLiteHandle(), NULL, __FILE__, __LINE__);
	}
}


void SQLitePreparedStatementImpl::bindString(size_t index, const UString& value)
{
	if (sqlite3_bind_text16(stmt, index+1, value.get(), value.length()*2, SQLITE_STATIC) != SQLITE_OK) {
		ThrowSQLiteException(db->getSQLiteHandle(), NULL, __FILE__, __LINE__);
	}
}


void SQLitePreparedStatementImpl::bindStringUTF8(size_t index, const ByteArray& value)
{
	if (sqlite3_bind_text(stmt, index+1, value.get(), value.length(), SQLITE_STATIC) != SQLITE_OK) {
		ThrowSQLiteException(db->getSQLiteHandle(), NULL, __FILE__, __LINE__);
	}
}


void SQLitePreparedStatementImpl::bindBLOB(size_t index, const ByteArray& value)
{
	if (sqlite3_bind_blob(stmt, index+1, value.get(), value.length(), SQLITE_STATIC) != SQLITE_OK) {
		ThrowSQLiteException(db->getSQLiteHandle(), NULL, __FILE__, __LINE__);
	}
}


void SQLitePreparedStatementImpl::bindNull(size_t index)
{
	if (sqlite3_bind_null(stmt, index+1) != SQLITE_OK) {
		ThrowSQLiteException(db->getSQLiteHandle(), NULL, __FILE__, __LINE__);
	}
}


void SQLitePreparedStatementImpl::bindBool(size_t index, bool value)
{
	bindInt32(index, value ? 1 : 0);
}


SQLResultImpl* SQLitePreparedStatementImpl::execute()
{
	int status = sqlite3_step(stmt);

	bool firstStepHasData;

	if (status == SQLITE_DONE) {
		firstStepHasData = false;
	} else if (status == SQLITE_ROW) {
		firstStepHasData = true;
	} else {
		ThrowSQLiteException(db->getSQLiteHandle(), "Error executing SQLite prepared statement", __FILE__, __LINE__);
	}

	return new SQLiteResultImpl(stmt, firstStepHasData, sqlite3_changes(db->getSQLiteHandle()));
}


void SQLitePreparedStatementImpl::finalize()
{
	sqlite3_finalize(stmt);
}


void SQLitePreparedStatementImpl::reset()
{
	if (sqlite3_reset(stmt) != SQLITE_OK) {
		ThrowSQLiteException(db->getSQLiteHandle(), "Error resetting prepared statement", __FILE__, __LINE__);
	}
}





