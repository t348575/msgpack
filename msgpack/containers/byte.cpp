#include <iostream>
#include <stdexcept>
#include <memory>
#include <sstream>
#include <string>
#include <bitset>

#include "byte.hpp"
#include "../formats.hpp"

namespace msgpack_byte {
	// constructors

	container::~container() {
		delete[] data;
	}

	// operators

	uint8_t& container::operator[] (int i) {
		if (i >= c) {
			throw std::out_of_range(std::to_string(i) + " out of range!");
		}
		return data[i];
	}

	bool container::operator==(const container& rhs) const {
		if (size() != rhs.size()) {
			return false;
		}
		const size_t n = size();
		for (int i = 0; i < n; i++) {
			if (rhs.data[i] != data[i]) {
				return false;
			}
		}
		return true;
	}

	bool container::operator!=(const container& rhs) const {
		if (size() != rhs.size()) {
			return true;
		}
		const size_t n = size();
		for (int i = 0; i < n; i++) {
			if (rhs.data[i] == data[i]) {
				return true;
			}
		}
		return false;
	}

	// insertion

	void container::push_back(uint8_t value) {
		data[s] = value;
		s++;
		check_expand();
	}

	void container::push_back(uint8_t* value) {
		data[s] = *value;
		s++;
		check_expand();
	}

	void container::push_back(uint16_t value) {
		check_resize(2);
		data[s] = ((value & 0xFF00) >> 8);
		data[s + 1] = (value & 0x00FF);
		s += 2;
		check_expand();
	}

	void container::push_back(uint16_t* value) {
		check_resize(2);
		data[s] = ((*value & 0xFF00) >> 8);
		data[s + 1] = (*value & 0x00FF);
		s += 2;
		check_expand();
	}

	void container::push_back(uint32_t value) {
		check_resize(4);
		data[s] = ((value & 0xFF000000) >> 24);
		data[s + 1] = ((value & 0x00FF0000) >> 16);
		data[s + 2] = ((value & 0x0000FF00) >> 8);
		data[s + 3] = (value & 0x000000FF);
		s += 4;
		check_expand();
	}

	void container::push_back(uint32_t* value) {
		check_resize(4);
		data[s] = ((*value & 0xFF000000) >> 24);
		data[s + 1] = ((*value & 0x00FF0000) >> 16);
		data[s + 2] = ((*value & 0x0000FF00) >> 8);
		data[s + 3] = (*value & 0x000000FF);
		s += 4;
		check_expand();
	}

	void container::push_back(uint64_t value) {
		check_resize(8);
		data[s] = ((value & 0xFF00000000000000) >> 56);
		data[s + 1] = ((value & 0x00FF000000000000) >> 48);
		data[s + 2] = ((value & 0x0000FF0000000000) >> 40);
		data[s + 3] = ((value & 0x000000FF00000000) >> 32);
		data[s + 4] = ((value & 0x00000000FF000000) >> 24);
		data[s + 5] = ((value & 0x0000000000FF0000) >> 16);
		data[s + 6] = ((value & 0x000000000000FF00) >> 8);
		data[s + 7] = (value & 0x00000000000000FF);
		s += 8;
		check_expand();
	}

	void container::push_back(uint64_t* value) {
		check_resize(8);
		data[s] = ((*value & 0xFF00000000000000) >> 56);
		data[s + 1] = ((*value & 0x00FF000000000000) >> 48);
		data[s + 2] = ((*value & 0x0000FF0000000000) >> 40);
		data[s + 3] = ((*value & 0x000000FF00000000) >> 32);
		data[s + 4] = ((*value & 0x00000000FF000000) >> 24);
		data[s + 5] = ((*value & 0x0000000000FF0000) >> 16);
		data[s + 6] = ((*value & 0x000000000000FF00) >> 8);
		data[s + 7] = (*value & 0x00000000000000FF);
		s += 8;
		check_expand();
	}

	void container::push_back(float value) {
		check_resize(4);
		uint8_t* float_bytes = reinterpret_cast<uint8_t*>(&value);
		push_back(float_bytes[3]);
		push_back(float_bytes[2]);
		push_back(float_bytes[1]);
		push_back(float_bytes[0]);
	}

	void container::push_back(float* value) {
		check_resize(4);
		uint8_t* float_bytes = reinterpret_cast<uint8_t*>(value);
		push_back(float_bytes[3]);
		push_back(float_bytes[2]);
		push_back(float_bytes[1]);
		push_back(float_bytes[0]);
	}

	void container::push_back(double value) {
		check_resize(8);
		uint8_t* double_bytes = reinterpret_cast<uint8_t*>(&value);
		push_back(double_bytes[7]);
		push_back(double_bytes[6]);
		push_back(double_bytes[5]);
		push_back(double_bytes[4]);
		push_back(double_bytes[3]);
		push_back(double_bytes[2]);
		push_back(double_bytes[1]);
		push_back(double_bytes[0]);
	}

	void container::push_back(double* value) {
		check_resize(8);
		uint8_t* double_bytes = reinterpret_cast<uint8_t*>(value);
		push_back(double_bytes[7]);
		push_back(double_bytes[6]);
		push_back(double_bytes[5]);
		push_back(double_bytes[4]);
		push_back(double_bytes[3]);
		push_back(double_bytes[2]);
		push_back(double_bytes[1]);
		push_back(double_bytes[0]);
	}

	void container::push_back(char value) {
		data[s] = value;
		s++;
		check_expand();
	}

	void container::push_back(const char* src, uint32_t len) {
		check_resize(len);
		for (uint32_t i = 0; i < len; i++) {
			data[s + i] = src[i];
		}
		s += len;
	}

	void container::push_back(char* src, uint32_t len) {
		check_resize(len);
		for (uint32_t i = 0; i < len; i++) {
			data[s + i] = src[i];
		}
		s += len;
	}

	void container::push_back(const std::string& src, uint32_t len) {
		if (len == 0) {
			len = src.length();
		}
		check_resize(len);
		for (uint32_t i = 0; i < len; i++) {
			data[s + i] = src[i];
		}
		s += len;
	}

	// reading

	uint8_t container::get_header(uint64_t& pos) {
		Iterator it = Iterator(data + pos);
		if (it != end()) {
			return data[pos++];
		}
		else {
			throw std::out_of_range("out of range!");
		}
	}
	uint8_t container::read_byte(uint64_t& pos) {
		return data[pos++];
	}
	uint16_t container::read_word(uint64_t& pos) {
		uint16_t output = 0;
		*((uint8_t*)(&output)) = data[pos + 1];
		*((uint8_t*)(&output) + 1) = data[pos];
		pos += 2;
		return output;
	}
	// remaining reads in byte.hpp

	// utility

	bool container::empty() const {
		return size() == 0;
	}

	size_t container::size() const {
		return s;
	}

	size_t container::capacity() const {
		return c;
	}

	void container::resize(size_t reserve) {
		c = (c + reserve) + 1;
		uint8_t* temp_arr = new uint8_t[c];
		std::copy(data, data + s, temp_arr);
		delete[] data;
		data = temp_arr;
	}

	bool container::shrink_to_fit(bool lenient) {
		if (lenient && s != c - 1 && c > lenient_size && c - lenient_size > s) {
			c = s + 1;
			uint8_t* temp_arr = new uint8_t[c];
			std::copy(data, data + s, temp_arr);
			delete[] data;
			data = temp_arr;
			return true;
		}
		else if (!lenient) {
			c = s + 1;
			uint8_t* temp_arr = new uint8_t[c];
			std::copy(data, data + s, temp_arr);
			delete[] data;
			data = temp_arr;
			return true;
		}
		return false;
	}

	uint8_t* container::raw_pointer() {
		return data;
	}

	uint8_t* container::raw_pointer(uint64_t pos) {
		return data + pos;
	}

	// internal

	void container::check_expand() {
		if (s == c) {
#ifdef doubling_strategy
			c = c * 2;
#else
			c = size_t(c * 1.1);
#endif
			uint8_t* temp_arr = new uint8_t[c];
			std::copy(data, data + s, temp_arr);
			delete[] data;
			data = temp_arr;
		}
	}

	void container::clear_resize(size_t reserve) {
		s = 0;
		c = reserve + 1;
		delete[] data;
		data = new uint8_t[c];
	}

	void container::check_resize(size_t bytes) {
		if (bytes + s >= c) {
			resize(bytes - c + s);
		}
	}

	// iterator implementation

	container::Iterator::reference container::Iterator::operator*() const {
		return *ptr;
	}

	container::Iterator::pointer container::Iterator::operator->() {
		return ptr;
	}

	container::Iterator& container::Iterator::operator++() {
		ptr++;
		return *this;
	}

	container::Iterator container::Iterator::operator++(int) {
		Iterator tmp = *this;
		++(*this);
		return tmp;
	}

	container::Iterator& container::Iterator::operator+=(int const& lhs) {
		this->ptr += lhs;
		return *this;
	}

	container::Iterator container::Iterator::operator+(int const& lhs) {
		return this->ptr + lhs;
	}

	container::Iterator& container::Iterator::operator--() {
		ptr--;
		return *this;
	}

	container::Iterator container::Iterator::operator--(int) {
		Iterator tmp = *this;
		--(*this);
		return tmp;
	}

	container::Iterator& container::Iterator::operator-=(int const& lhs) {
		this->ptr -= lhs;
		return *this;
	}

	container::Iterator container::Iterator::operator-(int const& lhs) {
		return this->ptr - lhs;
	}

	bool operator== (const container::Iterator& a, const container::Iterator& b) {
		return a.ptr == b.ptr;
	}

	bool operator!= (const container::Iterator& a, const container::Iterator& b) {
		return a.ptr != b.ptr;
	}

	container::Iterator container::begin() const {
		return Iterator(&data[0]);
	}

	container::Iterator container::end() const {
		return Iterator(&data[s]);
	}

	// msgpack_byte as stringstream

	std::stringstream to_stringstream(msgpack_byte::container& element, bool hex) {
		std::stringstream result;
		if (hex) {
			for (auto& e : element) {
				result << hexify<uint32_t>(e);
			}
		}
		else {
			for (auto& e : element) {
				result << e;
			}
		}
		return result;
	}

	std::string to_string(msgpack_byte::container& element) {
		std::string result = "";
		result.reserve(element.size());
		for (auto& e : element) {
			result += e;
		}
		return result;
	}
}