#ifndef ABSTRACTSHAREDSTRING_H_
#define ABSTRACTSHAREDSTRING_H_

#include <nxcommon/config.h>
#include "AbstractSharedBuffer.h"
#include "ByteArray.h"
#include <vector>
#include <iterator>



#define ASS_DEFINE_PROTECTED_CONSTRUCTORS(DerivedT) \
	protected: \
		template <typename Deleter> \
		DerivedT(DerivedT::unit_t* data, size_t size, size_t bufSize, Deleter del = default_delete<DerivedT::unit_t[]>()) \
				: AbstractSharedString(data, size, bufSize, del) {} \
		DerivedT(DerivedT::unit_t* data, size_t size, size_t bufSize, bool, bool) \
				: AbstractSharedString(data, size, bufSize, false, false) {} \
		DerivedT(const DerivedT::unit_t* data, size_t size, bool, bool, bool) \
				: AbstractSharedString(data, size, false, false, false) {} \



template <typename ImplT, typename UnitT, UnitT term = 0>
class AbstractSharedString : public AbstractSharedBuffer<ImplT, UnitT, true, term>
{
public:
	typedef UnitT unit_t;

private:
	typedef AbstractSharedBuffer<ImplT, UnitT, true, term> BaseClass;

public:
	template <typename Deleter>
	static ImplT fromCustomDelete(UnitT* s, size_t len, size_t bufSize, Deleter del)
			{ return ImplT(s, len, bufSize-1, del); }
	static ImplT from(UnitT* s, size_t len, size_t bufSize)
			{ return fromCustomDelete(s, len, bufSize, default_delete<UnitT[]>()); }
	static ImplT fromMalloc(UnitT* s, size_t len, size_t bufSize)
			{ return fromCustomDelete(s, len, bufSize, typename BaseClass::MallocFreeDeleter()); }

	template <typename Deleter>
	static ImplT fromCustomDelete(UnitT* s, size_t len, Deleter del)
			{ return fromCustomDelete(s, len, len+1, del); }
	static ImplT from(UnitT* s, size_t len)
			{ return fromCustomDelete(s, len, default_delete<UnitT[]>()); }
	static ImplT fromMalloc(UnitT* s, size_t len)
			{ return fromCustomDelete(s, len, BaseClass::MallocFreeDeleter()); }

	template <typename Deleter>
	static ImplT fromCustomDelete(UnitT* s, Deleter del)
			{ return fromCustomDelete(s, ImplT::strlen(s), del); }
	static ImplT from(UnitT* s)
			{ return fromCustomDelete(s, default_delete<UnitT[]>()); }
	static ImplT fromMalloc(UnitT* s)
			{ return fromCustomDelete(s, typename BaseClass::MallocFreeDeleter()); }

	static ImplT writeAlias(UnitT* s, size_t len, size_t bufSize)
			{ return ImplT(s, len, bufSize-1, false, false); }
	static ImplT writeAlias(UnitT* s, size_t len)
			{ return writeAlias(s, len, len+1); }
	static ImplT writeAlias(UnitT* s)
			{ return writeAlias(s, ImplT::strlen(s)); }

	static ImplT readAlias(const UnitT* s, size_t len)
			{ return ImplT(s, len, false, false, false); }
	static ImplT readAlias(const UnitT* s)
			{ return readAlias(s, ImplT::strlen(s)); }

	static ImplT join(const ImplT& separator, const UnitT** elems, size_t numElems);

	using BaseClass::join;

public:
	AbstractSharedString() : AbstractSharedBuffer<ImplT, UnitT, true, term>() {}

	AbstractSharedString(const AbstractSharedString& other) : BaseClass(other) {}

	AbstractSharedString(const UnitT* str, size_t len) : BaseClass(str, len) {}

	AbstractSharedString(const UnitT* str) : AbstractSharedBuffer<ImplT, UnitT, true, term>(str, str ? ImplT::strlen(str) : 0) {}

	AbstractSharedString(size_t capacity) : AbstractSharedBuffer<ImplT, UnitT, true, term>(capacity) {}

	AbstractSharedString(const ByteArray& other);

	using BaseClass::resize;
	size_t resize() { resize(ImplT::strlen(BaseClass::d.get())); return this->msize; }

	using BaseClass::append;
	ImplT& append(long long val, unsigned int base = 10) { return append(ImplT::convertFromLongLong(val, base)); }
	ImplT& append(unsigned long long val, unsigned int base = 10) { return append(ImplT::convertFromULongLong(val, base)); }
	ImplT& append(long val, unsigned int base = 10) { return append(ImplT::convertFromLong(val, base)); }
	ImplT& append(unsigned long val, unsigned int base = 10) { return append(ImplT::convertFromULong(val, base)); }
	ImplT& append(int val, unsigned int base = 10) { return append(ImplT::convertFromInt(val, base)); }
	ImplT& append(unsigned int val, unsigned int base = 10) { return append(ImplT::convertFromUInt(val, base)); }
	ImplT& append(double val) { return append(ImplT::convertFromDouble(val)); }
	ImplT& append(float val) { return append(ImplT::convertFromFloat(val)); }

	ImplT substr(size_t begin, size_t len) const;
	ImplT substr(size_t begin) const { return substr(begin, this->msize-begin); }

	bool startsWith(const ImplT& other);
	bool endsWith(const ImplT& other);

	using BaseClass::indexOf;

	template <typename OutputIterator>
	void split(OutputIterator out, UnitT separator) const;

	std::vector<ImplT> split(UnitT separator) const { std::vector<ImplT> v; split(std::back_inserter(v), separator); return v; }

	ImplT& operator<<(UnitT c) { return append(c); }
	ImplT& operator<<(const ImplT& other) { return append(other); }
	ImplT& operator<<(long long val) { return append(val); }
	ImplT& operator<<(unsigned long long val) { return append(val); }
	ImplT& operator<<(long val) { return append(val); }
	ImplT& operator<<(unsigned long val) { return append(val); }
	ImplT& operator<<(int val) { return append(val); }
	ImplT& operator<<(unsigned int val) { return append(val); }
	ImplT& operator<<(double val) { return append(val); }
	ImplT& operator<<(float val) { return append(val); }

	/*bool operator==(const ImplT& other) const { return ImplT::strcmp(*static_cast<const ImplT*>(this), other) == 0; }
	bool operator!=(const ImplT& other) const { return ImplT::strcmp(*static_cast<const ImplT*>(this), other) != 0; }
	bool operator<(const ImplT& other) const { return ImplT::strcmp(*static_cast<const ImplT*>(this), other) < 0; }
	bool operator<=(const ImplT& other) const { return ImplT::strcmp(*static_cast<const ImplT*>(this), other) <= 0; }
	bool operator>(const ImplT& other) const { return ImplT::strcmp(*static_cast<const ImplT*>(this), other) > 0; }
	bool operator>=(const ImplT& other) const { return ImplT::strcmp(*static_cast<const ImplT*>(this), other) >= 0; }*/

	using BaseClass::operator==;
	using BaseClass::operator!=;
	using BaseClass::operator<;
	using BaseClass::operator<=;
	using BaseClass::operator>;
	using BaseClass::operator>=;

	bool operator==(const UnitT* other) const { return *this == ImplT(other); }
	bool operator!=(const UnitT* other) const { return *this != ImplT(other); }
	bool operator<(const UnitT* other) const { return *this < ImplT(other); }
	bool operator<=(const UnitT* other) const { return *this <= ImplT(other); }
	bool operator>(const UnitT* other) const { return *this > ImplT(other); }
	bool operator>=(const UnitT* other) const { return *this >= ImplT(other); }

protected:
	template <typename Deleter>
	AbstractSharedString(UnitT* data, size_t size, size_t bufSize, Deleter del = default_delete<UnitT[]>())
			: AbstractSharedBuffer<ImplT, UnitT, true, term>(data, size, bufSize, del) {}
	AbstractSharedString(UnitT* data, size_t size, size_t bufSize, bool, bool)
			: AbstractSharedBuffer<ImplT, UnitT, true, term>(data, size, bufSize, false) {}
	AbstractSharedString(const UnitT* data, size_t size, bool, bool, bool)
			: AbstractSharedBuffer<ImplT, UnitT, true, term>(data, size, false, false) {}

protected:
	static size_t strlen(const UnitT* str)
	{
		size_t len;
		for (len = 0 ; str[len] != term ; len++);
		return len;
	}

	static ImplT convertFromLong(long val, unsigned int base) { return ImplT::convertFromLongLong(val, base); }
	static ImplT convertFromULong(unsigned long val, unsigned int base) { return ImplT::convertFromULongLong(val, base); }
	static ImplT convertFromInt(int val, unsigned int base) { return ImplT::convertFromLong(val, base); }
	static ImplT convertFromUInt(unsigned int val, unsigned int base) { return ImplT::convertFromULong(val, base); }
	static ImplT convertFromFloat(float val) { return ImplT::convertFromDouble(val); }
};




template <typename ImplT, typename UnitT, UnitT term>
AbstractSharedString<ImplT, UnitT, term>::AbstractSharedString(const ByteArray& other)
		: BaseClass(shared_ptr<UnitT>(other.d, (UnitT*) other.d.get()),
				(other.msize + sizeof(UnitT) - 1) / sizeof(UnitT),	// Round up, validity of this new size gets checked below.
																	// If we rounded down here, the terminator might be written in
																	// the valid data region of other, which would be illegal.
				other.mcapacity / sizeof(UnitT) - 1,	// Round down, otherwise we would access the buffer out of bounds.
														// Also, decrease by 1 because we need a slot for the null terminator.
				shared_ptr<UnitT>(other.readAliasDummy, NULL),	// We don't care about the actual pointer in readAliasDummy
				other.isnull)
{
	if (this->mcapacity < this->msize) {
		// It could very well be that the capacity is lower than the size because of the rounding up and decrementing above.
		// Here we have not enough capacity, so we have to fall back to copying the buffer

		// realloc() is guaranteed to actually append the terminator, even if it wasn't there before.
		this->realloc(this->msize);
	} else {
		// Hooray, we have enough capacity!
		// Now we just have to append the null terminator. We can do that because it lies outside the valid data region of other,
		// so other doesn't care about it, and if either this or other want to change the buffer, COW copies them anyway.
		this->d.get()[this->msize] = term;
	}
}



template <typename ImplT, typename UnitT, UnitT term>
ImplT AbstractSharedString<ImplT, UnitT, term>::substr(size_t begin, size_t len) const
{
	UnitT* sub = new UnitT[len+1];
	memcpy(sub, this->d.get() + begin, len * sizeof(UnitT));
	sub[len] = term;
	return ImplT::from(sub, len);
}


template <typename ImplT, typename UnitT, UnitT term>
ImplT AbstractSharedString<ImplT, UnitT, term>::join(const ImplT& separator, const UnitT** elems, size_t numElems)
{
	ImplT joined;

	for (size_t i = 0 ; i < numElems ; i++) {
		if (i != 0) {
			joined.append(separator);
		}
		joined.append(elems[i]);
	}

	return joined;
}


template <typename ImplT, typename UnitT, UnitT term>
bool AbstractSharedString<ImplT, UnitT, term>::startsWith(const ImplT& other)
{
	if (other.length() > this->length()) {
		return false;
	}

	return ImplT::compare(this->d.get(), other.get(), other.length()) == 0;
	//return ImplT::strncmp(*static_cast<const ImplT*>(this), other, other.length()) == 0;
}


template <typename ImplT, typename UnitT, UnitT term>
bool AbstractSharedString<ImplT, UnitT, term>::endsWith(const ImplT& other)
{
	size_t len = this->length();
	size_t olen = other.length();

	if (olen > len) {
		return false;
	}

	// TODO: This should be done without a copy, but ImplT::strncmp() currently doesn't support offsets
	//return substr(len-olen, olen) == other;
	return ImplT::compare(this->d.get() + (len-olen), other.get(), olen) == 0;
}


template <typename ImplT, typename UnitT, UnitT term>
template <typename OutputIterator>
void AbstractSharedString<ImplT, UnitT, term>::split(OutputIterator out, UnitT separator) const
{
	ptrdiff_t prevIdx = 0;
	ptrdiff_t idx = 0;

	while ((idx = indexOf(separator, prevIdx)) != -1) {
		*out++ = substr(prevIdx, idx-prevIdx);
		prevIdx = idx+1;
	}

	*out++ = substr(prevIdx);
}



#endif /* ABSTRACTSHAREDSTRING_H_ */
