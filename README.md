# msgpack
A simple C++17 (only) compatible msgpack implementation.

### Note: Still in development (only packing complete, unpacking still to be done)

### Note 2: Packing memory optimization not very good, allocates ~100MB extra for some reason when trying to pack 200MB of randomly generated data (not a problem at smaller sizes)

### Data structures added so far:
- vector
- map
- tuple
- Primitive types
    - All integers (int8, int16, int32, int64) signed and unsigned
    - Float, Double
    - String (char *, std::string)



### Data structures to be added
- list
- set
- multimap, unordered map, etc.
- user defined structures & classes


### Features to be added
- Iterable in memory msgpack (ie. stored as msgpack but iterable)
- Write to streams instead of stringstream or string


### Instructions
1. Copy msgpack.hpp, formats.hpp, and the containers folder
2. Include msgpack.hpp

### Hello World
Compile and run main.cpp to run a sample benchmark ~ 100 - 250 MB
```cpp
#include <iostream>
#include <tuple>
#include <map>
#include <vector>
int main() {
    msgpack_byte::container dest; // byte container holding the packed bytes
    tuple <char, unsigned int, double, string, vector<unsigned int>, map<string, uint64_t> > t; // an example container to pack
    vector<unsigned int> vec{ 1, 2, 3, 4, 5 };
    string abc = "test string";
    map<string, uint64_t> cde;
    cde.insert(make_pair(string("abc"), 4142342342342343));
    cde.insert(make_pair(string("cde"), 5));
    cde.insert(make_pair(string("def"), 11231233));
    t = make_tuple('a', 10, 0.333333333333333, abc, vec, cde);
    msgpack::pack(t, dest); // pass the container to be packed and the byte container
    std::cout << msgpack_byte::to_stringstream(dest).str() << std::endl; // return a stringstream of the packed data (in hex)
    std::cout << "Packed size: " << dest.size() << std::endl;
    return 0;
}
```