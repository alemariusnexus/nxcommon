#ifndef NXCOMMON_LOG_H_
#define NXCOMMON_LOG_H_

#include "script/luasysdefs.h"

#ifndef GENERATE_LUAJIT_FFI_CDEF

#include <nxcommon/config.h>
#include "file/File.h"
#include <cstdarg>
#include <ctime>

#endif


#ifndef GENERATE_LUAJIT_FFI_CDEF
extern "C"
{
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


#ifndef GENERATE_LUAJIT_FFI_CDEF
void OpenLogFile(const File& logfile);
#endif

LUASYS_EXPORT void SetLogLevel(int level);

LUASYS_EXPORT int GetLogLevel();

LUASYS_EXPORT const char* GetLogLevelName(int level);


#define LogError(...) LogMessage(LOG_LEVEL_ERROR, __VA_ARGS__)
#define LogWarning(...) LogMessage(LOG_LEVEL_WARNING, __VA_ARGS__)
#define LogInfo(...) LogMessage(LOG_LEVEL_INFO, __VA_ARGS__)
#define LogDebug(...) LogMessage(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LogVerbose(...) LogMessage(LOG_LEVEL_VERBOSE, __VA_ARGS__)



// Most of them are inline, which I'm not sure is valid to use with FFI (if the function is not called
// from C++, the compiler might not emit code for it at all, right?).
// We provide separate non-inline versions for Lua below.
#ifndef GENERATE_LUAJIT_FFI_CDEF

inline bool IsLogLevelActive(int level)
{
	return logLevel >= level;
}

void _LogMessagevl(int level, const char* fmt, va_list args);

inline void _LogMessagev(int level, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_LogMessagevl(level, fmt, args);
	va_end(args);
}



// Let's try to do the checking in an inline function, so if the log level is disabled, we might not need
// a function call.

inline void LogMessagevl(int level, const char* fmt, va_list args)
{
	if (IsLogLevelActive(level)) {
		_LogMessagevl(level, fmt, args);
	}
}

inline void LogMessagev(int level, const char* fmt, ...)
{
	if (IsLogLevelActive(level)) {
		va_list args;
		va_start(args, fmt);
		_LogMessagevl(level, fmt, args);
		va_end(args);
	}
}

#endif




LUASYS_EXPORT bool _LuaIsLogLevelActive(int level);

LUASYS_EXPORT void _LuaLogMessagev(int level, const char* fmt, ...);


#ifndef GENERATE_LUAJIT_FFI_CDEF
}
#endif





#ifndef GENERATE_LUAJIT_FFI_CDEF

template <typename... Args>
void _LogMessage(int level, const char* fmt, Args... args)
{
	_LogMessagev(level, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void LogMessage(int level, const char* fmt, Args... args)
{
	if (IsLogLevelActive(level)) {
		_LogMessage(level, fmt, args...);
	}
}

#endif


/*extern "C"
{

void LuaLogMessage(const char* msg);

}*/

#endif /* NXCOMMON_LOG_H_ */
