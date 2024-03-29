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

#ifndef _FILE_H_
#define _FILE_H_

#include <nxcommon/config.h>
#include "FilePath.h"
#include "ArchiveHandlerSharedFileData.h"
#include <cstdlib>
#include <vector>
#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>
#include <memory>
#include <unordered_map>


using std::vector;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::istream;
using std::ostream;
using std::iostream;
using std::shared_ptr;
using std::unordered_map;


class InputStream;
class FileFinder;
class FileChildList;
class ArchiveHandler;


#ifdef _POSIX_VERSION
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#elif defined(_WIN32)
#include <windows.h>
#endif





/**	\brief Types of files which can be returned by File::getType().
 */
enum FileType {
	TYPE_ERROR,		//<! An error occured when trying to get the file type.

	TYPE_FILE,		//<! The file is a regular file.
	TYPE_DIRECTORY,	//<! The file is a directory.
	TYPE_LINK,		//<! The file is a (symbolic) link.
	TYPE_OTHER		//<! The file has an unknown type (e.g. device files on Linux).
};





/**	\brief Platform-independent representation of a file.
 *
 * 	This class handles operations on files on the local filesystem. it uses the FilePath class to handle paths to the files. Files
 * 	do not have to exist to be used by this class (but some methods throw exceptions if the file does not exist).
 *
 * 	In addition to normal files, this class can also handle IMG/DIR archives like they were simple directories.
 * 	For example, if '/home/alemariusnexus/gta3.img' was an IMG archive, then a path '/home/alemariusnexus/gta3.img/bistro.txd'
 * 	might be used to access the file 'bistro.txd' inside 'gta3.img'. IMG archives will be treated just like directories and can be
 * 	iterated over. Files inside IMG files may be opened using openStream().
 */
class File {
public:
	typedef int64_t filesize;

private:
	class ArchiveHandlerData
	{
	public:
		~ArchiveHandlerData() { for (auto kv : data) { delete kv.second; } }
		unordered_map<ArchiveHandler*, ArchiveHandlerSharedFileData*> data;
	};

public:
	static File createTemporaryFile();
	static File createTemporaryDirectory();
	static File getCurrentDirectory();
	static void setCurrentDirectory(const File& cdir);
	static File getExecutableFile();

public:
	/**	\brief Constructs a file from the given path.
	 *
	 * 	@param path The path to the file (does not have to exist).
	 */
	File(const CString& path, FilePath::Syntax syntax = FilePath::System);

	/**	\brief Constructs a file from a given path.
	 *
	 * 	The path will be used without being copied.
	 *
	 *	@param path The path to be used. This constructor will not make a copy of it. Therefore, don't delete it without this
	 *		file being deleted too.
	 *	@param autoDeletePath If true, the path will be automatically deleted by this file's destructor.
	 */
	File(const FilePath& path);

	/**	\brief Copy constructor.
	 */
	File(const File& other);

	/**	\brief Construct a file from a parent and child file.
	 *
	 * 	@param parent The parent file.
	 * 	@param The child file.
	 */
	File(const File& parent, const CString& child);

	File();

	bool physicallyExists() const;

	/**	\brief Checks whether this file exists.
	 *
	 * 	Existance checks are also performed for files inside IMG archives. If you don't need this, consider using physicallyExists(),
	 * 	as it is quicker in some cases.
	 *
	 *	@return true if the file exists (files inside IMG/DIR archives are checked, too).
	 */
	bool exists() const;

	/**	\brief Determines the type of this file (e.g. regular file, directory, link).
	 *
	 * 	@return The file type or TYPE_ERROR in case of error.
	 */
	FileType getType() const;

	/**	\brief Get an iterator over the child files and directories of this file.
	 *
	 * 	IMG archives can be iterated, too. The entries will be iterated then.
	 *
	 *	@return The child iterator.
	 */

	FileChildList getChildren() const;

	File getAbsoluteFile(const File& cdir = File()) const;

	File getCanonicalFile(const File& cdir = File()) const;

	/**	\brief Returns the parent of this file.
	 *
	 *	That is the directory in which this file resides.
	 *
	 *	@return The parent of this file or NULL if it has no parent.
	 */
	File getParent() const;

	/**	\brief Returns this files FilePath.
	 *
	 * 	@return The path of this file.
	 */
	FilePath getPath() const { return path; }

	/**	\brief Returns true if this file is a directory.
	 *
	 * 	@return true if it's a directory.
	 */
	bool isDirectory() const { return getType() == TYPE_DIRECTORY; }

	/**	\brief Returns true if this file is a (symbolic) link.
	 *
	 * 	@return true if it's a link.
	 */
	bool isLink() const { return getType() == TYPE_LINK; }

	/**	\brief Returns true if this file is a regular file (no directory, link etc.).
	 *
	 * 	@return true if it's a regular file.
	 */
	bool isRegularFile() const { return getType() == TYPE_FILE; }

	/**	\brief Tries to guess the content type of this file by looking at the file extension.
	 *
	 *	@return The guesses content type.
	 *	@see FilePath::guessContentType()
	 */
	FileContentType guessContentType() const { return path.guessContentType(); }

	/**	\brief Opens a stream on this file.
	 *
	 * 	Files inside IMG/DIR archives can also be opened. The corresponding entries will then be opened using IMGArchive.
	 *
	 * 	@param flags The stream flags.
	 * 	@return The newly created stream.
	 */
	istream* openInputStream(ifstream::openmode mode = ifstream::in) const;

	ostream* openOutputStream(ostream::openmode mode = ostream::out) const;

	iostream* openInputOutputStream(iostream::openmode mode = iostream::in | iostream::out) const;

	ByteArray readAll(ifstream::openmode mode = ifstream::in) const;

	unsigned int getChildCount(bool recursive = false, bool archiveEntries = false) const;

	filesize getSize() const;

	bool mkdir() const;

	bool mkdirs() const;

	bool remove() const;

	void resize(filesize size) const;

	uint64_t getModifyTime() const;

	bool isChildOf(const File& other, bool recursive = false) const;

	File findChild(FileFinder& finder, bool& found, bool recursive = false, bool archiveEntries = false) const;

	unsigned int findChildren(FileFinder& finder, vector<File>& results, bool recursive = false, bool archiveEntries = false) const;

	void copyTo(const File& newFile) const;
	void copyFrom(istream* inStream) const;
	void copyTo(ostream* stream) const;

	File& operator=(const File& other);
	bool operator==(const File& other) const { return path == other.path; }
	bool operator!=(const File& other) const { return !(*this == other); }
	bool operator>(const File& other) const { return path > other.path; }
	bool operator>=(const File& other) const { return !(*this < other); }
	bool operator<(const File& other) const { return path < other.path; }
	bool operator<=(const File& other) const { return !(*this > other); }

	uint32_t crc32() const;

	bool isNull() const { return path.isNull(); }

	File correctCase(const File& cdir = File()) const;

	// TODO Implement
	File relativeTo(const File& dir) const { return *this; }

	ArchiveHandlerSharedFileData* getArchiveHandlerData(ArchiveHandler* handler) const;

	void setArchiveHandlerData(ArchiveHandler* handler, ArchiveHandlerSharedFileData* data) const;

	bool isArchiveDirectory() const;

	bool isDirectoryOrArchiveDirectory() const;

	bool isArchiveEntry() const;

	CString toString() const { return path.toString(); }

	CString getFileName() const { return path.getFileName(); }

	CString getExtension() const { return path.getExtension(); }

	CString getFullExtension() const { return path.getFullExtension(); }

	CString getBaseFileName() const { return path.getBaseFileName(); }

	CString getFullBaseFileName() const { return path.getFullBaseFileName(); }

private:
	FilePath path;
	shared_ptr<ArchiveHandlerData> archiveHandlerData;
};

#endif /* _FILE_H_ */
