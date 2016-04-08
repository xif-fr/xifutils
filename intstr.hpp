#ifndef XIF_UTILS_INTSTR_H
#define XIF_UTILS_INTSTR_H

	/// Ints <> string conversions
#include <type_traits>
#include <limits>
#include <string>
#include <algorithm>
#include <stdexcept>

#define IX_HEX 16
#define IX_HEX_MAJ 160
#define IX_DEC 10
#define IX_OCT 8
#define IX_BIN 2

template <typename int_t, 
          typename = typename std::enable_if<std::is_signed<int_t>::value>::type>
inline bool _ixisnegative (int_t n) {
	return (n < 0);
}
template <typename int_t, 
          typename = void,
          typename = typename std::enable_if<not std::is_signed<int_t>::value>::type>
inline bool _ixisnegative (int_t n) {
	return false;
}

template <typename int_t, 
          typename = typename std::enable_if<std::is_integral<int_t>::value>::type>
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
          class = typename std::enable_if<std::is_pointer<ptr_t>::value>::type>
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
	if (str.empty()) throw std::runtime_error("atoix : empty string");
	if (base < 2 or base > 16) throw std::range_error("atoix : base must be between 2 and 16");
	static const char* _ix_chars = "0123456789abcdef";
	if (str.length() > 2 and str.substr(0,2) == "0x") { if (base == 0) base = IX_HEX; if (base == IX_HEX) str.erase(0,2); }
	if (str.length() > 2 and str.substr(0,2) == "0b") { if (base == 0) base = IX_BIN; if (base == IX_BIN) str.erase(0,2); }
	if (base == 0 and str.length() > 1 and str[0] == '0') base = IX_OCT;
	if (base == 0) base = IX_DEC;
	uintmax_t n = 0, t, b = 1;
	for (int32_t i = (int32_t)str.length()-1; i != -1; i--) {
		for (uintmax_t ic = 0; ic < base; ++ic) {
			if (_ix_chars[ic] == ::tolower(str[(size_t)i])) {
				t = ic*b;
				goto calculus;
			}
		}
		throw std::runtime_error(std::string("atoix : character '")+str[(size_t)i]+"' not valid for this base");
	calculus:
		n += t;
		if (n > std::numeric_limits<int_t>::max())
			throw std::runtime_error("atoix : overflow");
		b *= base;
	}
	return (int_t)n;
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

#endif