#ifndef XIF_UTILS_MULT_UNITS_H
#define XIF_UTILS_MULT_UNITS_H

#include <string>
#include <sstream>
#include <iomanip>
#include <tuple>
#include <cmath>

namespace xif {

	enum mult_t               { MULT_SCI, MULT_Femto, MULT_Pico, MULT_Nano, MULT_Micro, MULT_Mili, MULT_0, MULT_Kilo, MULT_Mega, MULT_Giga, MULT_Tera };
	const char* mult_strs[] = { ""      , "f"       , "p"      , "n"      , "µ"       , "m"      , ""    , "k"      , "M"      , "G"      , "T"       };

	inline std::pair<double,mult_t> mult_autovalue (double v, double fact = 1e3) {
		double x = v;
		mult_t m = MULT_0;
		while (fabs(x) > fact) {
			if (m == MULT_Tera) { return {v, MULT_SCI}; }
			x /= fact;
			m = (mult_t)((int)m+1);
		}
		while (fabs(x) < 1.) {
			if (m == MULT_Femto) { return {v, MULT_SCI}; }
			x *= fact;
			m = (mult_t)((int)m-1);
		}
		return {x, m};
	}
	
	inline std::string ftoa_h (double x, size_t p, size_t maxd = 5) {
		std::ostringstream s;
		size_t d = lround(floor(fabs(log10(fabs(x)))));
		if (x < 1) {
			if (d+p-1 >= maxd) s << std::scientific << std::setprecision((uint)(p-1));
			else s << std::fixed << std::setprecision((uint)(d+p));
		} else {
			if (d >= p and x != 0.) s << std::scientific << std::setprecision((uint)(p-1));
			else s << std::fixed << std::setprecision((uint)(p-d-1));
		}
		s << x;
		return s.str();
	}
	
	inline std::string ftoa_f (double x, size_t p) {
		std::ostringstream s;
		ssize_t d = lround(ceil(log10(fabs(x))));
		s << std::fixed << std::setprecision((uint)std::max<ssize_t>(p-d,0)) << x;
		return s.str();
	}
	
	inline std::string mult_fmt (double x, const char* unit, uint8_t prec, int8_t sz_pad = 0, double fact = 1e3) {
		mult_t m;
		std::tie(x,m) = mult_autovalue(x, fact);
		std::ostringstream s;
		if (m != MULT_SCI) s << std::fixed;
		s << std::setprecision(prec) << x << ' ' << mult_strs[m] << unit;
		std::string str = s.str();
		if (sz_pad != 0) {
			uint8_t npad = abs(sz_pad) - str.length();
			for (uint8_t i = 0; i < npad; i++)
				str.insert((sz_pad<0 ? str.begin() : str.end()), ' ');
		}
		return str;
	}

}

#endif