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

#ifndef SQLITEDATABASEIMPL_H_
#define SQLITEDATABASEIMPL_H_

#include "SQLDatabaseImpl.h"
#include "sqlite3.h"
#include "../../file/File.h"



class SQLiteDatabaseImpl : public SQLDatabaseImpl
{
public:
	SQLiteDatabaseImpl(const File& file, int flags);
	virtual ~SQLiteDatabaseImpl();
	virtual SQLPreparedStatementImpl* createPreparedStatement();
	virtual SQLResultImpl* sendQuery(const UString& query);
	virtual SQLResultImpl* sendQueryUTF8(const CString& query);
	sqlite3* getSQLiteHandle() { return sqlite; }
	virtual uint64_t getLastInsertID() const;
	virtual UString escapeString(const UString& str) const;
	virtual CString escapeStringUTF8(const CString& str) const;

private:
	sqlite3* sqlite;
};

#endif /* SQLITEDATABASEIMPL_H_ */
