/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef SCRIPTSQLDATABASE_H_
#define SCRIPTSQLDATABASE_H_

#include "../ScriptObject.h"
#include "../../sql/SQLDatabase.h"
#include <memory>

using std::shared_ptr;



class ScriptSQLDatabase : public ScriptObject
{
	SCRIPTOBJECT_DECLARE_HELPER_METHODS(ScriptSQLDatabase)

public:
	static void registerClass(lua_State* lua);
	static shared_ptr<ScriptSQLDatabase> create(lua_State* lua, const SQLDatabase& db);

public:
	void setErrorMessage(const CString& msg = CString());
	CString getErrorMessage() const { return lastErrorMsg; }

private:
	ScriptSQLDatabase(lua_State* lua, const SQLDatabase& db) : ScriptObject(lua), db(db) {}

private:
	SQLDatabase db;
	CString lastErrorMsg;
};

#endif /* SCRIPTSQLDATABASE_H_ */
