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

#include "ScriptSQLResult.h"
#include "../../sql/SQLException.h"





void ScriptSQLResult::registerClass(lua_State* lua)
{
	createClass(lua);

	luaS_createmethod(lua, "nextRecord", [] (lua_State* lua) {
		auto sres = getSelf(lua);

		auto db = sres->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			lua_pushboolean(lua, sres->res.nextRecord() ? 1 : 0);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		return 1;
	});

	luaS_createmethod(lua, "getInt", [] (lua_State* lua) {
		auto sres = getSelf(lua);
		lua_Unsigned idx = luaL_checkunsigned(lua, 2);

		auto db = sres->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			int64_t val = sres->res.getInt64(idx);
			lua_pushinteger(lua, val);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		return 1;
	});

	luaS_createmethod(lua, "getUInt", [] (lua_State* lua) {
		auto sres = getSelf(lua);
		lua_Unsigned idx = luaL_checkunsigned(lua, 2);

		auto db = sres->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			uint64_t val = sres->res.getUInt64(idx);
			lua_pushunsigned(lua, val);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		return 1;
	});

	luaS_createmethod(lua, "getFloat", [] (lua_State* lua) {
		auto sres = getSelf(lua);
		lua_Unsigned idx = luaL_checkunsigned(lua, 2);

		auto db = sres->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			double val = sres->res.getDouble(idx);
			lua_pushnumber(lua, val);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		return 1;
	});

	luaS_createmethod(lua, "getString", [] (lua_State* lua) {
		auto sres = getSelf(lua);
		lua_Unsigned idx = luaL_checkunsigned(lua, 2);

		auto db = sres->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			ByteArray barr = sres->res.getStringUTF8(idx);
			lua_pushlstring(lua, barr.get(), barr.getSize()-1);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		return 1;
	});

	luaS_createmethod(lua, "getStringUTF16", [] (lua_State* lua) {
		auto sres = getSelf(lua);
		lua_Unsigned idx = luaL_checkunsigned(lua, 2);

		auto db = sres->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			UString str = sres->res.getString(idx);
			lua_pushlstring(lua, (const char*) str.get(), str.getSize()*2);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		return 1;
	});

	luaS_createmethod(lua, "getColumnName", [] (lua_State* lua) {
		auto sres = getSelf(lua);
		lua_Unsigned idx = luaL_checkunsigned(lua, 2);

		CString name = sres->res.getColumnName(idx);

		lua_pushlstring(lua, name.get(), name.getLength());

		return 1;
	});

	luaS_createmethod(lua, "getColumnIndex", [] (lua_State* lua) {
		auto sres = getSelf(lua);
		const char* name = luaL_checkstring(lua, 2);

		lua_pushinteger(lua, sres->res.getColumnIndex(name));

		return 1;
	});

	luaS_createmethod(lua, "getColumnCount", [] (lua_State* lua) {
		auto sres = getSelf(lua);

		lua_pushunsigned(lua, sres->res.getColumnCount());

		return 1;
	});

	luaS_createmethod(lua, "getAffectedRowCount", [] (lua_State* lua) {
		auto sres = getSelf(lua);

		auto db = sres->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			uint64_t rc = sres->res.getAffectedRowCount();
			lua_pushunsigned(lua, rc);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		return 1;
	});

	luaS_createmethod(lua, "getDatabase", [] (lua_State* lua) {
		auto sres = getSelf(lua);
		sres->getDatabase();
		return 1;
	});
}


shared_ptr<ScriptSQLResult> ScriptSQLResult::create(lua_State* lua, const SQLResult& res)
{
	lua_getglobal(lua, CLASS_NAME);
	ScriptSQLResult* sres = new ScriptSQLResult(lua, res);
	return createInstance(lua, sres);
}


shared_ptr<ScriptSQLDatabase> ScriptSQLResult::getDatabase() const
{
	SQLDatabase db = res.getDatabase();
	return ScriptSQLDatabase::create(lua, db);
}
