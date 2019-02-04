#ifndef NXCOMMON_SRC_LIBNXCOMMON_NXCOMMON_DEBUG_H_
#define NXCOMMON_SRC_LIBNXCOMMON_NXCOMMON_DEBUG_H_

#include <nxcommon/config.h>
#include "CString.h"
#include "file/File.h"


#if (defined(__GLIBC__)  &&  ((__GLIBC__ << 16) + __GLIBC_MINOR__ >= (2 << 16) + 1))
#define _BACKTRACE_AVAILABLE
#endif


#ifdef _BACKTRACE_AVAILABLE
#include <execinfo.h>
#endif



#define MAX_TRACE_FRAMES 64





struct StackTrace
{
#ifdef _BACKTRACE_AVAILABLE
	void* trace[MAX_TRACE_FRAMES];
	int numFrames;
	size_t startFrame;
#endif
};




enum ResolveStackTraceAddressFlag
{
	RESOLVE_STACK_TRACE_ADDRESS_FLAG_SHORT_FILENAMES = (1 << 0),
	RESOLVE_STACK_TRACE_ADDRESS_FLAG_FUNCTION_NAMES = (1 << 1),
	RESOLVE_STACK_TRACE_ADDRESS_FLAG_FUNCTION_NAMES_DEMANGLED = (1 << 2),
	RESOLVE_STACK_TRACE_ADDRESS_FLAG_KEEP_RAW = (1 << 3),

	RESOLVE_STACK_TRACE_ADDRESS_FLAG_FULL = (
			RESOLVE_STACK_TRACE_ADDRESS_FLAG_FUNCTION_NAMES
			| RESOLVE_STACK_TRACE_ADDRESS_FLAG_FUNCTION_NAMES_DEMANGLED
			),
	RESOLVE_STACK_TRACE_ADDRESS_FLAG_SHORT_FULL = (
			RESOLVE_STACK_TRACE_ADDRESS_FLAG_SHORT_FILENAMES
			| RESOLVE_STACK_TRACE_ADDRESS_FLAG_FUNCTION_NAMES
			| RESOLVE_STACK_TRACE_ADDRESS_FLAG_FUNCTION_NAMES_DEMANGLED
			),
	RESOLVE_STACK_TRACE_ADDRESS_FLAG_SHORT = (
			RESOLVE_STACK_TRACE_ADDRESS_FLAG_SHORT_FILENAMES
			),
};




bool GetStackTrace(StackTrace& st, size_t numLevels, size_t startFrame = 0);
CString FormatStackTrace(const StackTrace& st);

CString GetFormattedStackTrace(size_t numLevels, size_t startFrame = 0);


CString ResolveFormattedStackTraceAddresses(const CString& trace, const File& executableFile, int flags = 0);

#endif /* NXCOMMON_SRC_LIBNXCOMMON_NXCOMMON_DEBUG_H_ */
