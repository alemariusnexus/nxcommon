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

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <nxcommon-test/config.h>
#include <gtest/gtest.h>
#include <nxcommon/CString.h>
#include <nxcommon/sql/SQLDatabase.h>
#include "printhelpers.h"


extern CString testRootPath;

//extern SQLDatabase mysqlDb;

extern CString mysqlHost;
extern CString mysqlUser;
extern CString mysqlPass;
extern CString mysqlDb;
extern unsigned int mysqlPort;

extern CString psqlHost;
extern CString psqlUser;
extern CString psqlPass;
extern CString psqlDb;
extern unsigned int psqlPort;

#endif /* GLOBAL_H_ */
