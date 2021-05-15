#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <cstdint>
#include <iterator>
#include <cstddef>
#include <sstream>
#include <iomanip>
#include <any>

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
		void push_back(const std::string& src, uint32_t len = 0);

		// utility

		bool empty() const;
		size_t size() const;
		size_t capacity() const;
		void resize(size_t reserve);
		bool shrink_to_fit(bool lenient = true);

		uint8_t* raw_pointer();
		uint8_t* raw_pointer(uint64_t pos);

		// internal

		void check_expand();
		void clear_resize(size_t reserve);
		void check_resize(size_t reserve);

		// iterator class

		class Iterator {
		public:

			uint8_t* ptr;

			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = uint8_t;
			using pointer = uint8_t*;
			using reference = uint8_t&;

			Iterator(pointer m_ptr) : ptr(m_ptr) { }
			reference operator*() const;
			pointer operator->();
			Iterator& operator++();
			Iterator operator++(int);
			Iterator& operator+=(int const& lhs);
			Iterator operator+(int const& lhs);
			Iterator& operator--();
			Iterator operator--(int);
			Iterator& operator-=(int const& lhs);
			Iterator operator-(int const& lhs);
			friend bool operator== (const Iterator& a, const Iterator& b);
			friend bool operator!= (const Iterator& a, const Iterator& b);
		};

		Iterator begin() const;
		Iterator end() const;

		// reading

		uint8_t get_header(uint64_t& pos);
		uint8_t read_byte(uint64_t& pos);
		uint16_t read_word(uint64_t& pos);

		template<typename T = uint32_t>
		T read_d_word(uint64_t& pos) {
			T output;
			*((uint8_t*)(&output)) = data[pos + 3];
			*((uint8_t*)(&output) + 1) = data[pos + 2];
			*((uint8_t*)(&output) + 2) = data[pos + 1];
			*((uint8_t*)(&output) + 3) = data[pos];
			pos += 4;
			return output;
		}
		template<typename T = uint64_t>
		T read_q_word(uint64_t& pos) {
			T output;
			*((uint8_t*)(&output)) = data[pos + 7];
			*((uint8_t*)(&output) + 1) = data[pos + 6];
			*((uint8_t*)(&output) + 2) = data[pos + 5];
			*((uint8_t*)(&output) + 3) = data[pos + 4];
			*((uint8_t*)(&output) + 4) = data[pos + 3];
			*((uint8_t*)(&output) + 5) = data[pos + 2];
			*((uint8_t*)(&output) + 6) = data[pos + 1];
			*((uint8_t*)(&output) + 7) = data[pos];
			pos += 8;
			return output;
		}

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
	std::string to_string(container& element);
};

#endif