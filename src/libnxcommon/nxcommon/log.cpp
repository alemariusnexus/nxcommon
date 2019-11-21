#include "log.h"
#include "exception.h"
#include "util.h"
#include "CString.h"
#include "stream/IOException.h"
#include <pthread.h>



FILE* _mainLogfile = nullptr;
int logLevel = LOG_LEVEL_INFO;
pthread_mutex_t _logMutex;


void OpenLogFile(const File& logfile)
{
	char errbuf[256];

	if (_mainLogfile) {
		fclose(_mainLogfile);
	}

	_mainLogfile = fopen(logfile.toString().get(), "a");
	if (!_mainLogfile) {
		throw IOException("Error opening log file", __FILE__, __LINE__);
	}

	pthread_mutex_init(&_logMutex, nullptr);
}


void SetLogLevel(int level)
{
	logLevel = level;
}


int GetLogLevel()
{
	return logLevel;
}


const char* GetLogLevelName(int level)
{
	switch (level) {
	case LOG_LEVEL_NONE:
		return "none";

	case LOG_LEVEL_ERROR:
		return "error";

	case LOG_LEVEL_WARNING:
		return "warning";

	case LOG_LEVEL_INFO:
		return "info";

	case LOG_LEVEL_DEBUG:
		return "debug";

	case LOG_LEVEL_VERBOSE:
		return "verbose";
	}

	return "[INVALID]";
}


/*void LogMessagev(int level, const char* fmt, ...)
{
	// Don't check for log level. This is done by LogMessage()

	time_t t = time(nullptr);

	const char* typeCode = "[???]";

	switch (level) {
	case LOG_LEVEL_ERROR:
		typeCode = "[ERR]";
		break;
	case LOG_LEVEL_WARNING:
		typeCode = "[WRN]";
		break;
	case LOG_LEVEL_INFO:
		typeCode = "[INF]";
		break;
	case LOG_LEVEL_DEBUG:
		typeCode = "[DBG]";
		break;
	case LOG_LEVEL_VERBOSE:
		typeCode = "[VRB]";
		break;
	}

	char timeStr[64];
	ctime_r(&t, timeStr);

	// Strip the trailing newline. Not entirely sure if this is legal...
	timeStr[strlen(timeStr)-1] = '\0';

	FILE* outStreams[] = { _mainLogfile, stdout };

	for (size_t i = 0 ; i < sizeof(outStreams) / sizeof(FILE*) ; i++) {
		FILE* out = outStreams[i];

		if (out) {
			pthread_mutex_lock(&_logMutex);

			fprintf(out, "%s %s - ", typeCode, timeStr);

			va_list args;
			va_start(args, fmt);
			vfprintf(out, fmt, args);
			va_end(args);

			fprintf(out, "\n");

			pthread_mutex_unlock(&_logMutex);

			fflush(out);
		}
	}
}*/


void _LogMessagevl(int level, const char* fmt, va_list args)
{
	// Don't check for log level. This is done by LogMessage()

	time_t t = time(nullptr);

	const char* typeCode = "[???]";

	switch (level) {
	case LOG_LEVEL_ERROR:
		typeCode = "[ERR]";
		break;
	case LOG_LEVEL_WARNING:
		typeCode = "[WRN]";
		break;
	case LOG_LEVEL_INFO:
		typeCode = "[INF]";
		break;
	case LOG_LEVEL_DEBUG:
		typeCode = "[DBG]";
		break;
	case LOG_LEVEL_VERBOSE:
		typeCode = "[VRB]";
		break;
	}

	char timeStr[64];
	ctime_r(&t, timeStr);

	// Strip the trailing newline. Not entirely sure if this is legal...
	timeStr[strlen(timeStr)-1] = '\0';

	FILE* outStreams[] = { _mainLogfile, stdout };

	for (size_t i = 0 ; i < sizeof(outStreams) / sizeof(FILE*) ; i++) {
		FILE* out = outStreams[i];

		if (out) {
			pthread_mutex_lock(&_logMutex);

			fprintf(out, "%s %s - ", typeCode, timeStr);

			va_list argsCpy;
			va_copy(argsCpy, args);
			vfprintf(out, fmt, argsCpy);
			va_end(argsCpy);

			fprintf(out, "\n");

			pthread_mutex_unlock(&_logMutex);

			fflush(out);
		}
	}
}


bool _LuaIsLogLevelActive(int level)
{
	return IsLogLevelActive(level);
}


void _LuaLogMessagev(int level, const char* fmt, ...)
{
	if (IsLogLevelActive(level)) {
		va_list args;
		va_start(args, fmt);
		_LogMessagevl(level, fmt, args);
		va_end(args);
	}
}


/*void LuaLogMessage(const char* msg)
{
	LogInfo("%s", msg);
}*/
