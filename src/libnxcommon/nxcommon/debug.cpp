#include "debug.h"
#include "exception/InvalidStateException.h"

#ifdef _POSIX_VERSION
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif




bool GetStackTrace(StackTrace& st, size_t numLevels, size_t startFrame)
{
#ifdef _BACKTRACE_AVAILABLE
	st.numFrames = backtrace(st.trace, numLevels);
	st.startFrame = startFrame + 1;
	return true;
#else
	return false;
#endif
}


CString FormatStackTrace(const StackTrace& st)
{
#ifdef _BACKTRACE_AVAILABLE
	CString backTrace(st.numFrames*32);

	char** btArr = backtrace_symbols(st.trace + st.startFrame, st.numFrames - st.startFrame);

	for (int i = 0 ; i < st.numFrames-st.startFrame ; i++) {
		backTrace.append(btArr[i]).append("\n");
	}

	free(btArr);

	return backTrace;
#else
	return CString();
#endif
}


CString GetFormattedStackTrace(size_t numLevels, size_t startFrame)
{
#ifdef _BACKTRACE_AVAILABLE
	CString backTrace(numLevels*32);

	void* buf[numLevels];
	int backTraceSize = backtrace(buf, numLevels);
	char** btArr = backtrace_symbols(buf + startFrame + 1, backTraceSize - startFrame - 1);

	for (int i = 0 ; i < backTraceSize-startFrame-1 ; i++) {
		backTrace.append(btArr[i]).append("\n");
	}

	free(btArr);

	return backTrace;
#else
	return CString();
#endif
}


CString ResolveFormattedStackTraceAddresses(const CString& trace, const File& executableFile, int flags)
{
#ifdef _POSIX_VERSION
	// TODO: Find a way to do this properly (without forking to external processes and praying)

	CString cmd = CString::format("sed 's/.*\\[0x\\([0-9a-fA-F]*\\)\\]$/\\1/' | addr2line -e '%s' -p", executableFile.toString().get());

	if ((flags & RESOLVE_STACK_TRACE_ADDRESS_FLAG_SHORT_FILENAMES) != 0) {
		cmd.append(" -s");
	}
	if ((flags & RESOLVE_STACK_TRACE_ADDRESS_FLAG_FUNCTION_NAMES) != 0) {
		cmd.append(" -f");
	}
	if ((flags & RESOLVE_STACK_TRACE_ADDRESS_FLAG_FUNCTION_NAMES_DEMANGLED) != 0) {
		cmd.append("  -C");
	}

	int pipeIn[2];
	int pipeOut[2];

	if (pipe(pipeIn)  ||  pipe(pipeOut)) {
		throw InvalidStateException("Error creating pipes for resolving stack trace addresses", __FILE__, __LINE__);
	}

	pid_t pid = fork();

	if (pid == -1) {
		throw InvalidStateException("Error forking for resolving stack trace addresses", __FILE__, __LINE__);
	}

	if (pid != 0) {
		// Parent process

		FILE* childIn = fdopen(pipeOut[0], "r");
		FILE* childOut = fdopen(pipeIn[1], "w");
		close(pipeOut[1]);
		close(pipeIn[0]);

		fprintf(childOut, "%s", trace.get());
		fflush(childOut);

		fclose(childOut);

		CString formattedTrace(trace.size());

		std::vector<CString> frames = trace.split('\n');
		char resolvedBuf[512];

		bool first = true;
		for (CString frame : frames) {

			bool validFormattedFrame = false;

			if (!frame.isEmpty()) {
				if (!first) {
					formattedTrace.append('\n');
				}

				char* cResolvedAddr = fgets(resolvedBuf, sizeof(resolvedBuf), childIn);

				if (cResolvedAddr) {
					size_t len = strlen(cResolvedAddr);

					if (cResolvedAddr[len-1] == '\n') {
						len--;
					}

					formattedTrace.append(CString::readAlias(cResolvedAddr, len));

					if (strstr(cResolvedAddr, "??:") == nullptr) {
						validFormattedFrame = true;
					}

					if (!validFormattedFrame  ||  (flags & RESOLVE_STACK_TRACE_ADDRESS_FLAG_KEEP_RAW) != 0) {
						formattedTrace.append("   |   ");
					}
				}
			}

			if (!validFormattedFrame  ||  (flags & RESOLVE_STACK_TRACE_ADDRESS_FLAG_KEEP_RAW) != 0) {
				formattedTrace.append(frame);
			}

			first = false;
		}

		fclose(childIn);

		int res;
		wait(&res);

		return formattedTrace;
	} else {
		// Child process

		close(1);
		dup(pipeOut[1]);

		close(0);
		dup(pipeIn[0]);

		close(pipeOut[0]);
		close(pipeOut[1]);
		close(pipeIn[0]);
		close(pipeIn[1]);

		execl("/bin/sh", "/bin/sh", "-c", cmd.get(), (char*) NULL);
		exit(1);
	}
#else
	return trace;
#endif
}

