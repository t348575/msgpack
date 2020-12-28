#include <vector>
#include <tuple>
#include <cstdint>
#include <iostream>
#include <sstream>

#include "msgpack.hpp"
using namespace std;
int main() {
	string z;
	/*double runs[100];
	double sum = 0;
	for (int j = 0; j < 100; j++) {
		auto start = chrono::high_resolution_clock::now();
		vector<uint8_t> a(1000000000);
		for (int i = 0; i < 1000000000; i++) {
			a.push_back((uint8_t)14);
		}
		auto end = chrono::high_resolution_clock::now();
		runs[j] = (double)chrono::duration_cast<chrono::milliseconds>(end - start).count();
		cout << "Took: " << runs[j] << endl;
	}
	for (int i = 0; i < 100; i++) {
		sum += runs[i];
	}
	cout << "Average time: " << (double)(sum / 100) << endl;*/
	byte::container dest;
	tuple <char, unsigned int, double, string, vector<unsigned int>, map<string, uint64_t> > t;
	vector<unsigned int> vec{ 1, 2, 3, 4, 5 };
	string abc = "test string";
	map<string, uint64_t> cde;
	cde.insert(make_pair(string("abc"), 4142342342342343));
	cde.insert(make_pair(string("cde"), 5));
	cde.insert(make_pair(string("def"), 11231233));
	t = make_tuple('a', 10, 0.333333333333333, abc, vec, cde);
	msgpack::pack(t, dest);
	cout << byte::to_stringstream(dest).str() << endl;
	cout << "Packed size: " << dest.size() << endl;
	return 0;
}