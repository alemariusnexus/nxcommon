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

#include "MySQLDriver.h"




MySQLDriver MySQLDriver::inst;



MySQLDriver* MySQLDriver::getInstance()
{
	return &inst;
}


SQLDatabase MySQLDriver::openDatabase(const CString& host, const CString& user, const CString& pass,
			const CString& db, unsigned int port)
{
	MySQLDatabaseImpl* impl = new MySQLDatabaseImpl(host, user, pass, db, port);
	return SQLDatabase(impl);
}
