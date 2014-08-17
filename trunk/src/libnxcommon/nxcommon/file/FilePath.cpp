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

#include "FilePath.h"
#include "File.h"
#include "../strutil.h"
#include "FileException.h"
#include <cstring>
#include <cstdio>

#ifdef _POSIX_VERSION

#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <dirent.h>

#elif defined(_WIN32)

#include <windows.h>

#endif



const char* FilePath::windowsPathSeparators = "/\\";
const char* FilePath::unixPathSeparators = "/";



FilePath::FilePath(const CString& path, uint8_t syntax)
		: path(normalize(path, syntax)), syntax(syntax)
{
}


FilePath::FilePath(const FilePath& other)
		: path(other.path), syntax(other.syntax)
{
}


FilePath::FilePath(const FilePath& parent, const CString& child)
		: syntax(parent.syntax)
{
	CString tpath(parent.path);
	tpath.append("/");
	tpath.append(child);
	path = normalize(tpath, syntax);
}


FilePath::FilePath()
		: path(CString()), syntax(System)
{
}


FilePath::~FilePath()
{
}


CString FilePath::getExtension() const
{
	const char* cpath = path.get();
	const char* fnameBack = cpath + path.length() - 1;
	const char* cptr = fnameBack;

	while (cptr != cpath-1  &&  *cptr != '/') {
		if (*cptr == '.') {
			//printf("path.substr(%u, %u) @ %u\n", cptr-cpath+1, fnameBack-cptr, path.length());
			return path.substr(cptr-cpath+1, fnameBack-cptr);
		}

		cptr--;
	}

	return CString("");
}


CString FilePath::getFullExtension() const
{
	const char* cpath = path.get();
	const char* fnameBack = cpath + path.length() - 1;
	const char* cptr = fnameBack;

	const char* extStart = NULL;

	while (cptr != cpath-1  &&  *cptr != '/') {
		if (*cptr == '.') {
			extStart = cptr+1;
		}

		cptr--;
	}

	if (extStart) {
		return path.substr(extStart-cpath, fnameBack-extStart+1);
	}

	return CString("");
}


CString FilePath::getFileName() const
{
	size_t plen = path.length();

	if (plen == 0) {
		// Empty path means current path
		return CString(".");
	}

	const char* cpath = path.get();

	//const char* fnameEnd = rtrimfind(cpath, getPathSeparators(syntax));
	const char* fnameBack = cpath + plen - 1;

	if (fnameBack == cpath  &&  cpath[0] == '/') {
		// It's just a slash

		if (syntax == Unix) {
			// That's the root directory!
			return CString("/");
		} else {
			// Well, I guess that's just the current directory plus nothing.
			// TODO Confirm/Disprove this
			return CString(".");
		}
	}

	// Normal case: We have a directory at the end. Find out where it starts

	const char* fnameStart = strrchr(cpath, '/');

	if (!fnameStart) {
		// No slashes at all, so the whole path is just the file name
		return path;
	}

	fnameStart++;

	// Now, fnameStart is the first file name character, and fnameBack is the last
	return path.substr(fnameStart-cpath, fnameBack-fnameStart+1);
}


CString FilePath::getBaseFileName() const
{
	CString fname = getFileName();
	const char* cfname = fname.get();

	const char* dot = strchr(cfname, '.');

	if (!dot)
		return fname;

	return fname.substr(0, dot-cfname);
}


CString FilePath::getFullBaseFileName() const
{
	CString fname = getFileName();
	const char* cfname = fname.get();

	const char* dot = strrchr(cfname, '.');

	if (!dot)
		return fname;

	return fname.substr(0, dot-cfname);
}


FilePath FilePath::getDirectoryPath() const
{
	if (path.isEmpty()) {
		// dirname of current directory, see below.
		return FilePath(CString("."), syntax);
	}

	const char* cpath = path.get();
	const char* fnameBack = cpath + path.length() - 1;

	if (fnameBack == cpath  &&  cpath[0] == '/') {
		// The path consists of just path separators.

		if (syntax == Unix) {
			// This is the root directory. The Linux 'dirname' utility returns the root itself in this case,
			// and so do we.
			return FilePath(CString("/"), Unix);
		} else {
			// Well, I guess that's just the current directory plus nothing.
			// TODO Confirm/Disprove this

			// At least on Linux, the dirname of the current directory is considered the current directory itself.
			return FilePath(CString("."), Windows);
		}
	}

	const char* dirStart = FindLastOccurrence(cpath, fnameBack, '/');

	if (dirStart) {
		if (syntax == Windows  &&  dirStart-cpath == 2  &&  isalpha(cpath[0])  &&  cpath[1] == ':') {
			// This is something like X:/example on Windows. In this case, the '/' is crucial, as it means
			// the root path on drive X.
			return FilePath(path.substr(0, 3), Windows);
		} else if (syntax == Unix  &&  dirStart == cpath) {
			// This is the root directory on Unix
			return FilePath("/", Unix);
		}

		return FilePath(path.substr(0, dirStart-cpath), syntax);
	}

	// At this point, we know that the only path separators are at the end
	if (syntax == Windows  &&  fnameBack-cpath == 2  &&  isalpha(cpath[0])  &&  cpath[1] == ':') {
		// Here, we have something like X:/ (the root directory on drive X). Like on Linux, we define
		// that the the directory path is the root itself in this case.
		return FilePath(path.substr(0, 3), Windows);
	}

	//return path.substr(0, fnameBack-cpath+1);
	return FilePath(CString("."), syntax);
}


FileContentType FilePath::guessContentType() const
{
	CString tmpExt = getExtension();

	if (tmpExt.isNull()) {
		return CONTENT_TYPE_UNKNOWN;
	}

	CString ext = tmpExt.lower();

	FileContentType retval;

	if (ext == CString("img")) {
		retval = CONTENT_TYPE_IMG;
	} else if (ext == CString("dir")) {
		retval = CONTENT_TYPE_DIR;
	} else if (ext == CString("ide")) {
		retval = CONTENT_TYPE_IDE;
	} else if (ext == CString("dff")) {
		retval = CONTENT_TYPE_DFF;
	} else if (ext == CString("ipl")) {
		retval = CONTENT_TYPE_IPL;
	} else if (ext == CString("txd")) {
		retval = CONTENT_TYPE_TXD;
	} else if (ext == CString("dat")) {
		retval = CONTENT_TYPE_DAT;
	} else if (ext == CString("col")) {
		retval = CONTENT_TYPE_COL;
	} else if (ext == CString("ifp")) {
		retval = CONTENT_TYPE_IFP;
	} else if (ext == CString("gxt")) {
		retval = CONTENT_TYPE_GXT;
	} else {
		retval = CONTENT_TYPE_UNKNOWN;
	}

	return retval;
}


/*bool FilePath::isIMGPath() const
{
	FilePath parent = getParentPath();
	bool imgPath = false;

	if (parent.isNull()) {
		return false;
	}

	if (File(parent, false).isRegularFile()) {
		FileContentType type = parent->guessContentType();

		if (type == CONTENT_TYPE_DIR  ||  type == CONTENT_TYPE_IMG) {
			imgPath = true;
		}
	}

	delete parent;
	return imgPath;
}*/


CString FilePath::normalize(const CString& src, uint8_t syntax)
{
	if (src.isNull())
		return src;

	const char* csrc = src.get();

	size_t srcLen = src.length();
	char* cdest = new char[srcLen+1];
	size_t srcIdx = 0, destIdx = 0;

	while (srcIdx < srcLen) {
		char c = csrc[srcIdx];

		if (syntax == Windows  &&  c == '\\')
			c = '/';

		if (c == '/'  &&  destIdx > 0  &&  cdest[destIdx-1] == '/') {
			srcIdx++;
			continue;
		}

		cdest[destIdx] = c;

		srcIdx++;
		destIdx++;
	}

	cdest[destIdx] = '\0';

	if (syntax == Unix  &&  destIdx == 1  &&  cdest[0] == '/') {
		// This is the root path on UNIX, so we may not strip the / away
		return CString::from(cdest);
	} else if (syntax == Windows  &&  destIdx == 3  &&  isalpha(cdest[0])  &&  cdest[1] == ':') {
		// This is a device root path on Windows (X:/), so we may not strip the / away either
		return CString::from(cdest);
	}

	// Strip all slashes from the end
	rtrim(cdest, '/');

	if (cdest[0] == '\0') {
		delete[] cdest;
		return CString(".");
	}

	return CString::from(cdest);
}


bool FilePath::operator==(const FilePath& other) const
{
	return path == other.path;
}


bool FilePath::operator>(const FilePath& other) const
{
	return path > other.path;
}


bool FilePath::operator<(const FilePath& other) const
{
	return path < other.path;
}


FilePath FilePath::getAbsolutePath(const FilePath& cdir) const
{
	if (isAbsolute())
		return *this;

	return FilePath(cdir, path);
}


bool FilePath::isRoot() const
{
	if (syntax == Unix) {
		return path == CString("/");
	} else {
		const char* cpath = path.get();
		return path.length() == 3  &&  isalpha(cpath[0])  &&  cpath[1] == ':'  &&  cpath[2] == '/';
	}
}


/*FilePath* FilePath::absolute() const
{
#ifdef _POSIX_VERSION
	char absPath[MAXPATHLEN];

	if (!realpath(path.get(), absPath)) {
		throw FileException("Error getting absolute path via realpath()!", __FILE__, __LINE__);
	}

	return new FilePath(absPath);
#elif defined(_WIN32)
	char absPath[MAX_PATH];

	if (GetFullPathName(path.get(), sizeof(absPath), absPath, NULL) == 0) {
		throw FileException("Error getting absolute path via GetFullPathName()!", __FILE__, __LINE__);
	}

	return new FilePath(absPath);
#endif
}*/


bool FilePath::isAbsolute() const
{
	if (syntax == Unix) {
		return path.length() > 0  &&  path.get()[0] == '/';
	} else {
		return (path.length() > 2  &&  isalpha(path.get()[0])  &&  path.get()[1] == ':'  &&  path.get()[2] == '/')
			||  (path.length() > 1  &&  path.get()[0] == '/'  &&  path.get()[1] == '/');
	}
}

