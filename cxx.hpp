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
	
	inline void str_remove_all (std::string& str, const std::string& needle) {
		if (needle.empty()) return;
		size_t pos = 0;
		while ((pos = str.find(needle, pos)) != std::string::npos)
			str.erase(pos, needle.length());
	}
	
	inline void str_replace_all (std::string& str, std::vector<std::pair<std::string,std::string>> replace_array) {
		for (std::pair<std::string,std::string> replace_pair : replace_array) {
			std::str_replace_all(str, replace_pair.first, replace_pair.second);
		}
	}
	
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
