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

#ifndef FILECHILDLIST_H_
#define FILECHILDLIST_H_

#include <nxcommon/config.h>
#include "_File.h"
#include "ArchiveChildIterator.h"
#include <memory>

using std::shared_ptr;







class FileChildList
{
public:
	class Iterator
	{
	public:
		Iterator(const Iterator& other);
		Iterator();

		Iterator& operator=(const Iterator& other);

		Iterator& operator++();
		Iterator operator++(int);

		bool operator==(const Iterator& other) const;
		bool operator!=(const Iterator& other) const { return !(*this == other); }

		File operator*() const { return impl->currentFile; }
		const File* operator->() const { return &impl->currentFile; }

	private:
		Iterator(const File& file);

	private:
#ifdef _POSIX_VERSION
		void posixNext();
#endif

	private:
		class Impl
		{
		public:
#ifdef _POSIX_VERSION
			Impl(File iteratedDir, File currentFile, int curIdx, ArchiveChildIterator* archiveIt, DIR* dir)
					: iteratedDir(iteratedDir), currentFile(currentFile), curIdx(curIdx), archiveIt(archiveIt), dir(dir)
			{}
#else
			Impl(File iteratedDir, File currentFile, int curIdx, ArchiveChildIterator* archiveIt, HANDLE* dirHandle)
					: iteratedDir(iteratedDir), currentFile(currentFile), curIdx(curIdx), archiveIt(archiveIt), dirHandle(dirHandle)
			{}
#endif
			~Impl();

			File iteratedDir;
			File currentFile;
			int curIdx;

			ArchiveChildIterator* archiveIt;

#ifdef _POSIX_VERSION
			DIR* dir;
#else
			HANDLE dirHandle;
#endif
		};

	private:
		shared_ptr<Impl> impl;

		friend class FileChildList;
	};

	typedef Iterator iterator;

public:
	FileChildList(const FileChildList& other);
	Iterator begin() const { return Iterator(iteratedDir); }
	Iterator end() const { return Iterator(); }

private:
	FileChildList(const File& iteratedDir);

private:
	File iteratedDir;

	friend class File;
};

#endif /* FILEITERATOR_H_ */
