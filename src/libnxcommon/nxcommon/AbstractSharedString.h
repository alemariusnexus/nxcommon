#ifndef ABSTRACTSHAREDSTRING_H_
#define ABSTRACTSHAREDSTRING_H_

#include "AbstractSharedBuffer.h"
#include "ByteArray.h"



template <typename ImplT, typename UnitT, UnitT term = 0>
class AbstractSharedString : public AbstractSharedBuffer<ImplT, UnitT, true, term>
{
private:
	typedef AbstractSharedBuffer<ImplT, UnitT, true, term> BaseClass;

public:
	static ImplT from(UnitT* s, size_t len, size_t bufSize)
			{ return ImplT(s, len, bufSize-1, false); }
	static ImplT from(UnitT* s, size_t len)
			{ return from(s, len, len+1); }
	static ImplT from(UnitT* s)
			{ return from(s, ImplT::strlen(s)); }

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
	ImplT& append(long val, unsigned int base = 10) { return append(ImplT::convertFromLong(val, base)); }
	ImplT& append(unsigned long val, unsigned int base = 10) { return append(ImplT::convertFromULong(val, base)); }
	ImplT& append(int val, unsigned int base = 10) { return append(ImplT::convertFromInt(val, base)); }
	ImplT& append(unsigned int val, unsigned int base = 10) { return append(ImplT::convertFromUInt(val, base)); }
	ImplT& append(double val) { return append(ImplT::convertFromDouble(val)); }
	ImplT& append(float val) { return append(ImplT::convertFromFloat(val)); }

	ImplT substr(size_t begin, size_t len) const;
	ImplT substr(size_t begin) const { return substr(begin, this->msize-begin); }

	ImplT& operator<<(UnitT c) { return append(c); }
	ImplT& operator<<(const ImplT& other) { return append(other); }
	ImplT& operator<<(long val) { return append(val); }
	ImplT& operator<<(unsigned long val) { return append(val); }
	ImplT& operator<<(int val) { return append(val); }
	ImplT& operator<<(unsigned int val) { return append(val); }
	ImplT& operator<<(double val) { return append(val); }
	ImplT& operator<<(float val) { return append(val); }

	bool operator==(const ImplT& other) const { return ImplT::strcmp(*static_cast<const ImplT*>(this), other) == 0; }
	bool operator!=(const ImplT& other) const { return ImplT::strcmp(*static_cast<const ImplT*>(this), other) != 0; }
	bool operator<(const ImplT& other) const { return ImplT::strcmp(*static_cast<const ImplT*>(this), other) < 0; }
	bool operator<=(const ImplT& other) const { return ImplT::strcmp(*static_cast<const ImplT*>(this), other) <= 0; }
	bool operator>(const ImplT& other) const { return ImplT::strcmp(*static_cast<const ImplT*>(this), other) > 0; }
	bool operator>=(const ImplT& other) const { return ImplT::strcmp(*static_cast<const ImplT*>(this), other) >= 0; }

protected:
	AbstractSharedString(UnitT* data, size_t size, size_t bufSize, bool)
			: AbstractSharedBuffer<ImplT, UnitT, true, term>(data, size, bufSize, default_delete<UnitT[]>()) {}
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

	static int strcmp(const ImplT& s1, const ImplT& s2)
	{
		size_t s1Len = s1.length();
		size_t s2Len = s2.length();
		int res = memcmp(s1.d.get(), s2.d.get(), min(s1Len, s2Len));
		if (res == 0) res = s1Len-s2Len;
		return res;
	}

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
				shared_ptr<UnitT>(other.readAliasDummy, NULL)) // We don't care about the actual pointer in readAliasDummy
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




#endif /* ABSTRACTSHAREDSTRING_H_ */
