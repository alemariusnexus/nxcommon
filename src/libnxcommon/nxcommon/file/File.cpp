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

#include "_File.h"
#include <cstdio>
#include <cstring>
#include <cassert>
#include "../util.h"
#include "FileException.h"
#include "FileFinder.h"
#include "FileChildList.h"
#include "FileSystem.h"
#include "../stream/RangedStream.h"
#include "../CRC32.h"
#include <map>
#include <utility>
#include <list>

#ifdef _POSIX_VERSION
#include <errno.h>
#endif


using std::map;
using std::pair;
using std::list;







File::File(const CString& path, FilePath::Syntax syntax)
		: path(FilePath(path, syntax)), archiveHandlerData(shared_ptr<ArchiveHandlerData>(new ArchiveHandlerData))
{
}


File::File(const FilePath& path)
		: path(path), archiveHandlerData(shared_ptr<ArchiveHandlerData>(new ArchiveHandlerData))
{
}


File::File(const File& other)
		: path(other.path), archiveHandlerData(other.archiveHandlerData)
{
}


File::File(const File& parent, const CString& child)
		: archiveHandlerData(shared_ptr<ArchiveHandlerData>(new ArchiveHandlerData))
{
	/*if (parent.exists()  &&  !parent.isDirectory()) {
		FileContentType type = parent.guessContentType();

		if (type != CONTENT_TYPE_DIR  &&  type != CONTENT_TYPE_IMG) {
			char* errmsg = new char[parent.getPath()->toString().length() + 128];
			sprintf(errmsg, "Attempt to call File::File(const File& parent, const char* child) with non-directory '%s' as parent",
					parent.getPath()->toString().get());
			FileException ex(errmsg, __FILE__, __LINE__);
			delete[] errmsg;
			throw ex;
		}
	}*/

	path = FilePath(parent.getPath(), child);
}


File::File()
		: path(FilePath()), archiveHandlerData(shared_ptr<ArchiveHandlerData>(NULL))
{
}


File File::getCurrentDirectory()
{
#ifdef _POSIX_VERSION
	char buf[PATH_MAX];
	return File(getcwd(buf, sizeof(buf)));
#elif defined(_WIN32)
	DWORD size = GetCurrentDirectory(0, NULL);
	char* buf = new char[size+1];
	GetCurrentDirectory(size+1, buf);
	return File(CString::from(buf));
#endif
}


void File::setCurrentDirectory(const File& cdir)
{
#ifdef _POSIX_VERSION
	if (chdir(cdir.getPath().toString().get()) != 0) {
		throw FileException("Error calling chdir(const char*)!", __FILE__, __LINE__);
	}
#elif defined(_WIN32)
	if (SetCurrentDirectory(cdir.getPath().toString().get()) == 0) {
		throw FileException("Error calling SetCurrentDirectory()!", __FILE__, __LINE__);
	}
#endif
}


File File::createTemporaryFile()
{
	// tmpnam() is dangerous, so we try to use platform-specific functions

#ifdef _POSIX_VERSION
	const char* tmpDir;
#ifdef P_tmpdir
	tmpDir = P_tmpdir;
#else
	tmpDir = getenv("TMPDIR");

	if (!tmpDir) {
		tmpDir = "/tmp";
	}
#endif // P_tmpdir

	char* tmpTemplate = new char[strlen(tmpDir)+16];
	sprintf(tmpTemplate, "%s/nxc-XXXXXX", tmpDir);
	close(mkstemp(tmpTemplate));
	File file(tmpTemplate);
	delete[] tmpTemplate;
	return file;
#elif defined(_WIN32)
	DWORD status;
	char tmpPath[512];
	if ((status = GetTempPath(sizeof(tmpPath), tmpPath))  ==  0) {
		char* errmsg = new char[128];
		sprintf(errmsg, "Internal error creating temporary file (in GetTempPath()): %d.", (int) status);
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}
	char tmpFilePath[1024];
	if ((status = GetTempFileName(tmpPath, "nxc", 0, tmpFilePath))  ==  0) {
		char* errmsg = new char[128];
		sprintf(errmsg, "Internal error creating temporary file (in GetTempFileName()): %d.", (int) status);
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}
	return File(tmpFilePath);
#else
	return File(tmpnam());
#endif // _POSIX_VERSION
}


File File::createTemporaryDirectory()
{
	// tmpnam() is dangerous, so we try to use platform-specific functions

#if defined(_POSIX_VERSION)  &&  !defined(__ANDROID__)
	const char* tmpDir;
#ifdef P_tmpdir
	tmpDir = P_tmpdir;
#else
	tmpDir = getenv("TMPDIR");

	if (!tmpDir) {
		tmpDir = "/tmp";
	}
#endif // P_tmpdir

	char* tmpTemplate = new char[strlen(tmpDir)+16];
	sprintf(tmpTemplate, "%s/nxc-XXXXXX", tmpDir);
	mkdtemp(tmpTemplate);
	File file(tmpTemplate);
	delete[] tmpTemplate;
	return file;
#elif defined(_WIN32)
	DWORD status;
	char tmpPath[512];
	if ((status = GetTempPath(sizeof(tmpPath), tmpPath))  ==  0) {
		char* errmsg = new char[128];
		sprintf(errmsg, "Internal error creating temporary file (in GetTempPath()): %d.", (int) status);
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}

	UUID uuid;

	UuidCreate(&uuid);

	char* uuidStr;

	UuidToString(&uuid, (unsigned char**) &uuidStr);

	char tmpDirPath[1024];
	sprintf(tmpDirPath, "%s/nxc-%s.TMP", tmpPath, uuidStr);

	RpcStringFree((unsigned char**) &uuidStr);

	File dir(tmpDirPath);
	dir.mkdir();

	return dir;
#else
	File dir(tmpnam(NULL));
	dir.mkdir();
	return dir;
#endif // _POSIX_VERSION
}


File File::getExecutableFile()
{
#ifdef _POSIX_VERSION
	File exe("/proc/self/exe");

	if (exe.physicallyExists()) {
		return exe.getCanonicalFile();
	} else {
		return File();
	}
#elif defined(_WIN32)
	char path[MAX_PATH];

	GetModuleFileName(NULL, path, sizeof(path));

	return File(path);
#endif
}


bool File::physicallyExists() const
{
#ifdef _POSIX_VERSION
	struct stat fileInfo;
	return stat(path.toString().get(), &fileInfo) == 0;
#elif defined(_WIN32)
	DWORD attribs = GetFileAttributes(path.toString().get());
	return attribs != 0xFFFFFFFF;
#endif
}


bool File::exists() const
{
	if (physicallyExists())
		return true;

	for (ArchiveHandler* handler : FileSystem::getInstance()->getArchiveHandlers()) {
		if (handler->exists(*this))
			return true;
	}

	return false;
}


FileType File::getType() const
{
	if (!physicallyExists()) {
		for (auto handler : FileSystem::getInstance()->getArchiveHandlers()) {
			FileType type = handler->getType(*this);

			if (type != TYPE_ERROR)
				return type;
		}

		return TYPE_ERROR;
	}

#ifdef _POSIX_VERSION
	struct stat fileInfo;

	if (stat(path.toString().get(), &fileInfo) != 0) {
		return TYPE_ERROR;
	}

	if (S_ISDIR(fileInfo.st_mode)) {
		return TYPE_DIRECTORY;
	} else if (S_ISREG(fileInfo.st_mode)) {
		return TYPE_FILE;
	} else if (S_ISLNK(fileInfo.st_mode)) {
		return TYPE_LINK;
	} else {
		return TYPE_OTHER;
	}
#elif defined(_WIN32)
	DWORD attribs = GetFileAttributes(path.toString().get());

	if (attribs == INVALID_FILE_ATTRIBUTES) {
		return TYPE_ERROR;
	}

	if ((attribs & FILE_ATTRIBUTE_DIRECTORY) != 0) {
		return TYPE_DIRECTORY;
	} else {
		return TYPE_FILE;
	}
#endif
}


FileChildList File::getChildren() const
{
	return FileChildList(*this);
}


File File::getAbsoluteFile(const File& cdir) const
{
	if (path.isAbsolute())
		return *this;

	if (cdir.isNull()) {
		return File(path.getAbsolutePath(getCurrentDirectory().getPath()));
	} else {
		return File(path.getAbsolutePath(cdir.getPath()));
	}
}


File File::getCanonicalFile(const File& cdir) const
{
	File canCdir;

	if (cdir.isNull()  ||  cdir.physicallyExists()) {
		File oldCdir;
		File canFile;

		if (!cdir.isNull()) {
			oldCdir = getCurrentDirectory();
			setCurrentDirectory(cdir);
		}

		if (physicallyExists()) {
#ifdef _POSIX_VERSION
			char absPath[PATH_MAX];

			if (!realpath(path.toString().get(), absPath)) {
				throw FileException("Error getting canonical path via realpath()!", __FILE__, __LINE__);
			}

			canFile = File(absPath);
#elif defined(_WIN32)
			char absPath[MAX_PATH];

			if (GetFullPathName(path.toString().get(), sizeof(absPath), absPath, NULL) == 0) {
				throw FileException("Error getting canonical path via GetFullPathName()!", __FILE__, __LINE__);
			}

			canFile = File(absPath);
#endif
		} else if (exists()) {
			for (auto handler : FileSystem::getInstance()->getArchiveHandlers()) {
				canFile = handler->getCanonicalFile(*this);

				if (!canFile.isNull())
					break;
			}

			if (canFile.isNull()) {
				throw FileException("Error getting canonical path from ArchiveHandler!", __FILE__, __LINE__);
			}
		} else {
			throw FileException("Attempted to get canonical path to non-existent file!", __FILE__, __LINE__);
		}

		if (!oldCdir.isNull()) {
			setCurrentDirectory(oldCdir);
		}

		return canFile;
	} else {
		// cdir doesn't physically exist.

		File canFile;

		for (auto handler : FileSystem::getInstance()->getArchiveHandlers()) {
			canFile = handler->getCanonicalFile(*this, cdir);

			if (!canFile.isNull())
				break;
		}

		if (canFile.isNull()) {
			throw FileException("Error getting canonical path from ArchiveHandler!", __FILE__, __LINE__);
		}

		return canFile;
	}
}


File File::getParent() const
{
	// NOTE: This will RESOLVE the path, so if this file has a relative path, it will be interpreted relative
	// to the current working directory!

	File cfile = getCanonicalFile();

	if (cfile.path.isRoot()) {
		return File();
	}

	FilePath ppath = cfile.path.getDirectoryPath();
	return File(ppath);
}


istream* File::openInputStream(ifstream::openmode mode) const
{
	if (physicallyExists()) {
		if (isRegularFile()) {
			return new ifstream(path.toString().get(), mode | ifstream::in);
		} else {
			char* errMsg = new char[path.toString().length() + 64];
			sprintf(errMsg, "Attempt to open stream on non-regular file %s.", path.toString().get());
			FileException fex(errMsg, __FILE__, __LINE__);
			delete[] errMsg;
			throw fex;
		}
	} else if (exists()) {
		for (auto handler : FileSystem::getInstance()->getArchiveHandlers()) {
			istream* stream = handler->openInputStream(*this, mode | istream::in);

			if (stream)
				return stream;
		}

		char* errmsg = new char[path.toString().length() + 64];
		sprintf(errmsg, "Attempt to open stream on file that isn't supported by any ArchiveHandler: %s.",
				path.toString().get());
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	} else {
		char* errMsg = new char[path.toString().length() + 64];
		sprintf(errMsg, "Attempt to open stream on non-existent file %s.", path.toString().get());
		FileException fex(errMsg, __FILE__, __LINE__);
		delete[] errMsg;
		throw fex;
	}

	return NULL;
}


ostream* File::openOutputStream(ostream::openmode mode) const
{
	for (auto handler : FileSystem::getInstance()->getArchiveHandlers()) {
		ostream* stream = handler->openOutputStream(*this, mode | ostream::out);

		if (stream)
			return stream;
	}

	return new ofstream(path.toString().get(), mode | ostream::out);
}


iostream* File::openInputOutputStream(iostream::openmode mode) const
{
	for (auto handler : FileSystem::getInstance()->getArchiveHandlers()) {
		iostream* stream = handler->openInputOutputStream(*this, mode | iostream::out | iostream::in);

		if (stream)
			return stream;
	}

	if (!exists()) {
		ofstream tmp(path.toString().get());
		tmp.close();
	}
	return new fstream(path.toString().get(), mode | iostream::out | iostream::in);
}


ByteArray File::readAll(ifstream::openmode mode) const
{
	filesize sz = getSize();
	istream* in = openInputStream(mode);

	char* content = new char[sz];

	in->read(content, sz);

	// NOTE: Sometimes numRead != sz, because things like newline conversion might be involved.
	// This was the cause of a long-standing bug in libnxcommon...
	std::streamsize numRead = in->gcount();

	delete in;

	return ByteArray::from(content, numRead, sz);
}


unsigned int File::getChildCount(bool recursive, bool archiveEntries) const
{
	if (!archiveEntries  &&  isArchiveDirectory())
		return 0;
	if (!isDirectoryOrArchiveDirectory())
		return 0;

	unsigned int ccount = 0;

	for (File child : getChildren()) {
		ccount++;

		if (recursive)
			ccount += child.getChildCount(true, archiveEntries);
	}

	return ccount;
}


File::filesize File::getSize() const
{
	if (!exists()) {
		char* errMsg = new char[path.toString().length() + 64];
		sprintf(errMsg, "Attempt to get size of non-existent file %s", path.toString().get());
		FileException ex(errMsg);
		delete[] errMsg;
		throw ex;
	}

	if (!physicallyExists()  ||  !isRegularFile()) {
		if (!physicallyExists()) {
			for (auto handler : FileSystem::getInstance()->getArchiveHandlers()) {
				bool supported = false;
				filesize size = handler->getSize(*this, supported);

				if (supported)
					return size;
			}
		}

		char* errmsg = new char[path.toString().length() + 128];
		sprintf(errmsg, "Attemp to get size of non-regular file that is not supported by any ArchiveHandler: '%s'",
				path.toString().get());
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}

#ifdef _POSIX_VERSION
	struct stat fileInfo;

	if (stat(path.toString().get(), &fileInfo) != 0) {
		char* errStr = strerror(errno);
		char* errMsg = new char[strlen(errStr) + path.toString().length() + 64];
		sprintf(errMsg, "Internal error receiving size of file %s: %s", path.toString().get(), errStr);
		FileException ex(errMsg, __FILE__, __LINE__);
		delete[] errMsg;
		throw ex;
	}

	return fileInfo.st_size;
#elif defined(_WIN32)
    istream* stream = openInputStream(ifstream::binary | ifstream::in);
    stream->seekg(0, istream::end);
    filesize size = stream->tellg();
    delete stream;
    return size;
#endif
}


bool File::mkdir() const
{
#ifdef _POSIX_VERSION
	return ::mkdir(path.toString().get(), S_IRWXU | S_IRWXG | S_IRWXO) == 0;
#elif defined(_WIN32)
	return CreateDirectory(path.toString().get(), NULL) != 0;
#endif
}


bool File::mkdirs() const
{
	File parent(path.getDirectoryPath());

	if (!parent.isNull()  &&  !parent.exists()) {
		if (!parent.mkdirs()) {
			return false;
		}
	}

	return mkdir();
}


bool File::remove() const
{
	return ::remove(path.toString().get()) == 0;
}


void File::resize(filesize size) const
{
#ifdef _POSIX_VERSION
	truncate(path.toString().get(), size);
#elif defined(_WIN32)
	HANDLE fhandle = CreateFile(path.toString().get(), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);
	LARGE_INTEGER sizeVal;
	sizeVal.QuadPart = size;
	SetFilePointerEx(fhandle, sizeVal, 0, FILE_BEGIN);
	SetEndOfFile(fhandle);
	CloseHandle(fhandle);
#endif
}


uint64_t File::getModifyTime() const
{
#ifdef _POSIX_VERSION
	struct stat fileInfo;

	if (stat(path.toString().get(), &fileInfo) != 0) {
		char* errStr = strerror(errno);
		char* errmsg = new char[64 + strlen(errStr) + path.toString().length()];
		sprintf(errmsg, "Error getting modification time of file '%s': %s", path.toString().get(), errStr);
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}

	return fileInfo.st_mtime*1000;
#else
	HANDLE handle = CreateFile(path.toString().get(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE
			| FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	FILETIME mtime;

	if (GetFileTime(handle, NULL, NULL, &mtime) == 0) {
		char* errmsg = new char[128 + path.toString().length()];
		sprintf(errmsg, "Error getting modification time of file '%s'. Error code: %u",
				path.toString().get(), GetLastError());
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}

	CloseHandle(handle);
	uint64_t t =  (((uint64_t) mtime.dwLowDateTime | ((uint64_t) mtime.dwHighDateTime << 32))
			- 116444736000000000) / 10000;
	return t - (t % 1000);
#endif
}


bool File::isChildOf(const File& other, bool recursive) const
{
	File parent = getParent();

	if (getPath().isRoot())
		return *this == other;
	if (parent.isNull())
		return false;
	if (parent == other)
		return true;
	if (recursive)
		return parent.isChildOf(other, true);

	return false;
}


File File::findChild(FileFinder& finder, bool& found, bool recursive, bool archiveEntries) const
{
	if (!archiveEntries  &&  isArchiveEntry()) {
		found = false;
		return File();
	}

	for (File child : getChildren()) {
		if (finder.matches(child)) {
			found = true;
			return child;
		}

		if (finder.isInterrupted()) {
			found = false;
			return File();
		}

		if (recursive  &&  child.isDirectoryOrArchiveDirectory()) {
			bool cfound;
			File match = child.findChild(finder, cfound, true, archiveEntries);

			if (cfound) {
				found = true;
				return match;
			}
		}
	}

	found = false;
	return File();
}


unsigned int File::findChildren(FileFinder& finder, vector<File>& results, bool recursive, bool archiveEntries) const
{
	if (!archiveEntries  &&  isArchiveEntry())
		return 0;

	unsigned int matchCount = 0;

	for (File child : getChildren()) {
		bool matches = finder.matches(child);
		if (matches) {
			results.push_back(child);
			matchCount++;
		}

		if (finder.isInterrupted()) {
			return matchCount;
		}

		if (recursive  &&  child.isDirectoryOrArchiveDirectory()) {
			matchCount += child.findChildren(finder, results, true, archiveEntries);
		}
	}

	return matchCount;
}


void File::copyTo(const File& newFile) const
{
	istream* inStream = openInputStream(istream::binary);
	newFile.copyFrom(inStream);
}


void File::copyFrom(istream* inStream) const
{
	ostream* outStream = openOutputStream(ostream::binary);

	char buf[8192];

	while (!inStream->eof()) {
		inStream->read(buf, sizeof(buf));
		outStream->write(buf, inStream->gcount());
	}

	inStream->clear();

	delete outStream;
}


void File::copyTo(ostream* stream) const
{
	istream* inStream = openInputStream(istream::binary);

	char buf[8192];

	while (!inStream->eof()) {
		inStream->read(buf, sizeof(buf));
		stream->write(buf, inStream->gcount());
	}

	delete inStream;
}


File& File::operator=(const File& other)
{
	if (this == &other) {
		return *this;
	}

	path = other.path;
	archiveHandlerData = other.archiveHandlerData;
	return *this;
}


uint32_t File::crc32() const
{
	istream* stream = openInputStream(istream::binary);

	CRC32 crc;
	char buf[4096];

	while (!stream->eof()) {
		stream->read(buf, sizeof(buf));
		crc.append(buf, stream->gcount());
	}

	delete stream;

	return crc.getChecksum();
}


File File::correctCase(const File& cdir) const
{
	// This used to be implemented by checking if the absolute file exists, and if not, doing so repeatedly on
	// the parent file, until the file existed, and afterwards finding the correct case child by child.
	// This does obviously not work when we deal with a case-insensitive file system (like NTFS on Win32),
	// because then the file does exist even if it has the WRONG case.

	File absFile = getAbsoluteFile(cdir);

	list<CString> components;

	// Get a list of the path's components (excluding the root element)
	FilePath partialPath = absFile.getPath();
	assert(partialPath.isAbsolute());
	while (!partialPath.isRoot()) {
		components.push_front(partialPath.getFileName().lower());
		partialPath = partialPath.getDirectoryPath();
	};

	File baseDir;

	if (partialPath.getSyntax() == FilePath::Windows) {
		// TODO: Find a way to correct case for the drive letter

		// The drive letter is still left in pertialPath
		baseDir = File(partialPath);
	} else {
		baseDir = File("/");
	}

	// Recursively, iterate over all children until we find one that matches the current component case-insensitively, then
	// append to baseDir its case-SENSITIVE form.
	for (CString comp : components) {
		// comp is lower-case (see while-loop above)

		FileChildList children = baseDir.getChildren();

		bool found = false;
		for (File child : children) {
			if (child.getFileName().lower() == comp) {
				baseDir = File(baseDir, child.getFileName());
				found = true;
				break;
			}
		}

		if (!found) {
			char* errmsg = new char[256 + baseDir.getPath().toString().length() + comp.length()];
			sprintf(errmsg, "Couldn't correct case. Directory '%s' does not have a child with case-insensitive name '%s'!",
					baseDir.getPath().toString().get(), comp.get());
			FileException ex(errmsg, __FILE__, __LINE__);
			delete[] errmsg;
			throw ex;
		}
	}

	return baseDir;
}


ArchiveHandlerSharedFileData* File::getArchiveHandlerData(ArchiveHandler* handler) const
{
	auto it = archiveHandlerData->data.find(handler);

	if (it == archiveHandlerData->data.end())
		return NULL;

	return it->second;
}


void File::setArchiveHandlerData(ArchiveHandler* handler, ArchiveHandlerSharedFileData* data) const
{
	const_cast<File*>(this)->archiveHandlerData->data[handler] = data;
}


bool File::isDirectoryOrArchiveDirectory() const
{
	if (isDirectory())
		return true;

	return isArchiveDirectory();
}


bool File::isArchiveDirectory() const
{
	for (auto handler : FileSystem::getInstance()->getArchiveHandlers()) {
		if (handler->isArchiveDirectory(*this))
			return true;
	}

	return false;
}


bool File::isArchiveEntry() const
{
	for (auto handler : FileSystem::getInstance()->getArchiveHandlers()) {
		if (handler->isArchiveEntry(*this))
			return true;
	}

	return false;
}

