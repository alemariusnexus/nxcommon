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

#include "ScriptSQLPreparedStatement.h"
#include "ScriptSQLResult.h"
#include "../../sql/SQLException.h"
#include "../../sql/SQLResult.h"
#include <cmath>



SCRIPTOBJECT_DEFINE_HELPER_METHODS(ScriptSQLPreparedStatement, "SQLPreparedStatement")



void ScriptSQLPreparedStatement::registerClass(lua_State* lua)
{
	createClass(lua);

	luaS_createmethod(lua, "bindInt", [] (lua_State* lua) {
		auto sstmt = getSelf(lua);
		lua_Unsigned idx = luaL_checkunsigned(lua, 2);
		lua_Integer val = luaL_checkinteger(lua, 3);

		auto db = sstmt->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			sstmt->stmt.bindInt64(idx, val);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		lua_pushvalue(lua, 1);
		return 0;
	});

	luaS_createmethod(lua, "bindUInt", [] (lua_State* lua) {
		auto sstmt = getSelf(lua);
		lua_Unsigned idx = luaL_checkunsigned(lua, 2);
		lua_Unsigned val = luaL_checkunsigned(lua, 3);

		auto db = sstmt->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			sstmt->stmt.bindUInt64(idx, val);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		lua_pushvalue(lua, 1);
		return 1;
	});

	luaS_createmethod(lua, "bindFloat", [] (lua_State* lua) {
		auto sstmt = getSelf(lua);
		lua_Unsigned idx = luaL_checkunsigned(lua, 2);
		lua_Number val = luaL_checknumber(lua, 3);

		auto db = sstmt->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			sstmt->stmt.bindDouble(idx, val);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		lua_pushvalue(lua, 1);
		return 1;
	});

	luaS_createmethod(lua, "bindString", [] (lua_State* lua) {
		auto sstmt = getSelf(lua);
		lua_Unsigned idx = luaL_checkunsigned(lua, 2);

		size_t len;
		const char* buf = luaL_checklstring(lua, 3, &len);
		CString cstr(buf, len);

		auto db = sstmt->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			sstmt->stmt.bindStringUTF8(idx, cstr);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		lua_pushvalue(lua, 1);
		return 1;
	});

	luaS_createmethod(lua, "bindStringUTF16", [] (lua_State* lua) {
		auto sstmt = getSelf(lua);
		lua_Unsigned idx = luaL_checkunsigned(lua, 2);

		size_t len;
		const char* buf = luaL_checklstring(lua, 3, &len);
		UString str((const UChar*) buf, len/2);

		auto db = sstmt->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			sstmt->stmt.bindString(idx, str);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		lua_pushvalue(lua, 1);
		return 1;
	});

	luaS_createmethod(lua, "bindNull", [] (lua_State* lua) {
		auto sstmt = getSelf(lua);
		lua_Unsigned idx = luaL_checkunsigned(lua, 2);

		auto db = sstmt->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			sstmt->stmt.bindNull(idx);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		lua_pushvalue(lua, 1);
		return 1;
	});

	luaS_createmethod(lua, "bind", [] (lua_State* lua) {
		auto sstmt = getSelf(lua);

		SQLPreparedStatement stmt = sstmt->stmt;

		auto db = sstmt->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushvalue(lua, 1);
			return 1;
		}

		lua_pushnil(lua);

		while (lua_next(lua, 1) != 0) {
			lua_Unsigned idx = luaL_checkunsigned(lua, -2);

			int type = lua_type(lua, -1);

			if (type == LUA_TNIL) {
				stmt.bindNull(idx);
			} else if (type == LUA_TBOOLEAN) {
				stmt.bindBool(idx, lua_toboolean(lua, -1));
			} else if (type == LUA_TNUMBER) {
				lua_Number num = lua_tonumber(lua, -1);
				double numFloor = floor(num);

				if (num == numFloor) {
					// It's an integer!

					if (num >= 0.0) {
						stmt.bindUInt64(idx, (uint64_t) numFloor);
					} else {
						stmt.bindInt64(idx, (int64_t) numFloor);
					}
				} else {
					stmt.bindDouble(idx, num);
				}
			} else if (type == LUA_TSTRING) {
				size_t strLen;
				const char* str = lua_tolstring(lua, -1, &strLen);

				stmt.bindStringUTF8(idx, CString(str, strLen));
			} else {
				CString errmsg("Invalid bind value type at index ");
				errmsg << idx;
				db->setErrorMessage(errmsg);

				lua_pushvalue(lua, 1);
				return 1;
			}

			lua_pop(lua, 1);
		}

		lua_pushvalue(lua, 1);
		return 1;
	});

	luaS_createmethod(lua, "prepare", [] (lua_State* lua) {
		auto sstmt = getSelf(lua);

		size_t len;
		const char* buf = luaL_checklstring(lua, 2, &len);
		CString cstr(buf, len);

		auto db = sstmt->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			sstmt->stmt.prepareUTF8(cstr);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		return 0;
	});

	luaS_createmethod(lua, "prepareUTF16", [] (lua_State* lua) {
		auto sstmt = getSelf(lua);

		size_t len;
		const char* buf = luaL_checklstring(lua, 2, &len);
		UString str((const UChar*) buf, len/2);

		auto db = sstmt->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			sstmt->stmt.prepare(str);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		return 0;
	});

	luaS_createmethod(lua, "execute", [] (lua_State* lua) {
		auto sstmt = getSelf(lua);

		auto db = sstmt->getDatabase();

		CString errmsg = db->getErrorMessage();

		if (!errmsg.isNull()) {
			lua_pushnil(lua);
			return 1;
		}

		try {
			SQLResult res = sstmt->stmt.execute();
			ScriptSQLResult::create(lua, res);
		} catch (SQLException& ex) {
			db->setErrorMessage(ex.getMessage());
			lua_pushnil(lua);
			return 1;
		}

		return 1;
	});

	luaS_createmethod(lua, "getDatabase", [] (lua_State* lua) {
		auto sstmt = getSelf(lua);
		sstmt->getDatabase();
		return 1;
	});
}


shared_ptr<ScriptSQLPreparedStatement> ScriptSQLPreparedStatement::create(lua_State* lua, const SQLPreparedStatement& stmt)
{
	lua_getglobal(lua, CLASS_NAME);
	ScriptSQLPreparedStatement* sstmt = new ScriptSQLPreparedStatement(lua, stmt);
	return createInstance(lua, sstmt);
}


shared_ptr<ScriptSQLDatabase> ScriptSQLPreparedStatement::getDatabase() const
{
	SQLDatabase db = stmt.getDatabase();
	return ScriptSQLDatabase::create(lua, db);
}
