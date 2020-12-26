#include <stdexcept>
#include <memory>
#include <sstream>
#include <string>
#include <iomanip>

#include "byte.hpp"

namespace byte {

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

	// internal

	void container::check_expand() {
		if (s == c) {
			c = c * 2;
			uint8_t* temp_arr = new uint8_t[c];
			std::copy(data, data + (s - 1), temp_arr);
			delete[] data;
			data = temp_arr;
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
		++(*this); return tmp;
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
	
	/*template<typename T>
	std::string hexify(T i) {
		std::stringstream stream;
		stream << std::setfill('0') << std::setw(2) << std::hex << i;
		return stream.str();
	}

	std::stringstream to_stringstream(byte::container& element, bool hex) {
		std::stringstream result;
		if (hex) {
			for (auto& e : element) {
				result << hexify<uint32_t>(e) << " ";
			}
		}
		return result;
	}*/
}