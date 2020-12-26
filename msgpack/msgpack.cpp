#include <stdexcept>
#include <string>

#include "msgpack.hpp"

template <typename T>
void msgpack::pack(std::vector<T>& src, byte::container& dest, bool initial) {
	size_t n = src.size();
	size_t s = 0;
	uint8_t prefix;
	uint16_t post_prefix = n;
	bool use_post_prefix = false;
	if (n <= 15) {
		prefix = fixarray_t(n);
	}
	else if (n <= max16) {
		prefix = arr16;
		use_post_prefix = true;
	}
	else if (n <= max32) {
		prefix = arr32;
		use_post_prefix = true;
	}
	if (initial) {
		dest.clear_resize(int(n * 0.4));
	}
	dest.push_back(&prefix);
	if (use_post_prefix) {
		dest.push_back(&post_prefix);
	}
	for (uint32_t i = 0; i < n; i++) {
		pack(src[i], dest, false);
	}
}

void msgpack::pack(uint64_t src, byte::container& dest, bool initial) {
	uint8_t prefix;
	if (src <= max4) {
		prefix = ufixint_t(src);
		dest.push_back(&prefix);
	}
	else if (src <= max8) {
		prefix = uint8;
		dest.push_back(&prefix);
		dest.push_back(uint8_t(src));
	}
	else if (src <= max16) {
		prefix = uint16;
		dest.push_back(&prefix);
		dest.push_back(uint16_t(src));
	}
	else if (src <= max32) {
		prefix = uint32;
		dest.push_back(&prefix);
		dest.push_back(uint32_t(src));
	}
	else if (src <= max64) {
		prefix = uint64;
		dest.push_back(&prefix);
		dest.push_back(uint64_t(src));
	}
	else {
		throw std::range_error(std::to_string(src) + " out of range!");
	}
}