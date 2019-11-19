#ifndef CONFIGFILE_H_
#define CONFIGFILE_H_

#include <nxcommon/config.h>
#include "../file/File.h"
#include <rapidjson/document.h>
#include <vector>
#include <mutex>


#ifdef NXCOMMON_LUA_ENABLED

extern "C" {

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

}

#endif

using namespace rapidjson;



class ConfigFile
{
public:
	//ConfigFile(const File& file, const CString& name = CString());
	ConfigFile(const CString& name = CString());

	/*void addOptionOverride(const ConfigOverride& ovr) { addOptionOverrides(&ovr, (&ovr) + 1); }

	template <typename Iterator>
	void addOptionOverrides(Iterator beg, Iterator end);*/

	//void load();
	void load(const File& file);
	bool isLoaded() const;

	void cascade(Document& ldoc);
	void cascade(const File& file);

	Document& getDocument() { return doc; }

	bool hasOption(const CString& path) const;

	CString getStringOption(const CString& path, const CString& defaultVal = CString()) const;
	int getIntOption(const CString& path, int defaultVal = 0) const;
	unsigned int getUIntOption(const CString& path, unsigned int defaultVal = 0) const;
	int64_t getInt64Option(const CString& path, int64_t defaultVal = 0) const;
	uint64_t getUInt64Option(const CString& path, uint64_t defaultVal = 0) const;
	float getFloatOption(const CString& path, float defaultVal = 0.0f) const;
	double getDoubleOption(const CString& path, double defaultVal = 0.0) const;
	bool getBoolOption(const CString& path, bool defaultVal = false) const;
	const Value* getObjectOption(const CString& path, const Value* defaultVal = nullptr) const;
	const Value* getArrayOption(const CString& path, const Value* defaultVal = nullptr) const;

	CString requireStringOption(const CString& path) const;
	int requireIntOption(const CString& path) const;
	unsigned int requireUIntOption(const CString& path) const;
	int64_t requireInt64Option(const CString& path) const;
	uint64_t requireUInt64Option(const CString& path) const;
	float requireFloatOption(const CString& path) const;
	double requireDoubleOption(const CString& path) const;
	bool requireBoolOption(const CString& path) const;
	const Value& requireObjectOption(const CString& path) const;
	const Value& requireArrayOption(const CString& path) const;

#ifdef NXCOMMON_LUA_ENABLED
	void createLuaWrapper(lua_State* lua);
#endif

private:
	void loadDocument(Document& newDoc, const File& file);

	void cascadeDocument(Document& ovrDoc);

private:
	//File file;
	CString name;
	Document doc;

	//std::mutex overridesMtx;
	//std::vector<ConfigOverride> overrides;
};





/*template <typename Iterator>
void ConfigFile::addOptionOverrides(Iterator beg, Iterator end)
{
	std::lock_guard<std::mutex> lock(overridesMtx);

	while (beg != end) {
		overrides.push_back(*beg);
		beg++;
	}
}*/




#endif /* CONFIGFILE_H_ */
