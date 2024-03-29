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

#ifndef DEFAULTFILEFINDER_H_
#define DEFAULTFILEFINDER_H_

#include <nxcommon/config.h>
#include "FileFinder.h"



class DefaultFileFinder : public FileFinder {
public:
	DefaultFileFinder(const char* pattern, bool caseSensitive = true, bool exactMatch = true);
	virtual ~DefaultFileFinder();
	void setExactMatch(bool em) { exactMatch = em; }
	bool isCaseSensitive() const { return caseSensitive; }
	bool isExactMatch() const { return exactMatch; }
	virtual bool matches(const File& file);

private:
	char* pattern;
	bool caseSensitive;
	bool exactMatch;
};

#endif /* DEFAULTFILEFINDER_H_ */
