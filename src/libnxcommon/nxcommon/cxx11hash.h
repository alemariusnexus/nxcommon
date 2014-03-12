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

#ifndef CXX11HASH_H_
#define CXX11HASH_H_

#include <nxcommon/config.h>
#include <string>
#include <memory>
#include "CString.h"
#include <functional>

using std::size_t;




template <class K>
class CXX11Hash : public std::hash<K> {};

template <>
class CXX11Hash<const char*> {
public:
	size_t operator()(const char* s) const { return strHash(std::string(s)); }
private:
	CXX11Hash<std::string> strHash;
};

template <>
class CXX11Hash<std::shared_ptr<const char> > {
public:
	size_t operator()(std::shared_ptr<const char> s) const { return strHash(std::string(s.get())); }
private:
	CXX11Hash<std::string> strHash;
};

template <>
class CXX11Hash<CString> {
public:
	size_t operator()(const CString& s) const { return strHash(std::string(s.get())); }
private:
	CXX11Hash<std::string> strHash;
};

template <>
class CXX11Hash<void*> {
public:
	size_t operator()(const void* ptr) const { return (size_t) ptr; }
};

#endif /* CXX11HASH_H_ */
