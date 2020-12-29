#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <cstdint>
#include <iterator>
#include <cstddef>
#include <sstream>
#include <iomanip>

namespace msgpack_byte {

	class container {

	public:

		container() : s(0), c(2), data(new uint8_t[2]) {};
		container(size_t reserve) : s(0), c(reserve + 1), data(new uint8_t[reserve + 1]) {};
		~container();

		// operators

		uint8_t& operator[] (int i);
		bool operator==(const container& rhs) const;
		bool operator!=(const container& rhs) const;

		// insertion

		void push_back(uint8_t value);
		void push_back(uint8_t* value);
		void push_back(uint16_t value);
		void push_back(uint16_t* value);
		void push_back(uint32_t value);
		void push_back(uint32_t* value);
		void push_back(uint64_t value);
		void push_back(uint64_t* value);
		void push_back(float value);
		void push_back(float* value);
		void push_back(double value);
		void push_back(double* value);
		void push_back(char value);
		void push_back(const char* src, uint32_t len);
		void push_back(char* src, uint32_t len);
		void push_back(std::string& src, uint32_t len = 0);

		// utility

		bool empty() const;
		size_t size() const;
		size_t capacity() const;
		void resize(size_t reserve);
		bool free_empty(bool lenient = true);

		// internal

		void check_expand();
		void clear_resize(size_t reserve);
		void check_resize(size_t reserve);

		// iterator class

		class Iterator {

		private:

			uint8_t* ptr;

		public:

			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = uint8_t;
			using pointer = uint8_t*;
			using reference = uint8_t&;

			Iterator(pointer m_ptr) : ptr(m_ptr) {}
			reference operator*() const;
			pointer operator->();
			Iterator& operator++();
			Iterator operator++(int);
			friend bool operator== (const Iterator& a, const Iterator& b);
			friend bool operator!= (const Iterator& a, const Iterator& b);

		};

		Iterator begin() const;
		Iterator end() const;

	private:

		uint8_t* data;
		size_t s;
		size_t c;

	};

	template<typename T>
	std::string hexify(T i) {
		std::stringstream stream;
		stream << std::setfill('0') << std::setw(2) << std::hex << i;
		return stream.str();
	}

	std::stringstream to_stringstream(container& element, bool hex = true);
};

#endif