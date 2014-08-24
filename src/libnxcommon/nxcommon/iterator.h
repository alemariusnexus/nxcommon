/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef NXCOMMON_ITERATOR_H_
#define NXCOMMON_ITERATOR_H_

#include <nxcommon/config.h>



template <class T>
class IteratorIterable
{
private:
	T& container;

public:
	typedef decltype(container.begin()) BackendIterator;

public:
	class IteratorIterator
	{
	public:
		IteratorIterator() : it() {}
		IteratorIterator(BackendIterator it) : it(it) {}
		IteratorIterator(const IteratorIterator& other) : it(other.it) {}

		IteratorIterator& operator=(const IteratorIterator& other) { if (&other == this) return *this; it = other.it; return *this; }

		BackendIterator operator*() const { return it; }
		const BackendIterator* operator->() const { return &it; }

		bool operator==(const IteratorIterator& other) const { return it == other.it; }
		bool operator!=(const IteratorIterator& other) const { return it != other.it; }
		bool operator<(const IteratorIterator& other) const { return it < other.it; }
		bool operator>(const IteratorIterator& other) const { return it > other.it; }
		bool operator<=(const IteratorIterator& other) const { return it <= other.it; }
		bool operator>=(const IteratorIterator& other) const { return it >= other.it; }

		IteratorIterator operator+(size_t n) { return IteratorIterator(it + n); }
		IteratorIterator operator-(size_t n) { return IteratorIterator(it - n); }

		const BackendIterator& operator[](size_t idx) const { return it+idx; }

		IteratorIterator& operator++() { ++it; return *this; }
		IteratorIterator operator++(int) { IteratorIterator cpy(*this); ++(*this); return cpy; }

		IteratorIterator& operator--() { --it; return *this; }
		IteratorIterator operator--(int) { IteratorIterator cpy(*this); --(*this); return cpy; }

	private:
		BackendIterator it;
	};

public:
	IteratorIterable(T& container) : container(container) {}

	IteratorIterator begin() const { return IteratorIterator(container.begin()); }
	IteratorIterator end() const { return IteratorIterator(container.end()); }
};


template <class T>
IteratorIterable<T> ItIt(T& container)
{
	return IteratorIterable<T>(container);
}



template <class T>
class RangeIterable
{
public:
	RangeIterable() {}
	RangeIterable(const T& beg, const T& end) : ibeg(beg), iend(end) {}
	RangeIterable(const RangeIterable& other) : ibeg(other.ibeg), iend(other.iend) {}

	T begin() const { return ibeg; }
	T end() const { return iend; }

private:
	T ibeg, iend;
};


template <class T>
RangeIterable<T> ItRange(const T& beg, const T& end)
{
	return RangeIterable<T>(beg, end);
}


#endif /* NXCOMMON_ITERATOR_H_ */
