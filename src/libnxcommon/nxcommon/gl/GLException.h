/*
	Copyright 2010-2015 David "Alemarius Nexus" Lerch

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

#ifndef GLEXCEPTION_H_
#define GLEXCEPTION_H_

#include <nxcommon/config.h>
#include <GL/glew.h>
#include "../exception/Exception.h"



class GLException : public Exception {
public:
	static void checkError(const CString& = CString());
	static void checkFramebufferStatus(GLenum target, const CString& msg = CString());

public:
	GLException(const CString& message, const CString& srcFile = CString(), int srcLine = -1,
			Exception* nestedException = NULL)
			: Exception(message, srcFile, srcLine, nestedException, "GLException") {}
};

#endif /* GLEXCEPTION_H_ */
