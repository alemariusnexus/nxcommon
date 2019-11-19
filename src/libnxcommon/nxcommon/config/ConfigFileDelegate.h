#ifndef NXCOMMON_SRC_LIBNXCOMMON_NXCOMMON_CONFIG_CONFIGFILEDELEGATE_H_
#define NXCOMMON_SRC_LIBNXCOMMON_NXCOMMON_CONFIG_CONFIGFILEDELEGATE_H_

#include "ConfigFile.h"


class ConfigFileDelegate
{
public:
	bool hasOption(const CString& path) const { return getConfigFileDelegate()->hasOption(path); }

	CString getStringOption(const CString& path, const CString& defaultVal = CString()) const { return getConfigFileDelegate()->getStringOption(path, defaultVal); }
	int getIntOption(const CString& path, int defaultVal = 0) const { return getConfigFileDelegate()->getIntOption(path, defaultVal); }
	unsigned int getUIntOption(const CString& path, unsigned int defaultVal = 0) const { return getConfigFileDelegate()->getUIntOption(path, defaultVal); }
	int64_t getInt64Option(const CString& path, int64_t defaultVal = 0) const { return getConfigFileDelegate()->getInt64Option(path, defaultVal); }
	uint64_t getUInt64Option(const CString& path, uint64_t defaultVal = 0) const { return getConfigFileDelegate()->getUInt64Option(path, defaultVal); }
	float getFloatOption(const CString& path, float defaultVal = 0.0f) const { return getConfigFileDelegate()->getFloatOption(path, defaultVal); }
	double getDoubleOption(const CString& path, double defaultVal = 0.0) const { return getConfigFileDelegate()->getDoubleOption(path, defaultVal); }
	bool getBoolOption(const CString& path, bool defaultVal = false) const { return getConfigFileDelegate()->getBoolOption(path, defaultVal); }
	const Value* getObjectOption(const CString& path, const Value* defaultVal = nullptr) const { return getConfigFileDelegate()->getObjectOption(path, defaultVal); }
	const Value* getArrayOption(const CString& path, const Value* defaultVal = nullptr) const { return getConfigFileDelegate()->getArrayOption(path, defaultVal); }

	CString requireStringOption(const CString& path) const { return getConfigFileDelegate()->requireStringOption(path); }
	int requireIntOption(const CString& path) const { return getConfigFileDelegate()->requireIntOption(path); }
	unsigned int requireUIntOption(const CString& path) const { return getConfigFileDelegate()->requireUIntOption(path); }
	int64_t requireInt64Option(const CString& path) const { return getConfigFileDelegate()->requireInt64Option(path); }
	uint64_t requireUInt64Option(const CString& path) const { return getConfigFileDelegate()->requireUInt64Option(path); }
	float requireFloatOption(const CString& path) const { return getConfigFileDelegate()->requireFloatOption(path); }
	double requireDoubleOption(const CString& path) const { return getConfigFileDelegate()->requireDoubleOption(path); }
	bool requireBoolOption(const CString& path) const { return getConfigFileDelegate()->requireBoolOption(path); }
	const Value& requireObjectOption(const CString& path) const { return getConfigFileDelegate()->requireObjectOption(path); }
	const Value& requireArrayOption(const CString& path) const { return getConfigFileDelegate()->requireArrayOption(path); }

protected:
	virtual const ConfigFile* getConfigFileDelegate() const = 0;
	virtual ConfigFile* getConfigFileDelegate() { const ConfigFile* d = getConfigFileDelegate(); return const_cast<ConfigFile*>(d); }
};

#endif /* NXCOMMON_SRC_LIBNXCOMMON_NXCOMMON_CONFIG_CONFIGFILEDELEGATE_H_ */
