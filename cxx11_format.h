#ifndef XIF_CXX11_FORMAT_H
#define XIF_CXX11_FORMAT_H

#include <ostream>
#include <sstream>
#include <stdexcept>

template <typename Stream>
inline Stream& cxx11_format (Stream& write_stream, const char* format) {
	while (*format) {
		if (*format == '%') {
			if (*++format == '%') {
				write_stream << '%';
				++format;
				continue;
			}
			throw std::logic_error("cxx11_format: missing arguments");
		}
		write_stream << *format++;
	}
	return write_stream;
}

template <typename Stream, typename Arg, typename... Args>
inline Stream& cxx11_format (Stream& write_stream, const char* format, Arg arg, Args... args) {
	while (*format) {
		if (*format == '%') {
			++format;
			if (*format == '%') {
				write_stream << '%';
				++format;
				continue;
			}
			write_stream << arg;
			cxx11_format(write_stream, format, args...);
			write_stream.flush();
			return write_stream;
		}
		write_stream << *format++;
	}
	throw std::logic_error("cxx11_format: extra arguments provided");
}

template <typename... Args>
inline std::string cxx11_format_s (const char* format, Args... args) {
	std::ostringstream stream;
	cxx11_format(stream, format, args...);
	return stream.str();
}

#endif
