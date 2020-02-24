#ifndef NXCOMMON_LOGCPP_H_
#define NXCOMMON_LOGCPP_H_

#include "log.h"


#ifndef GENERATE_LUAJIT_FFI_CDEF

#include "file/File.h"

#endif


#ifndef GENERATE_LUAJIT_FFI_CDEF
void OpenLogFile(const File& logfile);
#endif



#ifndef GENERATE_LUAJIT_FFI_CDEF

// TODO: Provide LogMessage() for C-only code in log.h as well

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

#endif /* NXCOMMON_LOGCPP_H_ */
