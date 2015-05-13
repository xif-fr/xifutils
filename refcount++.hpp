/**********************************************************************
 * Xif Refcount++ Library
 * For bug reports, help, or improve requests, please mail at Félix Faisant <xcodexif@xif.fr>
 * Under Public Domain
 **********************************************************************
 * Refcount++ is a library for helping reference counting in C++ objects
 * Suitable for RAII or warpers
 * Derivate from refcountxx_base for turning your class into reference counted class
 * If you will subclass your reference counted class and if theses sublasses
 *  will need to be reference counted too (dyn allocs, ressources to close) 
 *  in addition to your reference counted base class, consider using REFCXX_* macros
 **********************************************************************/

#ifndef XIF_REFCOUNTXX_LIB
#define XIF_REFCOUNTXX_LIB

#include <inttypes.h>
#include <stddef.h>
#include <memory>

	/// Derivate from refcountxx_base only if your class will be not subclassed
	///  with refcount requirement (additionals ressouces or pointers to care about...)
class refcountxx_base {
protected:
	uint32_t* const _refcxx_refcount;
	bool can_destruct () const { return (*_refcxx_refcount)-1 == 0; } // Test it in your destructor. If true, you can destruct your object safely
public:
	refcountxx_base () : _refcxx_refcount(new uint32_t(1)) {}
	refcountxx_base (const refcountxx_base& o) : _refcxx_refcount(&(++*o._refcxx_refcount)) {}
	~refcountxx_base () { if (--*_refcxx_refcount == 0) { delete _refcxx_refcount; } }
};

	/// REFCXX_* macros to be used in classes
	/// Put REFCXX_REFCOUNTED in private: section
	/// Put REFCXX_CONSTRUCTOR and REFCXX_COPY_CONSTRUCTOR into initialization list
	/// Use REFCXX_DESTRUCT in the destructor like `REFCXX_DESTRUCT(my_file_warper) { fclose(my_file); }`. Use REFCXX_WILL_DESTRUCT _only_ in a refcounted class child's destructor
#define _REFCXX_RCVAR(classname) _refcxx_##classname##_refcount
#define REFCXX_REFCOUNTED(classname) uint32_t* _REFCXX_RCVAR(classname)
#define REFCXX_CONSTRUCTOR(classname) _REFCXX_RCVAR(classname)(new uint32_t(1))
#define REFCXX_COPY_CONSTRUCTOR(classname, obj_to_copy) _REFCXX_RCVAR(classname)(&(++*obj_to_copy._REFCXX_RCVAR(classname)))
#define REFCXX_DESTRUCT(classname) if (--*_REFCXX_RCVAR(classname) == 0) { delete _REFCXX_RCVAR(classname); _REFCXX_RCVAR(classname) = NULL; } if (!_REFCXX_RCVAR(classname)) 
#define REFCXX_WILL_DESTRUCT(classname) if (*_REFCXX_RCVAR(classname)-1 == 0) 
#define REFCXX_NO_ASSIGN(classname) classname& operator= (const classname&) = delete
#define REFCXX_ASSIGN(classname) classname& operator= (const classname& to_assign) { this->~classname(); new(this) classname(to_assign); return *this; }

#endif
