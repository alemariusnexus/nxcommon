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

#define GL_CPP

#include "gl.h"
#include <set>
#include <cstring>
#include <cstdio>
#include "GLException.h"
#include "../StringComparator.h"
#include <memory>

using std::set;
using std::shared_ptr;




#ifdef GTA_USE_OPENGL_ES

set<const char*, StringComparator> supportedExtensions;

#endif

int majorVersion, minorVersion;
bool nxglInitialized = false;





void nxglInit()
{
	if (nxglInitialized)
		return;

	const char* verStr = (const char*) glGetString(GL_VERSION);

	GLException::checkError();

#ifdef GTA_USE_OPENGL_ES
	const char* extStr = (const char*) glGetString(GL_EXTENSIONS);
	GLException::checkError();
	char* extStrCpy = new char[strlen(extStr)+1];
	strcpy(extStrCpy, extStr);

	char* ext = strtok(extStrCpy, " ");
	while (ext) {
		char* extCpy = new char[strlen(ext)+1];
		strcpy(extCpy, ext);
		supportedExtensions.insert(ext);
		ext = strtok(NULL, " ");
	}
	
	sscanf(verStr, "OpenGL ES %d.%d", &majorVersion, &minorVersion);
#else
	glewInit();
	sscanf(verStr, "%d.%d", &majorVersion, &minorVersion);
#endif

	nxglInitialized = true;
}


bool nxglIsExtensionSupported(const char* extension)
{
#ifdef GTA_USE_OPENGL_ES
	return supportedExtensions.find(extension) != supportedExtensions.end();
#else
	return glewIsSupported(extension);
#endif
}


bool nxglIsVersionSupported(int major, int minor)
{
	return majorVersion > major  ||  (majorVersion == major  &&  minorVersion >= minor);
}


void nxglGetVersion(int& major, int& minor)
{
	major = majorVersion;
	minor = minorVersion;
}


const char* nxglGetSupportedExtensions()
{
	return (const char*) glGetString(GL_EXTENSIONS);
}



#undef GL_CPP
