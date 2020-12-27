#include <vector>
#include <tuple>
#include <cstdint>
#include <iostream>
#include <sstream>

#include "msgpack.hpp"
using namespace std;
int main() {
	std::string z;
	/*double runs[100];
	double sum = 0;
	for (int j = 0; j < 100; j++) {
		auto start = std::chrono::high_resolution_clock::now();
		std::vector<uint8_t> a(1000000000);
		for (int i = 0; i < 1000000000; i++) {
			a.push_back((uint8_t)14);
		}
		auto end = std::chrono::high_resolution_clock::now();
		runs[j] = (double)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << "Took: " << runs[j] << std::endl;
	}
	for (int i = 0; i < 100; i++) {
		sum += runs[i];
	}
	std::cout << "Average time: " << (double)(sum / 100) << std::endl;*/
	byte::container dest;
	tuple <char, unsigned int, double> t;
	t = make_tuple('a', 10, 15.5);
	msgpack::pack(t, dest);
	std::cout << byte::to_stringstream(dest).str() << std::endl;
	return 0;
}