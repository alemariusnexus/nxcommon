#include "log.h"
#include "util.h"
#include <string.h>
#include <time.h>
#include <pthread.h>

#ifdef __ZEPHYR__
#include <zephyr.h>
#endif



FILE* _mainLogfile = NULL;
int logLevel = LOG_LEVEL_INFO;

#ifdef __ZEPHYR__
// Zephyr does not have PTHREAD_MUTEX_INITIALIZER, but supplies the non-standard PTHREAD_MUTEX_DEFINE()
PTHREAD_MUTEX_DEFINE(_logMutex);
#else
pthread_mutex_t _logMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

const char* logFormat = "%Y-%m-%d %H:%M:%S";
bool freeLogFormat = false;


#ifndef __ZEPHYR__
void OpenLogFile(FILE* file)
{
	if (_mainLogfile) {
		fclose(_mainLogfile);
	}

	_mainLogfile = file;

	//pthread_mutex_init(&_logMutex, nullptr);
}
#endif


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


void SetLogTimeFormat(const char* format)
{
	if (freeLogFormat) {
		free((char*) logFormat);
	}
	logFormat = malloc(strlen(format)+1);
	strcpy((char*) logFormat, format);
	freeLogFormat = true;
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

#ifndef __ZEPHYR__
	time_t t = time(NULL);
#endif

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

#ifdef __ZEPHYR__
	sprintf(timeStr, "(%llu)", (long long unsigned) k_uptime_get());
#else
	struct tm localTime;
	localtime_r(&t, &localTime);
	strftime(timeStr, sizeof(timeStr), logFormat, &localTime);
#endif

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
