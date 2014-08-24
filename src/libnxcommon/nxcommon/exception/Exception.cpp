/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of nxcommon.

	nxcommon is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	nxcommon is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with nxcommon.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "Exception.h"
#include <nxcommon/config.h>
#include <cstring>
#include <cstdio>





Exception::Exception(const CString& message, const CString& srcFile, int srcLine, const Exception* nestedException,
		const CString& exceptionName)
		: exceptionName(exceptionName), srcFile(srcFile), srcLine(srcLine),
		  nestedException(nestedException ? nestedException->clone() : NULL)
{
	setMessage(message);

#ifdef _BACKTRACE_AVAILABLE
	void* buf[50];
	int backTraceSize = backtrace(buf, 50);
	char** btArr = backtrace_symbols(buf, backTraceSize);

	for (int i = 0 ; i < backTraceSize ; i++) {
		backTrace.append(btArr[i]).append("\n");
	}

	free(btArr);
#endif
}


Exception::Exception(const Exception& ex)
		: exceptionName(ex.exceptionName), srcFile(ex.srcFile), srcLine(ex.srcLine),
		  nestedException(ex.nestedException ? ex.nestedException->clone() : NULL)

#ifdef _BACKTRACE_AVAILABLE
		, backTrace(ex.backTrace)
#endif
{
	setMessage(ex.message);
}


Exception::~Exception() throw()
{
	if (nestedException)
		delete nestedException;
}


CString Exception::getBacktrace() const throw()
{
#ifdef _BACKTRACE_AVAILABLE
	return backTrace;
#else
	return CString();
#endif
}


void Exception::setMessage(const CString& message)
{
	this->message = message;

	if (!message.isNull()) {
		fullMessage = buildFullMessage();
	}
}


CString Exception::buildFullMessage() const throw()
{
	CString msg;

	msg.append(exceptionName).append(": ");

#ifdef NXCOMMON_EXCEPTION_POSITION_INFO
	if (!srcFile.isNull()) {
		msg.append(srcFile);

		if (srcLine != -1) {
			msg.append(":").append(srcLine);
		}

		msg.append(" - ");
	}
#endif

	if (!message.isNull()) {
		msg.append(message);
	} else {
		msg.append("No message");
	}

	if (nestedException) {
		CString nestedMsg = nestedException->what();

		msg.append("\n\tCaused by: ").append(nestedMsg);
	}

	return msg;
}

