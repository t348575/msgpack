#include <iostream>
#include <chrono>
#include <vector>

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
	msgpack a;
	vector<uint32_t> b;
	for (uint32_t i = 0; i < 16; i++) {
		b.push_back(i);
	}
	std::cout << std::endl;
	a.pack<uint32_t>(b, dest);
	// std::cout << byte::to_stringstream(dest).str() << std::endl;
	return 0;
}