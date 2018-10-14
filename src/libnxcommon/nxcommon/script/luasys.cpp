/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "luasys.h"
#include "../exception.h"
#include <res_scriptsys_script.h>
#include <res_nxutil_ffi.h>
#include <res_nxutil_api.h>
#include <res_log_ffi.h>
#include <res_log_api.h>
#include <res_mathutil_script.h>
#include <res_vector_script.h>
#include <res_matrix_script.h>

#ifdef NXCOMMON_OPENGL_ENABLED
#include <res_Camera_ffi.h>
#include <res_Camera_api.h>
#endif


bool _luasysForceKeepSymbolDummyCalled = false;
void _LuasysForceKeepSymbolDummy();



void luaS_opencorelibs(lua_State* lua)
{
	luaL_loadbuffer(lua, (const char*) res_scriptsys_script_data, res_scriptsys_script_size, "scriptsys.lua");
	lua_pcall(lua, 0, 0, 0);

	luaL_loadbuffer(lua, (const char*) res_mathutil_script_data, res_mathutil_script_size, "mathutil.lua");
	lua_pcall(lua, 0, 0, 0);

	luaL_loadbuffer(lua, (const char*) res_vector_script_data, res_vector_script_size, "vector.lua");
	lua_pcall(lua, 0, 0, 0);

	luaL_loadbuffer(lua, (const char*) res_matrix_script_data, res_matrix_script_size, "matrix.lua");
	lua_pcall(lua, 0, 0, 0);

#ifdef NXCOMMON_OPENGL_ENABLED
	luaS_loadffimodule(lua, "camera", res_Camera_ffi_asCString(), res_Camera_api_asCString());
#endif

	luaS_loadffimodule(lua, "nxutil", res_nxutil_ffi_asCString(), res_nxutil_api_asCString());
	luaS_loadffimodule(lua, "log", res_log_ffi_asCString(), res_log_api_asCString());

	if (!_luasysForceKeepSymbolDummyCalled) {
		_LuasysForceKeepSymbolDummy();
		_luasysForceKeepSymbolDummyCalled = true;
	}
}


void luaS_newclass(lua_State* lua, bool inherit)
{
	lua_getglobal(lua, "CreateClass");

	if (inherit)
		lua_pushvalue(lua, -2);

	lua_pcall(lua, inherit ? 1 : 0, 1, 0);
}


void luaS_newinstance(lua_State* lua)
{
	lua_newtable(lua);
	lua_pushvalue(lua, -2);
	lua_setmetatable(lua, -2);
	lua_remove(lua, -2);
}


void luaS_newinstance(lua_State* lua, const CString& className)
{
	lua_getglobal(lua, className.get());
	luaS_newinstance(lua);
}


void luaS_createclass(lua_State* lua, const CString& name, bool inherit)
{
	luaS_newclass(lua, inherit);
	lua_setglobal(lua, name.get());
}


void luaS_createclass(lua_State* lua, const CString& name, const CString& baseName)
{
	if (!baseName.isNull()) {
		lua_getglobal(lua, baseName.get());
		luaS_createclass(lua, name, true);
	} else {
		luaS_createclass(lua, name, false);
	}
}


void luaS_createmethod(lua_State* lua, const CString& name, lua_CFunction cfunc)
{
	lua_pushstring(lua, name.get());
	lua_pushcfunction(lua, cfunc);
	lua_settable(lua, -3);
}


void luaS_createmethod(lua_State* lua, const CString& className, const CString& methodName, lua_CFunction cfunc)
{
	lua_getglobal(lua, className.get());
	luaS_createmethod(lua, methodName, cfunc);
}


void luaS_setfield(lua_State* lua, const CString& name)
{
	luaS_setfield(lua, name, -2);
}


void luaS_setfield(lua_State* lua, const CString& name, int instIdx)
{
	int absIdx = lua_absindex(lua, instIdx);
	lua_pushstring(lua, name.get());
	lua_pushvalue(lua, -2);
	lua_settable(lua, absIdx);
	lua_pop(lua, 1);
}


void luaS_getfield(lua_State* lua, const CString& name)
{
	lua_pushstring(lua, name.get());
	lua_gettable(lua, -2);
}


void luaS_getfield(lua_State* lua, const CString& name, int instIdx)
{
	int absIdx = lua_absindex(lua, instIdx);
	lua_pushstring(lua, name.get());
	lua_gettable(lua, absIdx);
}


void luaS_createfunc(lua_State* lua, const CString& name, lua_CFunction cfunc)
{
	lua_pushcfunction(lua, cfunc);
	lua_setglobal(lua, name.get());
}


int luaS_callmethod(lua_State* lua, int nargs, int nresults)
{
	// instance (kept)
	// method name (popped)
	// arguments (popped)

	lua_pushvalue(lua, -1-nargs);
	lua_gettable(lua, -3-nargs);
	lua_replace(lua, -2-nargs);
	lua_pushvalue(lua, -2-nargs);
	lua_insert(lua, -1-nargs);
	return lua_pcall(lua, nargs+1, nresults, 0);


}


int luaS_fastcallmethod(lua_State* lua, int nargs, int nresults)
{
	// method name (popped)
	// instance (popped)
	// arguments (popped)

	lua_pushvalue(lua, -2-nargs);
	lua_gettable(lua, -2-nargs);
	lua_replace(lua, -3-nargs);
	return lua_pcall(lua, nargs+1, nresults, 0);
}


bool luaS_instanceof(lua_State* lua)
{
	// instance (kept)
	// class (popped)

	if (!lua_istable(lua, -2)  ||  !lua_istable(lua, -1)) {
		lua_pop(lua, 1);
		return false;
	}

	lua_pushstring(lua, "instanceof");
	lua_insert(lua, -2);
	luaS_callmethod(lua, 1, 1);
	bool res = lua_toboolean(lua, -1);
	lua_pop(lua, 1);
	return res;
}


bool luaS_instanceof(lua_State* lua, const CString& clsName)
{
	lua_getglobal(lua, clsName.get());
	return luaS_instanceof(lua);
}


bool luaS_instanceof(lua_State* lua, const CString& clsName, int instIdx)
{
	lua_pushvalue(lua, instIdx);
	bool res = luaS_instanceof(lua, clsName);
	lua_pop(lua, 1);
	return res;
}


lua_Integer luaS_checkintrange(lua_State* lua, int arg, lua_Integer min, lua_Integer max)
{
	lua_Integer val = luaL_checkinteger(lua, arg);

	if (val < min  ||  val > max) {
		char buf[128];
		sprintf(buf, "Argument out of range! Should be [%ld, %ld].", (long) min, (long) max);
		return luaL_argerror(lua, arg, buf);
	}

	return val;
}


lua_Unsigned luaS_checkuintrange(lua_State* lua, int arg, lua_Unsigned min, lua_Unsigned max)
{
	lua_Unsigned val = luaL_checkunsigned(lua, arg);

	if (val < min  ||  val > max) {
		char buf[128];
		sprintf(buf, "Argument out of range! Should be [%u, %u].", min, max);
		return luaL_argerror(lua, arg, buf);
	}

	return val;
}


bool luaS_checkinstanceof(lua_State* lua, const CString& clsName, int instArgIdx)
{
	if (!luaS_instanceof(lua, clsName, instArgIdx)) {
		char* msg = new char[64 + clsName.length()];
		sprintf(msg, "Argument must be a %s!", clsName.get());
		CString cmsg = CString::from(msg);
		luaL_argerror(lua, instArgIdx, cmsg.get());
		return false;
	} else {
		return true;
	}
}


void luaS_loadcdef(lua_State* lua, const CString& cdef)
{
	lua_getglobal(lua, "require");
	lua_pushstring(lua, "ffi");

	if (lua_pcall(lua, 1, 1, 0) != 0) {
		const char* errmsg = lua_tostring(lua, -1);
		lua_pop(lua, 1);
		throw InvalidStateException(CString::format("Error calling require() for loading Lua Cdef: %s", errmsg),
				__FILE__, __LINE__);
	}

	lua_getfield(lua, -1, "cdef");
	lua_pushstring(lua, cdef.get());

	if (lua_pcall(lua, 1, 0, 0) != 0) {
		const char* errmsg = lua_tostring(lua, -1);
		lua_pop(lua, 1);
		throw InvalidStateException(CString::format("Error calling ffi.cdef() for loading Lua Cdef: %s", errmsg),
				__FILE__, __LINE__);
	}

	lua_pop(lua, 1); // result of require("ffi")
}


void luaS_loadffimodule (
		lua_State* lua, const CString& moduleName,
		const CString& ffiCode, const CString& luaCode,
		void (*loadFunction)(lua_State* lua)
) {
	luaS_loadcdef(lua, ffiCode);

	// The module chunk itself is the loader function
	if (luaL_loadstring(lua, luaCode.get()) != 0) {
		const char* errmsg = lua_tostring(lua, -1);
		throw InvalidStateException(CString::format("Error loading Lua module %s: %s", moduleName.get(), errmsg),
				__FILE__, __LINE__);
	}

	lua_getglobal(lua, "package");
	lua_getfield(lua, -1, "preload");
	lua_pushvalue(lua, -3);
	lua_setfield(lua, -2, moduleName.get());
	lua_pop(lua, 3); // package.preload, package, loader function (module chunk)

	if (loadFunction) {
		loadFunction(lua);
	}
}





#if LUA_VERSION_NUM < 502

lua_Unsigned luaL_checkunsigned(lua_State* lua, int arg)
{
	lua_Integer val = luaL_checkinteger(lua, arg);

	if (val < 0) {
		char* msg = new char[64];
		sprintf(msg, "Argument must be unsigned!");
		CString cmsg = CString::from(msg);
		luaL_argerror(lua, arg, cmsg.get());
		return false;
	} else {
		return true;
	}
}


void lua_pushunsigned(lua_State* lua, lua_Unsigned n)
{
	lua_pushinteger(lua, n);
}


int lua_absindex(lua_State *lua, int idx)
{
	return (idx > 0  ||  idx <= LUA_REGISTRYINDEX) ? idx : (lua_gettop(lua) + idx + 1);
}

#endif
