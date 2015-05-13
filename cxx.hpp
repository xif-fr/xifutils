#ifndef XIF_UTILS_CXX_H
#define XIF_UTILS_CXX_H

	/// Foo
#include <sys/types.h>

	/// RAII
#include <functional>

struct _raii_at_end {
	std::function<void()> f;
	_raii_at_end (decltype(f) func_at_end) : f(func_at_end) {}
	~_raii_at_end () { f(); }
};
#define RAII_AT_END(func_at_end) _raii_at_end __raii_at_end([&]()func_at_end)
#define RAII_AT_END_L(instr_at_end) _raii_at_end __raii_at_end([&](){instr_at_end;})
#define RAII_AT_END_N(name,func_at_end) _raii_at_end __raii_at_end_##name([&]()func_at_end)
#define RAII_AT_END_CN(name,func_at_end) _raii_at_end __raii_at_end_##name([=]()func_at_end)

	/// String helpers
#include <string>
#include <string.h>
#include <vector>

inline std::string operator "" _s (const char* str, size_t) { return std::string(str); }

inline char _s_add (char arg) { return arg; }
template <typename T> inline std::string _s_add (T arg) { return std::string(arg); }
template <typename T, typename... Args>
inline std::string _s_add (T arg, Args... args) {
	return arg + _s_add(args...);
}
template <typename... Args>
inline std::string _s_add (std::string& str, Args... args) {
	return str + _s_add(args...);
}

#define _s(...) _s_add(__VA_ARGS__).c_str()
#define _S(...) _s_add(__VA_ARGS__)

namespace std {
	
	inline void str_replace_all (std::string& str, const std::string& needle, const std::string& by) {
		if (needle.empty()) return;
		size_t pos = 0;
		while ((pos = str.find(needle, pos)) != std::string::npos) {
			str.replace(pos, needle.length(), by);
			pos += by.length();
		}
	}
	
	inline void str_replace_all (std::string& str, std::vector<std::pair<std::string,std::string>> replace_array) {
		for (std::pair<std::string,std::string> replace_pair : replace_array) {
			std::str_replace_all(str, replace_pair.first, replace_pair.second);
		}
	}
	
}

	/// Ints <> string conversions
#include <limits>
#include <algorithm>
#include <stdexcept>
#include <xifutils/traits.hpp>

#define IX_HEX 16
#define IX_HEX_MAJ 160
#define IX_DEC 10
#define IX_OCT 8
#define IX_BIN 2

template <typename int_t, 
          class = typename _enable_if_<std::is_signed<int_t>::value>::type>
inline bool _ixisnegative (int_t n) {
	return (n < 0);
}
template <typename int_t, 
          class = void,
          class = typename _disable_if_<std::is_signed<int_t>::value>::type>
inline bool _ixisnegative (int_t n) {
	return false;
}

template <typename int_t, 
          class = typename _enable_if_<std::is_integral<int_t>::value>::type>
inline std::string ixtoa (int_t n, uint8_t base = IX_DEC) {
	const char* _ix_chars = "0123456789abcdef";
	if (base == IX_HEX_MAJ) {
		_ix_chars = "0123456789ABCDEF";
		base = IX_HEX;
	}
	if (base < 2 or base > 16) throw std::range_error("ixtoa() : base must be between 2 and 16");
	if (n == 0) return "0";
	std::string buffer;
	bool is_neg = false;
	if (::_ixisnegative(n)) { is_neg = true; n = -n; }
	while (n > 0) {
		buffer += _ix_chars[n%base];
		n /= base;
	}
	if (is_neg) buffer += '-';
	std::reverse(buffer.begin(), buffer.end());
	return buffer;
}
template <typename ptr_t, 
          class = typename _enable_if_<std::is_pointer<ptr_t>::value>::type>
inline std::string ixtoa (ptr_t p) {
	return ::ixtoa((uintmax_t)p, IX_HEX);
}
template <typename int_t>
inline std::string ixtoap (int_t n, size_t padding, uint8_t base = IX_DEC) {
	std::string str = ::ixtoa<int_t>(n, base);
	if (padding == 0 && base == IX_HEX) 
		padding = sizeof(int_t)*2;
	if (str.length() < padding) 
		str.insert(str.begin(), padding-str.length(), '0');
	return str;
}

template <typename int_t>
inline int_t atoix (std::string str, uint8_t base = IX_DEC) {
	if (str.empty()) throw std::runtime_error("atoix() : Empty string");
	if (base < 2 or base > 16) throw std::range_error("atoix() : base must be between 2 and 16");
	static const char* _ix_chars = "0123456789abcdef";
	if (str.length() > 2 and str.substr(0,2) == "0x") { if (base == 0) base = IX_HEX; if (base == IX_HEX) str.erase(0,2); }
	if (str.length() > 2 and str.substr(0,2) == "0b") { if (base == 0) base = IX_BIN; if (base == IX_BIN) str.erase(0,2); }
	if (base == 0 and str.length() > 1 and str[0] == '0') base = IX_OCT;
	if (base == 0) base = IX_DEC;
	int_t n = 0, t, b = 1;
	for (ssize_t i = (ssize_t)str.length()-1; i != -1; i--) {
		for (size_t ic = 0; ic < base; ++ic) {
			if (_ix_chars[ic] == ::tolower(str[(size_t)i])) {
				t = (int_t)ic*b;
				goto calculus; }
		}
		throw std::runtime_error(_s_add("atoix() : '\\x",::ixtoa(str[(size_t)i],IX_HEX),"' char not valid for this base"));
	calculus:
		n += t;
		b *= base;
	}
	return n;
}

template <typename int_t>
inline int_t atoixs (std::string str, uint8_t base = 10) {
	signed mult = 1;
	if (str.length() > 1 and (str[0] == '+' or str[0] == '-')) {
		mult = (str[0] == '-') ? -1 : 1;
		str.erase(0,1);
	}
	return atoix<int_t>(str)*mult;
}

	/// Errors
#include <errno.h>
#include <stdexcept>

class errno_autoreset {
public: errno_autoreset () {}
	~errno_autoreset () { errno = 0; }
};
#define errno_autoreset_handle() errno_autoreset _errno_autoreset_handle

namespace xif {
	class sys_error : public std::runtime_error {
	public:
		int errorno;
		sys_error (std::string what, int r = 0) noexcept : std::runtime_error(what+" : "+::strerror((r)?r:errno)), errorno((r)?r:errno) { errno = 0; }
		sys_error (std::string what, const char* descr) noexcept : std::runtime_error(what+" : "+descr), errorno(0) {}
		sys_error (std::string what, std::string descr) noexcept : std::runtime_error(what+" : "+descr), errorno(0) {}
	};
}

	// Stack dump
#if DEBUG
#include <execinfo.h>
namespace xif {
	inline void stack_dump () {
		void* symbols[100];
		size_t sz = ::backtrace(symbols, sizeof(symbols)/(sizeof(void*)));
		char** strs = ::backtrace_symbols(symbols, (int)sz);
		::printf("--- %zd Stack frames\n", sz);
		for (size_t i = 0; i < sz; i++)
			::puts(strs[i]);
		::free(strs);
	}
}
#endif

#endif
