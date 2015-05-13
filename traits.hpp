#ifndef XIF_TRAITS_CXX_H
#define XIF_TRAITS_CXX_H

	/// Type traits & cie.

#ifdef XIF_NO_CXX11_STL
	#include <boost/utility/enable_if.hpp>
	#include <boost/type_traits.hpp>
	#define _enable_if_ boost::enable_if_c
	namespace std {
		using namespace boost::traits;
	}
#else
	#include <type_traits>
	#include <limits>
	#define _enable_if_ std::enable_if
#endif

namespace std {
	template <bool B, typename T = void>
	struct disable_if {
		typedef T type;
	};
	template <typename T>
	struct disable_if<true,T> {
	};
}

#define _disable_if_ std::disable_if

#endif
