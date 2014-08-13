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

#include "ScriptSQLDatabase.h"
#include "../../sql/SQLPreparedStatement.h"
#include "ScriptSQLPreparedStatement.h"
#include "ScriptSQLResult.h"
#include "../../sql/SQLException.h"



SCRIPTOBJECT_DEFINE_HELPER_METHODS(ScriptSQLDatabase, "SQLDatabase")




void ScriptSQLDatabase::registerClass(lua_State* lua)
{
	createClass(lua);


	luaS_createmethod(lua, "createPreparedStatement", [] (lua_State* lua) {
		auto sdb = getSelf(lua);

		CString errmsg = sdb->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		SQLPreparedStatement stmt;

		try {
			if (lua_gettop(lua) > 1) {
				size_t len;
				const char* buf = luaL_checklstring(lua, 2, &len);
				ByteArray barr(buf, len+1);
				stmt = sdb->db.createPreparedStatementUTF8(barr);
			} else {
				stmt = sdb->db.createPreparedStatement();
			}

			ScriptSQLPreparedStatement::create(lua, stmt);
		} catch (SQLException& ex) {
			sdb->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		return 1;
	});

	luaS_createmethod(lua, "sendQuery", [] (lua_State* lua) {
		auto sdb = getSelf(lua);

		size_t len;
		const char* buf = luaL_checklstring(lua, 2, &len);
		ByteArray barr(buf, len+1);

		CString errmsg = sdb->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			SQLResult res = sdb->db.sendQueryUTF8(barr);
			ScriptSQLResult::create(lua, res);
		} catch (SQLException& ex) {
			sdb->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		return 1;
	});

	luaS_createmethod(lua, "getLastInsertID", [] (lua_State* lua) {
		auto sdb = getSelf(lua);

		CString errmsg = sdb->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		uint64_t id = 0;
		try {
			id = sdb->db.getLastInsertID();
		} catch (SQLException& ex) {
			sdb->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		lua_pushunsigned(lua, id);
		return 1;
	});

	luaS_createmethod(lua, "getErrorMessage", [] (lua_State* lua) {
		auto sdb = getSelf(lua);

		CString errmsg = sdb->getErrorMessage();

		if (errmsg.isNull()) {
			lua_pushnil(lua);
		} else {
			lua_pushstring(lua, errmsg.get());
		}

		return 1;
	});

	luaS_createmethod(lua, "clearErrorMessage", [] (lua_State* lua) {
		auto sdb = getSelf(lua);

		CString errmsg = sdb->getErrorMessage();

		if (errmsg.isNull()) {
			lua_pushnil(lua);
		} else {
			lua_pushstring(lua, errmsg.get());
			sdb->setErrorMessage();
		}

		return 1;
	});

	luaS_createmethod(lua, "hasErrorMessage", [] (lua_State* lua) {
		auto sdb = getSelf(lua);
		CString errmsg = sdb->getErrorMessage();
		lua_pushboolean(lua, errmsg.isNull() ? 0 : 1);
		return 1;
	});
}


shared_ptr<ScriptSQLDatabase> ScriptSQLDatabase::create(lua_State* lua, const SQLDatabase& db)
{
	lua_getglobal(lua, CLASS_NAME);
	ScriptSQLDatabase* sdb = new ScriptSQLDatabase(lua, db);
	return createInstance(lua, sdb);
}


void ScriptSQLDatabase::setErrorMessage(const CString& msg)
{
	lastErrorMsg = msg;
}
