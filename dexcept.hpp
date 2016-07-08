#ifndef XIF_UTIL_EXCEPT
#define XIF_UTIL_EXCEPT

#include <exception>
#include <string>
#include <string.h>

namespace xif {

class dexcept : public std::exception {
protected:
	virtual std::string descr () const = 0; // Message-generating method, to be overloaded
	char** const _buf;
	const char* const _str;
public:
	explicit dexcept () noexcept : _buf(new char*(nullptr)), _str(nullptr) {} // what() message will be generated on the fly by descr()
	explicit dexcept (const char* str) noexcept : _buf(nullptr), _str(str) {} // For static, always valid c-strings only
	dexcept (const dexcept& o) : _buf(new char*(nullptr)), _str(o._str) {}
	dexcept (dexcept&& o) : _buf(o._buf), _str(o._str) {}
	virtual const char* what () const noexcept {
		if (_buf == nullptr) return _str;
		if (*_buf != nullptr) delete [] *_buf;
		try {
			std::string str = this->descr();
			*_buf = new char[str.length()+1];
			::memcpy(*_buf, str.c_str(), str.length()+1);
		} catch (...) {
			return NULL;
		}
		return *_buf; // Following the std::exception specs, the c-string will be valid until exception deallocation
	}
	virtual ~dexcept() noexcept {
		if (_buf != nullptr) {
			if (*_buf != nullptr) delete [] *_buf;
			delete _buf;
		}
	}
};

}

#endif
