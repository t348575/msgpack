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
#include "VisualProfiler.h"

struct AllocationMetrics {
	uint32_t TotalAllocated = 0;
	uint32_t TotalFreed = 0;
	uint32_t CurrentUsage() { return TotalAllocated - TotalFreed; }
};

static AllocationMetrics s_AllocationMetrics;

/*void* operator new(size_t size) {
	s_AllocationMetrics.TotalAllocated += size;
	return malloc(size);
}

void operator delete(void* memory, size_t size) {
	s_AllocationMetrics.TotalFreed += size;
	free(memory);
}*/

namespace msgpack {

#define PROFILING 1
#ifdef PROFILING
#define PROFILE_SCOPE(name) VisualProfilerTimer timer##__LINE__(name)
#define PROFILE_FUNCTION()  PROFILE_SCOPE(__FUNCSIG__)
#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#endif

	void PrintCurrentUsage() {
		std::cout << "Current usage: " << s_AllocationMetrics.CurrentUsage() << " Total Allocated: " << s_AllocationMetrics.TotalAllocated << " Total Freed: " << s_AllocationMetrics.TotalFreed << std::endl;
	}

	// some definitions

	using namespace msgpack_byte;

	template<typename T>
	void pack(std::vector<T>& src, container& dest, bool initial = true);
	template<typename ...T>
	void pack(std::tuple<T...>& src, container& dest, bool initial = true);
	template<typename T, typename S>
	void pack(std::map<T, S>& src, container& dest, bool initial = true);

	template<typename T>
	void unpack(std::vector<T>& dest, container& src);
	template<typename ...T>
	void unpack(std::tuple<T...>& dest, container& src);
	template<typename T, typename S>
	void unpack(std::map<T, S>& dest, container& src);
	template<typename T>
	void unpack(std::vector<T>& dest, container& src, uint64_t& pos);
	template<typename ...T>
	void unpack(std::tuple<T...>& dest, container& src, uint64_t& pos);
	template<typename T, typename S>
	void unpack(std::map<T, S>& dest, container& src, uint64_t& pos);

	template <typename Tup>
	size_t iterate_tuple_types_2(const Tup& t);
	template<typename A, typename B>
	size_t LengthOf(const std::map<A, B>& s);
	template <typename T>
	size_t LengthOf(const std::vector<T>& s);
	template <typename ...T>
	size_t LengthOf(const std::tuple<T...>& s);
	template <typename ... Params>
	size_t LengthOf(const std::basic_string<Params...>& s);
	template <typename T>
	size_t LengthOf(const T&);

	// utility

	size_t element_size(container& ele, uint64_t& pos) {
		uint8_t header = ele[pos];
		pos++;
		if (header >= 0 && header <= posmax8) {
			return 0;
		}
		else if (header >= fixmap && header < fixarray) {
			return header - uint8_t(fixmap);
		}
		else if (header >= fixarray && header < fixstr) {
			return header - uint8_t(fixarray);
		}
		else if (header >= fixstr && header < nil) {
			return header - uint8_t(fixstr);
		}
		switch (header) {
		case tru:
		case flse:
		case nil: {
			return 0;
		}
		case int8:
		case uint8: {
			pos++;
			return 1;
		}
		case int16:
		case uint16: {
			pos += 2;
			return 2;
		}
		case float32:
		case int32:
		case uint32: {
			pos += 4;
			return 4;
		}
		case float64:
		case int64:
		case uint64: {
			pos += 8;
			return 8;
		}
		case str8: {
			return ele.read_byte(pos);
		}
		case map16:
		case arr16:
		case str16: {
			return ele.read_word(pos);
		}
		case arr32:
		case map32:
		case str32: {
			return ele.read_d_word(pos);
		}
		}
		return 0;
	}

	// packing functions - primitive

	void pack(const char& src, container& dest, bool initial = false) { PROFILE_FUNCTION();

		dest.push_back(uint8_t(single_char));
		dest.push_back(src);
	}
	void pack(const char* src, size_t len, container& dest, bool initial = false) { PROFILE_FUNCTION();
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
	void pack(char* src, size_t len, container& dest, bool initial = false) { PROFILE_FUNCTION();
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
	void pack(const std::string& src, container& dest, bool initial = false) { PROFILE_FUNCTION();

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
	void pack_uint(const uint64_t& src, container& dest, bool initial = false) { PROFILE_FUNCTION();
		if (src <= posmax8) {
			dest.push_back(uint8_t(ufixint_t(src)));
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
	void pack_int(const int64_t& src, container& dest, bool initial = false) { PROFILE_FUNCTION();
		uint64_t a = src;;
		if (src >= 0 && src <= posmax8) {
			dest.push_back(uint8_t(src));
		}
		else if (src >= int8_t(neg32) && src < 0) {
			dest.push_back(uint8_t(src));
		}
		else if (src >= int8_t(negmax8) && src <= int8_t(posmax8)) {
			dest.push_back(uint8_t(int8));
			dest.push_back(uint8_t(src));
		}
		else if (src >= int16_t(negmax16) && src <= int16_t(posmax16)) {
			dest.push_back(uint8_t(int16));
			dest.push_back(uint16_t(src));
		}
		else if (src >= int32_t(negmax32) && src <= int32_t(posmax32)) {
			dest.push_back(uint8_t(int32));
			dest.push_back(uint32_t(src));
		}
		else if (src >= int64_t(negmax64) && src <= int64_t(posmax64)) {
			dest.push_back(uint8_t(int64));
			dest.push_back(uint64_t(src));
		}
		else {
			throw std::range_error(std::to_string(src) + " out of range!");
		}
	}
	void pack(uint8_t& src, container& dest, bool initial = false) { PROFILE_FUNCTION();
		pack_uint(static_cast<uint64_t>(src), dest);
	}
	void pack(uint16_t& src, container& dest, bool initial = false) { PROFILE_FUNCTION();
		pack_uint(static_cast<uint64_t>(src), dest);
	}
	void pack(uint32_t& src, container& dest, bool initial = false) { PROFILE_FUNCTION();
		pack_uint(static_cast<uint64_t>(src), dest);
	}
	void pack(uint64_t& src, container& dest, bool initial = false) { PROFILE_FUNCTION();
		pack_uint(static_cast<uint64_t>(src), dest);
	}
	void pack(int8_t& src, container& dest, bool initial = false) { PROFILE_FUNCTION();
		pack_int(static_cast<int64_t>(src), dest);
	}
	void pack(int16_t& src, container& dest, bool initial = false) { PROFILE_FUNCTION();
		pack_int(static_cast<int64_t>(src), dest);
	}
	void pack(int32_t& src, container& dest, bool initial = false) { PROFILE_FUNCTION();
		pack_int(static_cast<int64_t>(src), dest);
	}
	void pack(int64_t& src, container& dest, bool initial = false) { PROFILE_FUNCTION();
		pack_int(src, dest);
	}
	void pack(double& src, container& dest, bool initial = false) { PROFILE_FUNCTION();

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
	void pack(float& src, container& dest, bool initial = false) { PROFILE_FUNCTION();

		dest.push_back(uint8_t(float32));
		dest.push_back(src);
	}
	void pack(bool& src, container& dest, bool initial = false) { PROFILE_FUNCTION();
		if (src) {
			dest.push_back(uint8_t(tru));
		}
		else {
			dest.push_back(uint8_t(flse));
		}
	}
	/*void pack(container& dest, bool initial = false) {
		dest.push_back(uint8_t(nil));
	}*/

	// stl iterators

	template<class F, class...Ts, std::size_t...Is>
	void tuple_iterator_pack(std::tuple<Ts...>& tuple, F func, std::index_sequence<Is...>, container& dest) {
		PROFILE_FUNCTION();
		using expander = int[];
		(void)expander {
			0, ((void)func(dest, std::get<Is>(tuple)), 0)...
		};
	}

	template<class F, class...Ts>
	void tuple_iterator_pack(std::tuple<Ts...>& tuple, container& dest, F func) {
		PROFILE_FUNCTION();
		tuple_iterator_pack(tuple, func, std::make_index_sequence<sizeof...(Ts)>(), dest);
	}

	template <typename T>
	size_t LengthOf(const T&) {
		PROFILE_FUNCTION();
		// std::cout << typeid(T).name() << "\t" << sizeof(T) << std::endl;
		return sizeof(T);
	}

	template <typename ... Params>
	size_t LengthOf(const std::basic_string<Params...>& s) {
		PROFILE_FUNCTION();
		// std::cout << typeid(s).name() << "\t" << s.length() << std::endl;
		return s.length();
	}

	template <typename ...T>
	size_t LengthOf(const std::tuple<T...>& s) {
		PROFILE_FUNCTION();
		auto sum_length = [](const auto&... args) {
			// std::cout << typeid(args).name() << std::endl;
			return (LengthOf(args) + ...);
		};
		return std::apply(sum_length, s);
	}

	template <typename T>
	size_t LengthOf(const std::vector<T>& s) {
		PROFILE_FUNCTION();
		// std::cout << typeid(s).name() << "\t" << s.size() * sizeof(T) << std::endl;
		if (std::is_integral<T>::value) {
			return s.size() * sizeof(T);
		}
		else {
			size_t result = 0;
			for (auto& e : s) {
				result += LengthOf(e);
			}
			return result;
		}
	}

	template<typename A, typename B>
	size_t LengthOf(const std::map<A, B>& s) {
		PROFILE_FUNCTION();
		size_t result = 0;
		for (auto& e : s) {
			result += LengthOf(e.first);
			result += LengthOf(e.second);
		}
		return result;
	}

	template <typename Tup>
	size_t iterate_tuple_types_2(const Tup& t) {
		PROFILE_FUNCTION();
		auto sum_length = [](const auto&... args) {
			// std::cout << typeid(args).name() << std::endl;
			return (LengthOf(args) + ...);
		};
		return std::apply(sum_length, t);
	}

	// packing functions - STL

	template <typename T>
	void pack(std::vector<T>& src, container& dest, bool initial) {
		PROFILE_FUNCTION();
		size_t n = src.size();
		if (initial) {
			dest.check_resize(size_t((LengthOf(src) + 1) * compression_percent));
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
			pack(src[i], dest, false);
		}
		if (initial) {
			dest.shrink_to_fit();
		}
	}

	template<typename ...T>
	void pack(std::tuple<T...>& src, container& dest, bool initial) {
		PROFILE_FUNCTION();
		size_t n = std::tuple_size<typename std::remove_reference<decltype(src)>::type>::value;
		if (initial) {
			// std::cout << recursive_size(src) << "\t" << size_t(recursive_size(src) * compression_percent) << std::endl;
			dest.check_resize(size_t((iterate_tuple_types_2(src) + 1) * compression_percent));
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
		// iterate_tuple(src, dest);

		// msgpack::tuple_iterator_2(src, dest, [](container& dest, auto& x) { pack(x, dest); });

		tuple_iterator_pack(src, dest, [](container& dest, auto& src) { pack(src, dest, false); });
		if (initial) {
			dest.shrink_to_fit();
		}
	}

	template<typename T, typename S>
	void pack(std::map<T, S>& src, container& dest, bool initial) {
		PROFILE_FUNCTION();
		size_t n = src.size();
		// bool t_basic = true;
		// bool s_basic = false;
		if (initial) {
			dest.check_resize(size_t((LengthOf(src) + 1) * compression_percent));
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
		/*if (!(std::is_integral<T>::value || std::is_same<std::string, T>::value)) {
			t_basic = false;
		}
		if (std::is_integral<T>::value || std::is_same<std::string, T>::value) {
			s_basic = true;
		}*/
		for (auto& e : src) {
			/*if (t_basic) {
				pack(e.first, dest, false);
			}
			else {
				pack(e.first, dest, false);
			}
			if (s_basic) {
				pack(e.second, dest, false);
			}
			else {
				pack(e.second, dest, false);
			}*/
			pack(e.first, dest, false);
			pack(e.second, dest, false);
		}
		if (initial) {
			dest.shrink_to_fit();
		}
	}

	// unpacking

	template<typename T>
	void unpack_int(T& dest, container& src, uint64_t& pos) {
		uint8_t header = src.get_header(pos);
		if (header >= 0 && header <= posmax8) {
			dest = int8_t(header);
		}
		else {
			switch (header) {
			case int8:
			case uint8: {
				dest = src.read_byte(pos);
				break;
			}
			case int16:
			case uint16: {
				dest = src.read_word(pos);
				break;
			}
			case int32:
			case uint32: {
				dest = src.read_d_word(pos);
				break;
			}
			case int64:
			case uint64: {
				dest = src.read_q_word(pos);
				break;
			}
			}
		}
	}

	void unpack(char& dest, container& src, uint64_t& pos) {
		uint8_t header = src.get_header(pos);
		if (header == single_char) {
			dest = src.read_byte(pos);
		}
	}
	void unpack(std::string& dest, container& src, uint64_t& pos) {
		uint8_t header = src.get_header(pos);
		if (header >= fixstr && header <= fixstr_end) {
			uint8_t n = fixstr_len(header);
			dest.resize(n);
			memcpy(&dest[0], src.raw_pointer(pos), fixstr_len(header));
			pos += n;
		}
		else {
			if (header == str8) {
				uint8_t n = src.read_byte(pos);
				dest.resize(n);
				memcpy(&dest[0], src.raw_pointer(pos), n);
			}
			else if (header == str16) {
				uint16_t n = src.read_byte(pos);
				dest.resize(n);
				memcpy(&dest[0], src.raw_pointer(pos), n);
			}
			else if (header == str32) {
				uint32_t n = src.read_byte(pos);
				dest.resize(n);
				memcpy(&dest[0], src.raw_pointer(pos), n);
			}
		}
	}
	void unpack(uint8_t& dest, container& src, uint64_t& pos) {
		unpack_int(dest, src, pos);
	}
	void unpack(uint16_t& dest, container& src, uint64_t& pos) {
		unpack_int(dest, src, pos);
	}
	void unpack(uint32_t& dest, container& src, uint64_t& pos) {
		unpack_int(dest, src, pos);
	}
	void unpack(uint64_t& dest, container& src, uint64_t& pos) {
		unpack_int(dest, src, pos);
	}
	void unpack(int8_t& dest, container& src, uint64_t& pos) {
		unpack_int(dest, src, pos);
	}
	void unpack(int16_t& dest, container& src, uint64_t& pos) {
		unpack_int(dest, src, pos);
	}
	void unpack(int32_t& dest, container& src, uint64_t& pos) {
		unpack_int(dest, src, pos);
	}
	void unpack(int64_t& dest, container& src, uint64_t& pos) {
		unpack_int(dest, src, pos);
	}
	void unpack(double& dest, container& src, uint64_t& pos) {
		uint8_t header = src.get_header(pos);
		if (header == float32) {
			dest = (double)src.read_d_word<float>(pos);
		}
		else {
			dest = src.read_q_word<double>(pos);
		}
	}
	void unpack(float& dest, container& src, uint64_t& pos) {
		pos++; // skip header
		dest = src.read_d_word<float>(pos);
	}
	void unpack(bool& dest, container& src, uint64_t& pos) {
		dest = src.get_header(pos);
	}

	template<typename T>
	void unpack(std::vector<T>& dest, container& src, uint64_t& pos) {
		static_assert(!std::is_same<void, T>::value);
		size_t n = element_size(src, pos);
		dest.resize(n);
		for (uint64_t i = 0; i < n; i++) {
			T temp;
			unpack(temp, src, pos);
			dest[i] = temp;
		}
	}

	template<typename ...T>
	void unpack(std::tuple<T...>& dest, container& src, uint64_t& pos) {
		size_t n = element_size(src, pos);
		tuple_iterator_pack(dest, src, [&pos](container& dest, auto& src) { unpack(src, dest, pos); });
	}

	template<typename T, typename S>
	void unpack(std::map<T, S>& dest, container& src, uint64_t& pos) {
		size_t n = element_size(src, pos);
		for (uint64_t i = 0; i < n; i++) {
			T first;
			S second;
			unpack(first, src, pos);
			unpack(second, src, pos);
			dest.insert({ first, second });
		}
	}

	template<typename T>
	void unpack(std::vector<T>& dest, container& src) {
		uint64_t pos = 0;
		unpack<T>(dest, src, pos);
	}

	template<typename ...T>
	void unpack(std::tuple<T...>& dest, container& src) {
		uint64_t pos = 0;
		unpack<T...>(dest, src, pos);
	}

	template<typename T, typename S>
	void unpack(std::map<T, S>& dest, container& src) {
		uint64_t pos = 0;
		unpack<T, S>(dest, src, pos);
	}
};

#endif



// ANCIENT STUFF

/*
* template <typename T, typename S>
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
		else if (std::is_same<std::basic_string<char, struct std::char_traits<char>, class std::allocator<char> >, NthTypeOf<I, Ts...> >::value) {
			typename std::tuple_element<I, std::tuple<Ts...> >::type test = std::get<I>(tup);
			std::stringstream ss(test);
			result += static_cast<size_t>(ss.str().length());
		}
		else {
			std::cout << typeid(std::get<I>(tup)).name() << std::endl;
		}
return result + recursive_size<I + 1>(tup);
	}
template<class... Fs>
	void do_in_order(Fs&&... fs) {
		int unused[] = { 0, ((void)std::forward<Fs>(fs)(), 0)... };
		(void)unused; // blocks warnings
	}

	template<class T>
	constexpr std::make_index_sequence<std::tuple_size<T>::value>
		get_indexes(T const&)
	{
		return {};
	}

	template<size_t... Is, class Tuple, class F>
	void tuple_iterator_2(std::index_sequence<Is...>, Tuple&& tup, F&& f, container&& dest) {
		using std::get;
		do_in_order([&] { f(dest, get<Is>(std::forward<Tuple>(tup))); }...);
	}

	template<class Tuple, class F>
	void tuple_iterator_2(Tuple&& tup, container& dest, F&& f) {
		auto indexes = get_indexes(tup);
		tuple_iterator_2(indexes, std::forward<Tuple>(tup), std::forward<F>(f), std::forward<container>(dest));
	}
template <size_t I = 0, typename... Ts>
	typename std::enable_if<I == sizeof...(Ts), void>::type
		iterate_tuple(std::tuple<Ts...> tup, container& dest) {
		return;
	}

	template <size_t I = 0, typename... Ts>
	typename  std::enable_if<(I < sizeof...(Ts)), void>::type
		iterate_tuple(std::tuple<Ts...> tup, container& dest) {
		std::cout << typeid(std::get<I>(tup)).name() << std::endl;
		if (std::is_integral<NthTypeOf<I, Ts...> >::value || std::is_same<std::string, NthTypeOf<I, Ts...> >::value) {
			pack(std::get<I>(tup), dest);
		}
		else {
			pack(std::get<I>(tup), dest);
		}
		iterate_tuple<I + 1>(tup, dest);
	}
	*/