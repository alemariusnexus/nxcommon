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

#ifndef SQL_H_
#define SQL_H_

#include <nxcommon/config.h>
#include "SQLDatabase.h"
#include "SQLPreparedStatement.h"
#include "SQLResult.h"

#ifdef NXCOMMON_SQLITE_ENABLED
#include "SQLiteDriver.h"
#endif

#ifdef NXCOMMON_MYSQL_ENABLED
#include "MySQLDriver.h"
#endif

#ifdef NXCOMMON_PSQL_ENABLED
#include "PSQLDriver.h"
#endif

#endif /* SQL_H_ */
