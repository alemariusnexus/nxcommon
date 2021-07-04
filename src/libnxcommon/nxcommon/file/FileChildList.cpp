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

#include "FileChildList.h"
#include "_File.h"
#include "FileException.h"
#include "FileSystem.h"
#include <cerrno>




FileChildList::Iterator::Iterator(const File& file)
		: impl(shared_ptr<Impl>(new Impl(file, File(), -1, NULL, NULL)))
{
	if (!file.exists()) {
		char* errmsg = new char[file.getPath().toString().length() + 128];
		sprintf(errmsg, "Attempt to iterate over non-existant file '%s'!", file.getPath().toString().get());
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}

	if (file.physicallyExists()  &&  file.isDirectory()) {
#ifdef _POSIX_VERSION
		impl->dir = opendir(file.getPath().toString().get());

		if (impl->dir == NULL) {
			char* errStr = strerror(errno);
			char* errMsg = new char[strlen(errStr) + file.getPath().toString().length() + 128];
			sprintf(errMsg, "Internal error opening file %s for iteration: %s",
					file.getPath().toString().get(), errStr);
			FileException ex(errMsg, __FILE__, __LINE__);
			delete[] errMsg;
			throw ex;
		}

		posixNext();
#else
		WIN32_FIND_DATA fdata;
		char* pattern = new char[file.getPath().toString().length() + 3];
		strcpy(pattern, file.getPath().toString().get());
		strcat(pattern, "/*");
		impl->dirHandle = FindFirstFile(pattern, &fdata);

		bool empty = false;

		while(strcmp(fdata.cFileName, ".") == 0  ||  strcmp(fdata.cFileName, "..") == 0) {
			if (FindNextFile(impl->dirHandle, &fdata) == 0) {
				empty = true;
				break;
			}
		}

		if (!empty) {
			impl->currentFile = File(FilePath(file.getPath(), fdata.cFileName));
			impl->curIdx++;
		} else {
			impl->currentFile = File();
		}
#endif
	} else {
		for (ArchiveHandler* handler : FileSystem::getInstance()->getArchiveHandlers()) {
			ArchiveChildIterator* it = handler->getChildIterator(file);

			if (it) {
				impl->archiveIt = it;
				break;
			}
		}

		if (!impl->archiveIt) {
			char* errmsg = new char[256 + file.getPath().toString().length()];
			sprintf(errmsg, "Attempt to iterate over a file that is neither a directory nor supported as directory "
					"by any ArchiveHandler: %s", file.getPath().toString().get());
			FileException ex(errmsg, __FILE__, __LINE__);
			delete[] errmsg;
			throw ex;
		}

		impl->currentFile = impl->archiveIt->getNext();
		impl->curIdx++;

		if (impl->currentFile.isNull())
			impl->curIdx = -1;
	}
}


FileChildList::Iterator::Iterator(const Iterator& other)
		: impl(other.impl)
{
}


FileChildList::Iterator::Iterator()
		: impl(shared_ptr<Impl>(new Impl(File(), File(), -1, NULL, NULL)))
{
}


FileChildList::Iterator::Impl::~Impl()
{
	if (archiveIt)
		delete archiveIt;

#ifdef _POSIX_VERSION
	if (dir != NULL) {
		closedir(dir);
	}
#else
	if (dirHandle != NULL) {
		FindClose(dirHandle);
	}
#endif
}


FileChildList::Iterator& FileChildList::Iterator::operator=(const Iterator& other)
{
	if (this == &other)
		return *this;

	impl = other.impl;
	return *this;
}


#ifdef _POSIX_VERSION
void FileChildList::Iterator::posixNext()
{
	struct dirent* entry;

	while ((entry = readdir(impl->dir))  !=  NULL) {
		if (strcmp(entry->d_name, ".") != 0  &&  strcmp(entry->d_name, "..") != 0) {
			break;
		}
	}

	if (entry == NULL) {
		impl->currentFile = File();
		impl->curIdx = -1;
		return;
	}

	// Way faster than File(const File&, const char*) because it skips the parent-is-directory
	// check
	FilePath path(impl->iteratedDir.getPath(), entry->d_name);
	impl->currentFile = File(path);
	impl->curIdx++;
}
#endif


FileChildList::Iterator& FileChildList::Iterator::operator++()
{
	if (impl->archiveIt) {
		impl->currentFile = impl->archiveIt->getNext();

		if (impl->currentFile.isNull())
			impl->curIdx = -1;
	} else {
#ifdef _POSIX_VERSION
		posixNext();
#else
		WIN32_FIND_DATA fdata;

		while (true) {
			if (FindNextFile(impl->dirHandle, &fdata) == 0) {
				impl->currentFile = File();
				impl->curIdx = -1;
			} else {
				size_t len = strlen(fdata.cFileName);

				if (len == 1  &&  fdata.cFileName[0] == '.') {
					continue;
				}
				if (len == 2  &&  fdata.cFileName[0] == '.'  &&  fdata.cFileName[1] == '.') {
					continue;
				}

				// Way faster than File(const File&, const char*) because it skips the parent-is-directory
				// check
				FilePath path(impl->iteratedDir.getPath(), fdata.cFileName);
				impl->currentFile = File(path);
				impl->curIdx++;

				//FilePath* path = new FilePath(*iteratedDir->getPath(), fdata.cFileName);
				//nextFile = new File(path, true);
			}

			break;
		}
#endif
	}

	return *this;
}


FileChildList::Iterator FileChildList::Iterator::operator++(int)
{
	Iterator res(*this);
	++(*this);
	return res;
}


bool FileChildList::Iterator::operator==(const Iterator& other) const
{
	return impl->curIdx == -1 ? other.impl->curIdx == -1 : impl == other.impl;
}


FileChildList::FileChildList(const File& iteratedDir)
		: iteratedDir(iteratedDir)
{
}


FileChildList::FileChildList(const FileChildList& other)
		: iteratedDir(other.iteratedDir)
{
}
