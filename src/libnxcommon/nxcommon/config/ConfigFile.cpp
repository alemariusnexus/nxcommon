#include "ConfigFile.h"
#include "../exception.h"
#include "../util.h"
#include <rapidjson/error/en.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/pointer.h>

namespace rj = rapidjson;




/*ConfigFile::ConfigFile(const File& file, const CString& name)
		: file(file), name(name)
{
	if (name.isNull()) {
		this->name = file.toString();
	}
}*/


ConfigFile::ConfigFile(const CString& name)
		: name(name)
{
}


bool ConfigFile::isLoaded() const
{
	return !doc.IsNull();
}


void ConfigFile::cascade(Document& ldoc)
{
	cascadeDocument(ldoc);
}


void ConfigFile::cascade(const File& file)
{
	Document ovrDoc;
	loadDocument(ovrDoc, file);
	cascade(ovrDoc);
}


void ConfigFile::loadDocument(Document& newDoc, const File& file)
{
	FILE* handle = fopen(file.toString().get(), "r");

	if (!handle) {
		throw IOException(CString::format("Unable to open config file '%s' (path: %s)", name.get(),
				file.toString().get()), __FILE__, __LINE__);
	}

	char readBuf[8192];
	FileReadStream in(handle, readBuf, sizeof(readBuf));

	newDoc.ParseStream<kParseDefaultFlags | kParseCommentsFlag | kParseTrailingCommasFlag>(in);

	fclose(handle);

	if (newDoc.HasParseError()) {
		throw InvalidFormatException(CString::format(
				"Error parsing config file '%s' at offset %llu: %s\n",
				name.get(),
				(long long unsigned) newDoc.GetErrorOffset(),
				GetParseError_En(newDoc.GetParseError())),
				__FILE__, __LINE__);
	}
}


void ConfigFile::load(const File& file)
{
	Document newDoc;
	loadDocument(newDoc, file);

	doc = std::move(newDoc);
}


void ConfigFile::cascadeDocument(Document& ovrDoc)
{
	CascadeJSON(doc, ovrDoc, doc.GetAllocator());
}


bool ConfigFile::hasOption(const CString& path) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	return val != nullptr;
}


CString ConfigFile::getStringOption(const CString& path, const CString& defaultVal) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsString()) {
		return CString(val->GetString(), val->GetStringLength());
	}
	return defaultVal;
}


int ConfigFile::getIntOption(const CString& path, int defaultVal) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsInt()) {
		return val->GetInt();
	}
	return defaultVal;
}


unsigned int ConfigFile::getUIntOption(const CString& path, unsigned int defaultVal) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsUint()) {
		return val->GetUint();
	}
	return defaultVal;
}


int64_t ConfigFile::getInt64Option(const CString& path, int64_t defaultVal) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsInt64()) {
		return val->GetInt64();
	}
	return defaultVal;
}


uint64_t ConfigFile::getUInt64Option(const CString& path, uint64_t defaultVal) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsUint64()) {
		return val->GetUint64();
	}
	return defaultVal;
}


float ConfigFile::getFloatOption(const CString& path, float defaultVal) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsFloat()) {
		return val->GetFloat();
	}
	return defaultVal;
}


double ConfigFile::getDoubleOption(const CString& path, double defaultVal) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsDouble()) {
		return val->GetDouble();
	}
	return defaultVal;
}


bool ConfigFile::getBoolOption(const CString& path, bool defaultVal) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsBool()) {
		return val->GetBool();
	}
	return defaultVal;
}


const Value* ConfigFile::getObjectOption(const CString& path, const Value* defaultVal) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsObject()) {
		return val;
	}
	return defaultVal;
}


const Value* ConfigFile::getArrayOption(const CString& path, const Value* defaultVal) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsArray()) {
		return val;
	}
	return defaultVal;
}


CString ConfigFile::requireStringOption(const CString& path) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsString()) {
		return CString(val->GetString(), val->GetStringLength());
	}
	throw InvalidValueException(CString::format("Required config option %s not set or invalid", path.get()),
			__FILE__, __LINE__);
}


int ConfigFile::requireIntOption(const CString& path) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsInt()) {
		return val->GetInt();
	}
	throw InvalidValueException(CString::format("Required config option %s not set or invalid", path.get()),
			__FILE__, __LINE__);
}


unsigned int ConfigFile::requireUIntOption(const CString& path) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsUint()) {
		return val->GetUint();
	}
	throw InvalidValueException(CString::format("Required config option %s not set or invalid", path.get()),
			__FILE__, __LINE__);
}


int64_t ConfigFile::requireInt64Option(const CString& path) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsInt64()) {
		return val->GetInt64();
	}
	throw InvalidValueException(CString::format("Required config option %s not set or invalid", path.get()),
			__FILE__, __LINE__);
}


uint64_t ConfigFile::requireUInt64Option(const CString& path) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsUint64()) {
		return val->GetUint64();
	}
	throw InvalidValueException(CString::format("Required config option %s not set or invalid", path.get()),
			__FILE__, __LINE__);
}


float ConfigFile::requireFloatOption(const CString& path) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsFloat()) {
		return val->GetFloat();
	}
	throw InvalidValueException(CString::format("Required config option %s not set or invalid", path.get()),
			__FILE__, __LINE__);
}


double ConfigFile::requireDoubleOption(const CString& path) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsDouble()) {
		return val->GetDouble();
	}
	throw InvalidValueException(CString::format("Required config option %s not set or invalid", path.get()),
			__FILE__, __LINE__);
}


bool ConfigFile::requireBoolOption(const CString& path) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsBool()) {
		return val->GetBool();
	}
	throw InvalidValueException(CString::format("Required config option %s not set or invalid", path.get()),
			__FILE__, __LINE__);
}


const Value& ConfigFile::requireObjectOption(const CString& path) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsObject()) {
		return *val;
	}
	throw InvalidValueException(CString::format("Required config option %s not set or invalid", path.get()),
			__FILE__, __LINE__);
}


const Value& ConfigFile::requireArrayOption(const CString& path) const
{
	const Value* val = rj::Pointer(path.get()).Get(doc);
	if (val  &&  val->IsArray()) {
		return *val;
	}
	throw InvalidValueException(CString::format("Required config option %s not set or invalid", path.get()),
			__FILE__, __LINE__);
}



#ifdef NXCOMMON_LUA_ENABLED

void ConfigFile::createLuaWrapper(lua_State* lua)
{
	// local config = require("config")
	lua_getglobal(lua, "require");
	lua_pushstring(lua, "config");
	if (lua_pcall(lua, 1, 1, 0) != 0) {
		const char* errmsg = lua_tostring(lua, -1);
		lua_pop(lua, 1);
		throw InvalidStateException(CString::format(
				"Error calling require() for loading ConfigFile wrapper: %s", errmsg),
				__FILE__, __LINE__);
	}

	// local res = config.WrapConfigFile(this)
	lua_getfield(lua, -1, "WrapConfigFile");
	lua_pushlightuserdata(lua, this);
	if (lua_pcall(lua, 1, 1, 0) != 0) {
		const char* errmsg = lua_tostring(lua, -1);
		lua_pop(lua, 2);
		throw InvalidStateException(CString::format(
				"Error calling config.WrapConfigFile for loading ConfigFile wrapper: %s", errmsg),
				__FILE__, __LINE__);
	}

	lua_remove(lua, -2); // local config
}

#endif
