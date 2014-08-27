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

#ifndef NXCOMMON_EXCEPTION_H_
#define NXCOMMON_EXCEPTION_H_

#include <nxcommon/config.h>
#include "../CString.h"
#include <exception>
#include <cstdlib>


#if (defined(__GLIBC__)  &&  ((__GLIBC__ << 16) + __GLIBC_MINOR__ >= (2 << 16) + 1))
#define _BACKTRACE_AVAILABLE
#endif


#ifdef _BACKTRACE_AVAILABLE
#include <execinfo.h>
#endif


using std::exception;


class Exception : public exception {
public:
	Exception(const CString& message, const CString& srcFile = CString(), int srcLine = -1,
			const Exception* nestedException = NULL, const CString& exceptionName = CString("Exception"));
	Exception(const Exception& ex);
	virtual ~Exception() throw();
	virtual const char* what() const throw() { return fullMessage.get(); }
	virtual Exception* clone() const throw() { return new Exception(*this); }
	CString getMessage() const throw() { return message; }
	CString getBacktrace() const throw();
	CString getName() const throw() { return exceptionName; }

protected:
	void setMessage(const CString& message);

private:
	CString buildFullMessage() const throw();

private:
	CString exceptionName;
	CString message;
	CString fullMessage;
	CString srcFile;
	int srcLine;
	Exception* nestedException;

#ifdef _BACKTRACE_AVAILABLE
	CString backTrace;
#endif
};

#endif /* NXCOMMON_EXCEPTION_H_ */


