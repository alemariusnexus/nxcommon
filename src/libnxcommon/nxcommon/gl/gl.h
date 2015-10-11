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

#ifndef GL_H_
#define GL_H_

#include <nxcommon/config.h>


#ifdef GTA_USE_OPENGL_ES

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#else

#include <GL/glew.h>

#endif



void nxglInit();
bool nxglIsExtensionSupported(const char* extension);
bool nxglIsVersionSupported(int major, int minor);
void nxglGetVersion(int& major, int& minor);
const char* nxglGetSupportedExtensions();

#endif /* GL_H_ */
