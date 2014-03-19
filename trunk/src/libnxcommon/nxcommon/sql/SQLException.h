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

#ifndef SQLEXCEPTION_H_
#define SQLEXCEPTION_H_

#include "../exception/Exception.h"


class SQLException : public Exception
{
public:
	SQLException(const char* message, const char* srcFile = NULL, int srcLine = -1,
			Exception* nestedException = NULL)
			: Exception(message, srcFile, srcLine, nestedException, "SQLException") {}
};

#endif /* SQLEXCEPTION_H_ */
