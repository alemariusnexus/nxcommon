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

#ifndef PRINTHELPERS_H_
#define PRINTHELPERS_H_

#include "global.h"
#include <ostream>
#include <nxcommon/CString.h>
#include <nxcommon/file/FilePath.h>
#include <nxcommon/file/File.h>

#ifdef NXCOMMON_UNICODE_ENABLED
#include <nxcommon/UString.h>
#endif

using std::ostream;


ostream& operator<<(ostream& stream, const FilePath& fp);
ostream& operator<<(ostream& stream, const File& f);

#endif /* PRINTHELPERS_H_ */
