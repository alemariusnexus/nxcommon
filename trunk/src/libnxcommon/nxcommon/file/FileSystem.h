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

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "ArchiveHandler.h"
#include <vector>

using std::vector;




class FileSystem
{
public:
	typedef vector<ArchiveHandler*> ArchiveHandlerList;
	typedef ArchiveHandlerList::iterator ArchiveHandlerIterator;

private:
	static FileSystem* inst;

public:
	static FileSystem* getInstance();

public:
	ArchiveHandlerList getArchiveHandlers() { return archiveHandlers; }
	void registerArchiveHandler(ArchiveHandler* handler) { archiveHandlers.push_back(handler); }

private:
	FileSystem() {}

private:
	ArchiveHandlerList archiveHandlers;
};

#endif /* FILESYSTEM_H_ */
