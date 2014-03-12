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

#ifndef STRINGCOMPARATOR_H_
#define STRINGCOMPARATOR_H_

#include <nxcommon/config.h>
#include <cstring>
#include <memory>
#include "CString.h"

using std::shared_ptr;


class StringComparator
{
public:
	bool operator()(const char* s1, const char* s2) const { return strcmp(s1, s2) < 0; }
	bool operator()(shared_ptr<const char> s1, shared_ptr<const char> s2) const { return (*this)(s1.get(), s2.get()); }
	bool operator()(const CString& s1, const CString& s2) const { return s1 < s2; }
};


class StringEqual
{
public:
	bool operator()(const char* s1, const char* s2) const { return strcmp(s1, s2) == 0; }
	bool operator()(shared_ptr<const char> s1, shared_ptr<const char> s2) const { return (*this)(s1.get(), s2.get()); }
	bool operator()(const CString& s1, const CString& s2) const { return s1 == s2; }
};

#endif /* STRINGCOMPARATOR_H_ */
