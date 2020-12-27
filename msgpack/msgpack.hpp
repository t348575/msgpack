#ifndef MSGPACK_HPP
#define MSGPACK_HPP

#include <type_traits>
#include <iostream>
#include <numeric>
#include <vector>
#include <tuple>
#include <string>

#include "containers/byte.hpp"
#include "formats.hpp"

namespace msgpack {

	// packing functions - primitive

	// string

	void pack(const char src, byte::container& dest) {
		dest.push_back(uint8_t(single_char));
		dest.push_back(src);
	}
	void pack(const char* src, size_t len, byte::container& dest) {
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
	void pack(char* src, size_t len, byte::container& dest) {
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
	void pack(std::string src, byte::container& dest) {
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

	void pack_uint(uint64_t src, byte::container& dest) {
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

	void pack_int(int64_t src, byte::container& dest) {
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

	void pack(uint8_t src, byte::container& dest) {
		pack_uint(static_cast<uint64_t>(src), dest);
	}
	void pack(uint16_t src, byte::container& dest) {
		pack_uint(static_cast<uint64_t>(src), dest);
	}
	void pack(uint32_t src, byte::container& dest) {
		pack_uint(static_cast<uint64_t>(src), dest);
	}
	void pack(uint64_t src, byte::container& dest) {
		pack_uint(static_cast<uint64_t>(src), dest);
	}
	void pack(int8_t src, byte::container& dest) {
		pack_int(static_cast<int64_t>(src), dest);
	}
	void pack(int16_t src, byte::container& dest) {
		pack_int(static_cast<int64_t>(src), dest);
	}
	void pack(int32_t src, byte::container& dest) {
		pack_int(static_cast<int64_t>(src), dest);
	}
	void pack(int64_t src, byte::container& dest) {
		pack_int(src, dest);
	}

	// double

	void pack(double src, byte::container& dest) {
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

	void pack(float src, byte::container& dest) {
		dest.push_back(uint8_t(float32));
		dest.push_back(src);
	}

	// boolean

	void pack(bool src, byte::container& dest) {
		if (src) {
			dest.push_back(uint8_t(tru));
		}
		else {
			dest.push_back(uint8_t(flse));
		}
	}

	// nil

	void pack(byte::container& dest) {
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
		pack(std::get<I>(tup), dest);
		iterate_tuple<I + 1>(tup, dest);
	}

	// packing functions - STL

	template <typename T>
	void pack(std::vector<T> src, byte::container& dest, bool initial = true) {
		size_t n = src.size();
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
		if (initial) {
			dest.clear_resize(int(n * 0.4));
		}
		for (uint32_t i = 0; i < n; i++) {
			pack(src[i], dest);
		}
	}

	template<typename ...T>
	void pack(std::tuple<T...> src, byte::container& dest, bool initial = true) {
		size_t n = std::tuple_size<decltype(src)>::value;
		if (initial) {
			dest.clear_resize(int(n * 0.4));
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
	}

};

#endif