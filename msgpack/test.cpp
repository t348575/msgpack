#include <vector>
#include <list>
#include <chrono>
#include <tuple>
#include <random>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <type_traits>

#include "msgpack.hpp"

using namespace std;

#define TEST_NUM 100

#define maxchar 255
#define minchar 0
#define maxint 2147483647
#define minint -2147483648
#define maxdouble 1e300
#define mindouble -1e-300
#define constdouble 1e-300;
#define maxfloat 3e38
#define minfloat -1e-38
#define constfloat 1e-38;

char current_char = minchar;
int current_int = minint;
double current_double = mindouble;
float current_float = minfloat;

random_device rd;
mt19937 character_mt(rd());
uniform_int_distribution<uint32_t> character_pool(0, 255);
mt19937 size_mt(rd());
uniform_int_distribution<uint32_t> size_pool(0, 256);

char get_char() {
	if (current_char == maxchar) {
		current_char = minchar;
	}
	return current_char++;
}

int get_int() {
	if (current_int == maxint) {
		current_int = minint;
	}
	return current_int++;
}

double get_double() {
	if (current_double == maxdouble) {
		current_double = mindouble;
	}
	current_double += constdouble;
	return current_double;
}

float get_float() {
	if (current_float == maxfloat) {
		current_float = minfloat;
	}
	current_float += constfloat;
	return current_float;
}

char get_random_char() {
	return char(character_pool(character_mt));
}

string get_string() {
	uint32_t size = size_pool(size_mt);
	string result;
	result.reserve(size);
	for (uint32_t i = 0; i < size; i++) {
		result += get_random_char();
	}
	return result;
}
int main() {
	uint64_t total_bytes = 0;
	msgpack_byte::container dest;
	vector<tuple<char, vector<int>, int, string, double, map<int, vector<string> >, float > > test_vector(TEST_NUM);
	auto start_gen = chrono::high_resolution_clock::now();
	for (uint64_t i = 0; i < TEST_NUM; i++) {
		uint32_t first_vec_size = size_pool(size_mt);
		total_bytes += (first_vec_size * 4);
		vector<int> first_vec(first_vec_size);
		for (uint32_t j = 0; j < first_vec_size; j++) {
			first_vec[j] = get_int();
		}
		uint32_t map_size = size_pool(size_mt);
		map<int, vector<string> > first_map;
		for (uint32_t j = 0; j < map_size; j++) {
			uint32_t in_size = size_pool(size_mt);
			vector<string> in_vec(in_size);
			for (uint32_t k = 0; k < in_size; k++) {
				in_vec[k] = get_string();
				total_bytes += in_vec[k].length();
			}
			first_map.insert(make_pair(get_int(), in_vec));
		}
		total_bytes += (map_size * 4);
		string str = get_string();
		total_bytes += str.length();
		test_vector[i] = make_tuple(get_char(), first_vec, get_int(), str, get_double(), first_map, get_float());
		total_bytes += 17;
	}
	auto end_gen = chrono::high_resolution_clock::now();
	std::cout << total_bytes << " bytes " << (double)(total_bytes / 1e6) << "MB total generated in " << double(chrono::duration_cast<chrono::seconds>(end_gen - start_gen).count()) << " seconds" << endl;
	auto start_pack = chrono::high_resolution_clock::now();
	msgpack::pack(test_vector, dest);
	auto end_pack = chrono::high_resolution_clock::now();
	std::cout << dest.size() << " bytes " << (double)(dest.size() / 1e6) << "MB packed size in " << double(chrono::duration_cast<chrono::milliseconds>(end_pack - start_pack).count()) << " milliseconds" << endl;
	std::cout << "Packing efficiency: " << (double)((double)dest.size() / (double)total_bytes) * (double)100 << "%" << std::endl;
	return 0;
}