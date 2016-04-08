#ifndef XIF_UTILS_POLYVAR_H
#define XIF_UTILS_POLYVAR_H

#include <vector>
#include <map>
#include <string>
#include <inttypes.h>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <type_traits>
#include <limits>

namespace xif {
	
	namespace util {
		inline void str_escape_print (std::ostream& s, const std::string& str) {
			for (char c : str) {
				if (c == '\\') s << "\\\\"; else if (c == '\n') s << "\\n"; else if (c == '\b') s << "\\b"; else if (c == '\f') s << "\\f";
				else if (c == '\r') s << "\\r"; else if (c == '\t') s << "\\t"; else if (c == '"') s << "\\\""; else s << c;
			}
		}
	}
	
	class polyvar {
	public: 
		enum type { VOID, STR, FLOAT, INT, CHAR, BOOL, LIST, MAP };
		typedef std::map<std::string,xif::polyvar> map;
		typedef std::vector<xif::polyvar> vec;
		
	private:
		polyvar::type _type;
		void* _data;
		
	public:
		
			/// Public constructors from variables of apropriate type
		polyvar ()                                       : _type(VOID),  _data(NULL) {}
		polyvar (const std::string& s)                   : _type(STR),   _data(new std::string (s)) {}
		polyvar (const char* s)                          : _type(STR),   _data(new std::string (s)) {}
		template <typename float_t, 
		          typename std::enable_if<std::is_floating_point<float_t>::value>::type* = nullptr>
		polyvar (float_t f)                              : _type(FLOAT), _data(new double ((double)f)) {}
		template <typename int_t, 
		          typename std::enable_if<std::is_integral<int_t>::value and sizeof(int_t)!=1>::type* = nullptr>
		polyvar (int_t i)                                : _type(INT),   _data(new intmax_t ((intmax_t)i)) {}
		polyvar (char c)                                 : _type(CHAR),  _data(new char (c)) {}
		polyvar (bool b)                                 : _type(BOOL),  _data(new bool (b)) {}
		polyvar (const std::vector<polyvar>& v)          : _type(LIST),  _data(new std::vector<polyvar> (v)) {}
		polyvar (const std::map<std::string,polyvar> m)  : _type(MAP),   _data(new std::map<std::string,polyvar> (m)) {}
		
			/// Accessors
		class bad_type : public std::runtime_error {                   // thrown if requested type does not corresponds with actual type
			public: bad_type() : std::runtime_error("polyvar : bad type") {} 
		};
		type type () const { return this->_type; }
		
		                  const std::string& s () const { if (_type != STR)   throw bad_type();   return *(std::string*)_data; }
		                        std::string& s ()       { if (_type != STR)   throw bad_type();   return *(std::string*)_data; }
		operator std::string ()                   const { return this->s(); }
		
		                              double f () const { if (_type != FLOAT) throw bad_type();   return *(double*)_data; }
		                             double& f ()       { if (_type != FLOAT) throw bad_type();   return *(double*)_data; }
		template <typename float_t, 
		          typename std::enable_if<std::is_floating_point<float_t>::value>::type* = nullptr>
		operator float_t ()                       const { return (float_t)this->f(); }
		
		                            intmax_t i () const { if (_type != INT)   throw bad_type();   return *(intmax_t*)_data; }
		                           intmax_t& i ()       { if (_type != INT)   throw bad_type();   return *(intmax_t*)_data; }
		template <typename int_t, 
		          typename std::enable_if<std::is_integral<int_t>::value and sizeof(int_t)!=1>::type* = nullptr>
		operator int_t ()                         const { if (this->i() < std::numeric_limits<int_t>::min() or this->i() > std::numeric_limits<int_t>::max()) throw std::overflow_error("destination cast can't handle number");
		                                                  return (int_t)this->i(); }
		
		                                char c () const { if (_type != CHAR)  throw bad_type();   return *(char*)_data; }
		                               char& c ()       { if (_type != CHAR)  throw bad_type();   return *(char*)_data; }
		template <typename = char>
		operator char ()                          const { return this->c(); }
		
		                                bool b () const { if (_type != BOOL)  throw bad_type();   return *(bool*)_data; }
		                               bool& b ()       { if (_type != BOOL)  throw bad_type();   return *(bool*)_data; }
		template <typename = bool>
		operator bool ()                          const { return this->b(); }
		
		         const std::vector<polyvar>& v () const { if (_type != LIST)  throw bad_type();   return *(const std::vector<polyvar>*)_data; }
		               std::vector<polyvar>& v ()       { if (_type != LIST)  throw bad_type();   return *(std::vector<polyvar>*)_data; }
		polyvar& operator[] (size_t i)            const { return ((std::vector<polyvar>&)this->v()).at(i); }
		void push_back (const xif::polyvar& p)          { ((std::vector<polyvar>&)this->v()).push_back(p); }
		
		const std::map<std::string,polyvar>& m () const { if (_type != MAP)   throw bad_type();   return *(std::map<std::string,polyvar>*)_data; }
		std::map<std::string,polyvar>& m ()             { if (_type != MAP)   throw bad_type();   return *(std::map<std::string,polyvar>*)_data; }
		polyvar& operator[] (const char* i)       const { return ((std::map<std::string,polyvar>&)this->m())[i]; }
		polyvar& operator[] (std::string i)       const { return ((std::map<std::string,polyvar>&)this->m())[i]; }
		
			/// Utilities
		
		std::string to_json (uint8_t float_precision = 10) {
			std::stringstream s;
			s << std::setprecision(float_precision) << std::fixed;
			switch (this->_type) {
				case VOID:  s << "null";                               break;
				case STR:   s << '"'; xif::util::str_escape_print(s, *(std::string*)_data); s << '"';   break;
				case FLOAT: s << *(double*)_data;                      break;
				case INT:   s << *(intmax_t*)_data;                    break;
				case CHAR:  s << *(char*)_data;                        break;
				case BOOL:  s << ((*(bool*)_data) ? "true" : "false"); break;
				case LIST: {
					std::vector<xif::polyvar>& v = *(std::vector<polyvar>*)_data;
					s << '[';
					for (size_t i = 0; i < v.size(); i++) {
						s << v[i].to_json(float_precision);
						if (i != v.size()-1) s << ',';
					}
					s << ']';
				} break;
				case polyvar::MAP: {
					std::map<std::string,xif::polyvar>& m = *(std::map<std::string,polyvar>*)_data;
					s << '{';
					for (auto it = m.begin(); it != m.end(); it++) {
						if (it != m.begin()) s << ',';
						s << '"'; xif::util::str_escape_print(s, it->first); s << "\":";
						s << it->second.to_json(float_precision);
					}
					s << '}';
				} break;
			}
			return s.str();
		}
		
			/// Move / Assignment / Copy / Destructor
		polyvar (polyvar&& p) : _type(p._type), _data(p._data) { p._type = VOID; p._data = NULL; }
		polyvar& operator= (polyvar p) { std::swap(_type, p._type); std::swap(_data, p._data); return *this; }
		polyvar (const polyvar& p) : _type(p._type), _data(NULL) {
			switch (_type) {
				case VOID:  _data = NULL;                                                                         break;
				case STR:   _data = new std::string                   (*(std::string*)p._data);                   break;
				case FLOAT: _data = new double                        (*(double*)p._data);                        break;
				case INT:   _data = new intmax_t                      (*(intmax_t*)p._data);                      break;
				case CHAR:  _data = new char                          (*(char*)p._data);                          break;
				case BOOL:  _data = new bool                          (*(bool*)p._data);                          break;
				case LIST:  _data = new std::vector<polyvar>          (*(std::vector<polyvar>*)p._data);          break;
				case MAP:   _data = new std::map<std::string,polyvar> (*(std::map<std::string,polyvar>*)p._data); break;
			}
		}
		~polyvar () {
			if (this->_data == NULL) return;
			switch (_type) {
				case VOID:                                                break;
				case STR:   delete (std::string*)_data;                   break;
				case FLOAT: delete (double*)_data;                        break;
				case INT:   delete (intmax_t*)_data;                      break;
				case CHAR:  delete (char*)_data;                          break;
				case BOOL:  delete (bool*)_data;                          break;
				case LIST:  delete (std::vector<polyvar>*)_data;          break;
				case MAP:   delete (std::map<std::string,polyvar>*)_data; break;
			}
			_data = NULL;
		}
		
	};

}

#endif
