#ifndef MSGPACK_HPP
#define MSGPACK_HPP

#include <type_traits>
#include <iostream>
#include <numeric>
#include <vector>
#include <tuple>
#include <string>
#include <map>

#include "containers/byte.hpp"
#include "formats.hpp"

namespace msgpack {

	// some definitions

	template<typename T>
	void pack(std::vector<T> src, byte::container& dest, bool initial = true);
	template<typename ...T>
	void pack(std::tuple<T...> src, byte::container& dest, bool initial = true);
	template<typename T, typename S>
	void pack(std::map<T, S> src, byte::container& dest, bool initial = true);

	// utility

	template <typename T, typename S>
	struct is_string {
		static const bool value = false;
	};

	template <class T, class Traits, class Alloc>
	struct is_string<T, std::basic_string<T, Traits, Alloc> > {
		static const bool value = true;
	};

	template<int N, typename... Ts> using NthTypeOf =
		typename std::tuple_element<N, std::tuple<Ts...>>::type;

	template <size_t I = 0, typename... Ts>
	typename std::enable_if<I == sizeof...(Ts), size_t>::type
		recursive_size(std::tuple<Ts...> tup) {
		return 0;
	}

	template <size_t I = 0, typename... Ts>
	typename  std::enable_if<(I < sizeof...(Ts)), size_t>::type
		recursive_size(std::tuple<Ts...> tup) {
		// std::cout << __PRETTY_FUNCTION__ << std::endl;
		size_t result = 0;
		// is_string<std::string, NthTypeOf<I, Ts...> >::value
		// std::is_same<std::string, NthTypeOf<I, Ts...> >::value
		// if (std::is_integral<NthTypeOf<I, Ts...> >::value || std::is_floating_point<NthTypeOf<I, Ts...> >::value) {
			result += sizeof(NthTypeOf<I, Ts...>);
		// }
		/*else if (std::is_same<std::basic_string<char, struct std::char_traits<char>, class std::allocator<char> >, NthTypeOf<I, Ts...> >::value) {
			typename std::tuple_element<I, std::tuple<Ts...> >::type test = std::get<I>(tup);
			std::stringstream ss(test);
			result += static_cast<size_t>(ss.str().length());
		}
		else {
			std::cout << typeid(std::get<I>(tup)).name() << std::endl;
		}*/
		return result + recursive_size<I + 1>(tup);
	}

	// packing functions - primitive

	// string

	void pack(const char src, byte::container& dest, bool initial = false) {
		dest.push_back(uint8_t(single_char));
		dest.push_back(src);
	}
	void pack(const char* src, size_t len, byte::container& dest, bool initial = false) {
		uint32_t n = static_cast<uint32_t>(len);
		if (n <= fix32) {
			dest.push_back(uint8_t(fixstr_t(n)));
		}
		else if (n <= umax8) {
			dest.push_back(uint8_t(str8));
			dest.push_back(uint8_t(n));
		}
		else if (n <= umax16) {
			dest.push_back(uint8_t(str16));
			dest.push_back(uint16_t(n));
		}
		else if (n <= umax32) {
			dest.push_back(uint8_t(str32));
			dest.push_back(uint32_t(n));
		}
		else {
			throw std::range_error(std::to_string(n) + " out of range!");
		}
		dest.push_back(src, n);
	}
	void pack(char* src, size_t len, byte::container& dest, bool initial = false) {
		uint32_t n = static_cast<uint32_t>(len);
		if (n <= fix32) {
			dest.push_back(uint8_t(fixstr_t(n)));
		}
		else if (n <= umax8) {
			dest.push_back(uint8_t(str8));
			dest.push_back(uint8_t(n));
		}
		else if (n <= umax16) {
			dest.push_back(uint8_t(str16));
			dest.push_back(uint16_t(n));
		}
		else if (n <= umax32) {
			dest.push_back(uint8_t(str32));
			dest.push_back(uint32_t(n));
		}
		else {
			throw std::range_error(std::to_string(n) + " out of range!");
		}
		dest.push_back(src, n);
	}
	void pack(std::string src, byte::container& dest, bool initial = false) {
		uint32_t len = static_cast<uint32_t>(src.length());
		if (len <= fix32) {
			dest.push_back(uint8_t(fixstr_t(len)));
		}
		else if (len <= umax8) {
			dest.push_back(uint8_t(str8));
			dest.push_back(uint8_t(len));
		}
		else if (len <= umax16) {
			dest.push_back(uint8_t(str16));
			dest.push_back(uint16_t(len));
		}
		else if (len <= umax32) {
			dest.push_back(uint8_t(str32));
			dest.push_back(uint32_t(len));
		}
		else {
			throw std::range_error(std::to_string(len) + " out of range!");
		}
		dest.push_back(src);
	}

	// integers

	// unsigned int

	void pack_uint(uint64_t src, byte::container& dest, bool initial = false) {
		if (src <= umaxfixint) {
			dest.push_back(uint8_t(ufixint_t<uint64_t>(src)));
		}
		else if (src <= umax8) {
			dest.push_back(uint8_t(uint8));
			dest.push_back(uint8_t(src));
		}
		else if (src <= umax16) {
			dest.push_back(uint8_t(uint16));
			dest.push_back(uint16_t(src));
		}
		else if (src <= umax32) {
			dest.push_back(uint8_t(uint32));
			dest.push_back(uint32_t(src));
		}
		else if (src <= umax64) {
			dest.push_back(uint8_t(uint64));
			dest.push_back(uint64_t(src));
		}
		else {
			throw std::range_error(std::to_string(src) + " out of range!");
		}
	}

	// signed int

	void pack_int(int64_t src, byte::container& dest, bool initial = false) {
		uint64_t cmp = src;
		if (int64_t(cmp) < 0) {
			cmp = int64_t(cmp) * -1;
		}
		if (cmp <= maxfixint) {
			dest.push_back(uint8_t(fixint_t<int64_t>(cmp)));
		}
		else if (cmp <= max8) {
			dest.push_back(uint8_t(int8));
			dest.push_back(uint8_t(src));
		}
		else if (cmp <= max16) {
			dest.push_back(uint8_t(int16));
			dest.push_back(uint16_t(src));
		}
		else if (cmp <= max32) {
			dest.push_back(uint8_t(int32));
			dest.push_back(uint32_t(src));
		}
		else if (cmp <= max64) {
			dest.push_back(uint8_t(int64));
			dest.push_back(uint64_t(src));
		}
		else {
			throw std::range_error(std::to_string(src) + " out of range!");
		}
	}

	void pack(uint8_t src, byte::container& dest, bool initial = false) {
		pack_uint(static_cast<uint64_t>(src), dest);
	}
	void pack(uint16_t src, byte::container& dest, bool initial = false) {
		pack_uint(static_cast<uint64_t>(src), dest);
	}
	void pack(uint32_t src, byte::container& dest, bool initial = false) {
		pack_uint(static_cast<uint64_t>(src), dest);
	}
	void pack(uint64_t src, byte::container& dest, bool initial = false) {
		pack_uint(static_cast<uint64_t>(src), dest);
	}
	void pack(int8_t src, byte::container& dest, bool initial = false) {
		pack_int(static_cast<int64_t>(src), dest);
	}
	void pack(int16_t src, byte::container& dest, bool initial = false) {
		pack_int(static_cast<int64_t>(src), dest);
	}
	void pack(int32_t src, byte::container& dest, bool initial = false) {
		pack_int(static_cast<int64_t>(src), dest);
	}
	void pack(int64_t src, byte::container& dest, bool initial = false) {
		pack_int(src, dest);
	}

	// double

	void pack(double src, byte::container& dest, bool initial = false) {
		float src_as_float = float(src);
		double src_back_to_double = double(src_as_float);
		if (src_back_to_double == src) {
			// is float
			dest.push_back(uint8_t(float32));
			dest.push_back(src_as_float);
		}
		else {
			// is double
			dest.push_back(uint8_t(float64));
			dest.push_back(src);
		}
	}

	// float

	void pack(float src, byte::container& dest, bool initial = false) {
		dest.push_back(uint8_t(float32));
		dest.push_back(src);
	}

	// boolean

	void pack(bool src, byte::container& dest, bool initial = false) {
		if (src) {
			dest.push_back(uint8_t(tru));
		}
		else {
			dest.push_back(uint8_t(flse));
		}
	}

	// nil

	void pack(byte::container& dest, bool initial = false) {
		dest.push_back(uint8_t(nil));
	}

	// stl iterators

	template <size_t I = 0, typename... Ts>
	typename std::enable_if<I == sizeof...(Ts), void>::type
		iterate_tuple(std::tuple<Ts...> tup, byte::container& dest) {
		return;
	}

	template <size_t I = 0, typename... Ts>
	typename  std::enable_if<(I < sizeof...(Ts)), void>::type
		iterate_tuple(std::tuple<Ts...> tup, byte::container& dest) {
		if (std::is_integral<NthTypeOf<I, Ts...> >::value || std::is_same<std::string, NthTypeOf<I, Ts...> >::value) {
			pack(std::get<I>(tup), dest);
		}
		else {
			pack(std::get<I>(tup), dest, false);
		}
		iterate_tuple<I + 1>(tup, dest);
	}

	// packing functions - STL

	template <typename T>
	void pack(std::vector<T> src, byte::container& dest, bool initial) {
		size_t n = src.size();
		if (initial) {
			dest.check_resize(size_t(sizeof(src) * compression_percent));
		}
		if (n <= 15) {
			dest.push_back(fixarray_t(n));
		}
		else if (n <= umax16) {
			dest.push_back(uint8_t(arr16));
			dest.push_back(uint16_t(n));
		}
		else if (n <= umax32) {
			dest.push_back(uint8_t(arr32));
			dest.push_back(uint32_t(n));
		}
		for (uint32_t i = 0; i < n; i++) {
			pack(src[i], dest);
		}
		if (initial) {
			dest.free_empty();
		}
	}

	template<typename ...T>
	void pack(std::tuple<T...> src, byte::container& dest, bool initial) {
		size_t n = std::tuple_size<decltype(src)>::value;
		if (initial) {
			// std::cout << recursive_size(src) << "\t" << size_t(recursive_size(src) * compression_percent) << std::endl;
			dest.check_resize(size_t(recursive_size(src) * compression_percent));
		}
		if (n <= 15) {
			dest.push_back(fixarray_t(n));
		}
		else if (n <= umax16) {
			dest.push_back(uint8_t(arr16));
			dest.push_back(uint16_t(n));
		}
		else if (n <= umax32) {
			dest.push_back(uint8_t(arr32));
			dest.push_back(uint32_t(n));
		}
		iterate_tuple(src, dest);
		if (initial) {
			dest.free_empty();
		}
	}

	template<typename T, typename S>
	void pack(std::map<T, S> src, byte::container& dest, bool initial) {
		size_t n = src.size();
		bool t_basic = true;
		bool s_basic = false;
		if (initial) {
			dest.check_resize(size_t(n * compression_percent));
		}
		if (n <= 15) {
			dest.push_back(fixmap_t(n));
		}
		else if (n <= umax16) {
			dest.push_back(uint8_t(map16));
			dest.push_back(uint16_t(n));
		}
		else if (n <= umax32) {
			dest.push_back(uint8_t(map32));
			dest.push_back(uint32_t(n));
		}
		if (!(std::is_integral<T>::value || std::is_same<std::string, T>::value)) {
			t_basic = false;
		}
		if (std::is_integral<T>::value || std::is_same<std::string, T>::value) {
			s_basic = true;
		}
		for (auto e : src) {
			if (t_basic) {
				pack(e.first, dest);
			}
			else {
				pack(e.first, dest, false);
			}
			if (s_basic) {
				pack(e.second, dest);
			}
			else {
				pack(e.second, dest, false);
			}
		}
		if (initial) {
			dest.free_empty();
		}
	}
};

#endif