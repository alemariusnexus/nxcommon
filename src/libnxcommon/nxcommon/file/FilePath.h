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

#ifndef FILEPATH_H_
#define FILEPATH_H_

#include <nxcommon/config.h>
#include "CString.h"


/**	\brief Content types which can be guessed by FilePath::guessContentType().
 */
enum FileContentType {
	CONTENT_TYPE_UNKNOWN,	//!< The content type is unknown.

	CONTENT_TYPE_IMG,		//!< The file is an IMG archive.
	CONTENT_TYPE_TXD,		//!< The file is a texture dictionary (TXD).
	CONTENT_TYPE_DIR,		//!< The file is a DIR archive.
	CONTENT_TYPE_DFF,		//!< The file is a DFF mesh.
	CONTENT_TYPE_IDE,		//!< The file is an item definition (IDE) file.
	CONTENT_TYPE_IPL,		//!< The file is an item placement (IPL) file.
	CONTENT_TYPE_DAT,		//!< The file is a DAT file.
	CONTENT_TYPE_COL,		//!< The file is a Collision Mesh (COL) file.
	CONTENT_TYPE_IFP,		//!< The file is an animation package (IFP).
	CONTENT_TYPE_GXT		//!< The file is a GXT archive.
};


/**	\brief Represents file paths.
 */
class FilePath {
public:
	enum Syntax
	{
		Unix = 0,
		Windows = 1,

#ifdef _POSIX_VERSION
		System = Unix
#elif defined(_WIN32)
		System = Windows
#endif
	};

	enum Options
	{
		BackslashAsSeparator = 0x01
	};

private:
	static const char* windowsPathSeparators;
	static const char* unixPathSeparators;

public:
	/** \brief Constructs a new FilePath object from a path string.
	 *
	 *	@param path The path string.
	 */
	FilePath(const CString& path, uint8_t syntax = System);

	/**	\brief Copy constructor.
	 *
	 * 	@param other The path to copy.
	 */
	FilePath(const FilePath& other);

	/**	\brief Creates a new FilePath from a parent path and child file name.
	 *
	 * 	No checking will be done to ensure the path really exists.
	 *
	 * 	@param parent The parent FilePath.
	 *	@param child The child file name.
	 */
	FilePath(const FilePath& parent, const CString& child);

	FilePath();

	~FilePath();

	/**	\brief Returns the file name extension of this path.
	 *
	 *	@return The file name extension (without the '.').
	 */
	CString getExtension() const;

	CString getFullExtension() const;

	/**	\brief Returns the file name.
	 *
	 * 	That is the last part of the path.
	 *
	 *	@return The file name.
	 */
	CString getFileName() const;

	CString getBaseFileName() const;

	CString getFullBaseFileName() const;

	/**	\brief Returns the string representation of this path.
	 *
	 *	@return The string representation of this path.
	 */
	CString toString() const { return path; };

	/**	\brief Returns the parent path.
	 *
	 *	@return The parent path or NULL if this path has no parent.
	 */
	//FilePath getParentPath() const;

	FilePath getDirectoryPath() const;

	/**	\brief Tries to guess the content type of the file pointed to by this path by looking at it's extension.
	 *
	 * 	@return The guessed content type.
	 */
	FileContentType guessContentType() const;

	uint8_t getSyntax() const { return syntax; }

	/**	\brief Returns true if this path points to a file inside an IMG or DIR archive.
	 *
	 * 	This is <b>not true for IMG/DIR archives themselves!</b>. See the File class for more information on IMG paths.
	 *
	 *	@return true if this path points to a file inside an IMG/DIR archive.
	 */
	//bool isIMGPath() const;

	//bool isChildOf(const FilePath& other, bool recursive = false) const;

	bool isAbsolute() const;
	bool isRelative() const { return !isAbsolute(); }

	bool isRoot() const;

	FilePath getAbsolutePath(const FilePath& cdir) const;

	bool isNull() const { return path.isNull(); }

	bool operator==(const FilePath& other) const;
	bool operator!=(const FilePath& other) const { return !(*this == other); }
	bool operator>(const FilePath& other) const;
	bool operator>=(const FilePath& other) const { return !(*this < other); }
	bool operator<(const FilePath& other) const;
	bool operator<=(const FilePath& other) const { return !(*this > other); }

public:
	static bool isPathSeparator(char c, uint8_t syntax = System) { return c == '/'  ||  (syntax == Windows  &&  c == '\\'); }

	static const char* getPathSeparators(uint8_t syntax = System)
	{
		if (syntax == Windows) return windowsPathSeparators;
		else return unixPathSeparators;
	}

private:
	/**	\brief Normalizes a path.
	 *
	 * 	This removes directory separators from the end of the path and may do other normalizing tasks.
	 *
	 * 	@param src The input path.
	 * 	@return The normalized path.
	 */
	static CString normalize(const CString& src, uint8_t syntax);

private:
	CString path;
	uint8_t syntax;
};

#endif /* FILEPATH_H_ */
