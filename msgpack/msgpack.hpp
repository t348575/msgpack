#ifndef MSGPACK_HPP
#define MSGPACK_HPP

#include <vector>

#include "containers/byte.hpp"
#include "formats.hpp"

class msgpack {
	public:
		msgpack() = default;

		// packing functions - STL

		template <typename T>
		void pack(std::vector<T>& src, byte::container& dest, bool initial = true);

		// packing functions - primitive

		void pack(uint64_t src, byte::container& dest, bool initial = true);
};

#endif