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

#ifndef ARCHIVEHANDLER_H_
#define ARCHIVEHANDLER_H_

#include <nxcommon/config.h>
#include "File.h"
#include "ArchiveChildIterator.h"



class ArchiveHandler
{
public:
	virtual bool exists(const File& file) const = 0;
	virtual FileType getType(const File& file) const = 0;
	virtual bool isArchiveDirectory(const File& file) const = 0;
	virtual bool isArchiveEntry(const File& file) const = 0;
	virtual ArchiveChildIterator* getChildIterator(const File& file) const = 0;
	virtual File getCanonicalFile(const File& file, const File& cdir = File()) const = 0;
	virtual istream* openInputStream(const File& file, istream::openmode mode) const = 0;
	virtual ostream* openOutputStream(const File& file, ostream::openmode mode) const = 0;
	virtual iostream* openInputOutputStream(const File& file, iostream::openmode mode) const = 0;
	virtual File::filesize getSize(const File& file, bool& supported) const = 0;
};

#endif /* ARCHIVEHANDLER_H_ */
