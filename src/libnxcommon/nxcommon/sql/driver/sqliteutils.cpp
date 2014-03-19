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

#include "sqliteutils.h"
#include <cstring>
#include <cstdio>




void ThrowSQLiteException(sqlite3* sqlite, const char* msg, const char* srcFile, int srcLine)
{
	size_t msgLen = msg ? strlen(msg) : 0;
	const char* sqliteErr = sqlite3_errmsg(sqlite);
	char* errmsg = new char[msgLen + strlen(sqliteErr) + 4];
	if (msg)
		sprintf(errmsg, "%s: %s", msg, sqliteErr);
	else
		sprintf(errmsg, "%s", sqliteErr);
	SQLException ex(errmsg, srcFile, srcLine);
	delete[] errmsg;
	throw ex;
}

