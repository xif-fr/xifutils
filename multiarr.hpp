#ifndef XIF_UTILS_MULTIARR_H
#define XIF_UTILS_MULTIARR_H

#include <functional>

namespace xif {
	
	namespace _multiarr_det {
		template <size_t dim>
		struct mdimarr {
			size_t d[dim-1];
			mdimarr (std::function<size_t(size_t)> f_dim) {
				d[0] = f_dim(dim-1);
				for (size_t i = 2; i < dim; ++i)
					d[i-1] = d[i-2] * f_dim(dim-i);
			}
		};
		template <>
		struct mdimarr<1> {
			mdimarr (std::function<size_t(size_t)> f_dim) {}
		};
		template <typename T, size_t dim, size_t k>
		struct acc {
			const mdimarr<dim>& dims; T* ptr;
			acc<T,dim, k-1> operator[] (size_t i) { return acc<T,dim, k-1>({ dims, .ptr = ptr + i * dims.d[k-2] }); }
		};
		template <typename T, size_t dim>
		struct acc<T,dim, 1> {
			const mdimarr<dim>& dims; T* ptr;
			T& operator[] (size_t i) { return ptr[i]; }
		};
	}
	
	template <typename T, size_t dim>
	struct multiarr {
		T* ptr;
		const _multiarr_det::mdimarr<dim> dims;
		multiarr (T* raw, const _multiarr_det::mdimarr<dim> dims) : ptr(raw), dims(dims) {}
		multiarr (T* raw, std::function<size_t(size_t)> f_dim) : multiarr(raw, _multiarr_det::mdimarr<dim>(f_dim)) {}
		auto operator[] (size_t i) { return _multiarr_det::acc<T,dim, dim>({ dims, ptr }) [i]; }
	};
	
	template <typename T>
	struct multiarr<T, 1> {
		T* ptr;
		multiarr (T* raw) : ptr(raw) {}
		multiarr (T* raw, std::function<size_t(size_t)> f_dim) : multiarr(raw) {}
		T& operator[] (size_t i) { return ptr[i]; }
	};

}

#endif