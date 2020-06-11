#ifndef NXCOMMON_LOG_H_
#define NXCOMMON_LOG_H_

#include "script/luasysdefs.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef GENERATE_LUAJIT_FFI_CDEF

#include <nxcommon/config.h>
#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#endif


enum
{
	LOG_LEVEL_NONE = 0,
	LOG_LEVEL_ERROR = 10,
	LOG_LEVEL_WARNING = 20,
	LOG_LEVEL_INFO = 30,
	LOG_LEVEL_DEBUG = 40,
	LOG_LEVEL_VERBOSE = 50
};

extern int logLevel;

#ifndef __ZEPHYR__
void OpenLogFile(FILE* file);
#endif

LUASYS_EXPORT void SetLogLevel(int level);

LUASYS_EXPORT int GetLogLevel();

LUASYS_EXPORT const char* GetLogLevelName(int level);

LUASYS_EXPORT void SetLogTimeFormat(const char* format);


#define LogError(...) LogMessage(LOG_LEVEL_ERROR, __VA_ARGS__)
#define LogWarning(...) LogMessage(LOG_LEVEL_WARNING, __VA_ARGS__)
#define LogInfo(...) LogMessage(LOG_LEVEL_INFO, __VA_ARGS__)
#define LogDebug(...) LogMessage(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LogVerbose(...) LogMessage(LOG_LEVEL_VERBOSE, __VA_ARGS__)



// Most of them are inline, which I'm not sure is valid to use with FFI (if the function is not called
// from C++, the compiler might not emit code for it at all, right?).
// We provide separate non-inline versions for Lua below.
#ifndef GENERATE_LUAJIT_FFI_CDEF

static inline bool IsLogLevelActive(int level)
{
	return logLevel >= level;
}

void _LogMessagevl(int level, const char* fmt, va_list args);

static inline void _LogMessagev(int level, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_LogMessagevl(level, fmt, args);
	va_end(args);
}



// Let's try to do the checking in an inline function, so if the log level is disabled, we might not need
// a function call.

static inline void LogMessagevl(int level, const char* fmt, va_list args)
{
	if (IsLogLevelActive(level)) {
		_LogMessagevl(level, fmt, args);
	}
}

static inline void LogMessagev(int level, const char* fmt, ...)
{
	if (IsLogLevelActive(level)) {
		va_list args;
		va_start(args, fmt);
		_LogMessagevl(level, fmt, args);
		va_end(args);
	}
}


#ifdef NXCOMMON_C_ONLY

#define _LogMessage(level,fmt,...) _LogMessagev(level,fmt,__VA_ARGS__)
#define LogMessage(level,fmt,...) _LogMessage(level,fmt,__VA_ARGS__)

#endif

#endif


LUASYS_EXPORT bool _LuaIsLogLevelActive(int level);

LUASYS_EXPORT void _LuaLogMessagev(int level, const char* fmt, ...);


#ifdef __cplusplus
} // END extern "C"
#endif


#ifdef __cplusplus
#include "logcpp.h"
#endif


#endif /* NXCOMMON_LOG_H_ */
