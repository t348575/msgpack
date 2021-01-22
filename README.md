# msgpack
A simple C++17 (only) compatible msgpack implementation.

### Note: Still in development

### References:

- [msgpack.org](https://msgpack.org/) 
- [msgpack-c](https://github.com/msgpack/msgpack-c)
- [MessagePack](https://github.com/mneumann/MessagePack)
- [specification](https://github.com/msgpack/msgpack/blob/master/spec.md)

### Data structures added so far:
- vector
- map
- tuple
- list
- queue
- deque
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

### Current performance
- ~ 2GB of randomly generated data (not fixed sized `vectors`, `maps`, `tuple` and `std::string`) packed in 6.56 seconds on an `i7-7700HQ`
- Memory allocation could be better optimized

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
#include <string>
#include "msgpack.hpp"
int main() {

    msgpack_byte::container dest; // byte container holding the packed bytes

    // data generation

    std::tuple <char, unsigned int, double, string,
    std::vector<unsigned int>, std::map<std::string, uint64_t> > original, unpacked; // an example container to pack
    std::vector<unsigned int> vec{ 1, 2, 3, 4, 5 };
    std::string abc = "test string";
    std::map<string, uint64_t> cde;
    cde.insert(make_pair(string("abc"), 4142342342342343));
    cde.insert(make_pair(string("cde"), 5));
    cde.insert(make_pair(string("def"), 11231233));
    original = std::make_tuple('a', 10, 0.333333333333333, abc, vec, cde);

    msgpack::pack(original, dest); // pass the container to be packed and the byte container
    msgpack::unpack(unpacked, dest); // pass container to parse into and the byte container

    std::cout << msgpack_byte::to_stringstream(dest).str() << std::endl; // return a stringstream of the packed data (in hex)
    std::cout << "Packed size: " << dest.size() << std::endl;
    return 0;
}
```

### Compile time defines
Compile with different #define values to change performance
- `#define lenient_size` an integer value after which garbage collection trims extra memory for `msgpack_byte::container` default `1000`
- `#define compression_percent` a float value to with which memory preallocation is adjust (to accomodate msgpack's formatting) default `1.1`
- `#define doubling_strategy` define this without value to opt for doubling of byte container instead of growing by factor of `1.1`
