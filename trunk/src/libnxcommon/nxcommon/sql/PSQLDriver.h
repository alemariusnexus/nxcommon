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

#ifndef PSQLDRIVER_H_
#define PSQLDRIVER_H_

#include "SQLDatabase.h"
#include "driver/PSQLDatabaseImpl.h"
#include <map>

using std::map;



class PSQLDriver
{
private:
	static PSQLDriver inst;

public:
	static PSQLDriver* getInstance();

public:
	SQLDatabase openDatabase(const CString& host, const CString& user, const CString& pass = CString(),
			const CString& db = CString(), unsigned int port = 0, unsigned int timeoutSecs = 0);
	SQLDatabase openDatabase(const map<CString, CString>& params);

private:
	PSQLDriver() {}
};

#endif /* PSQLDRIVER_H_ */
