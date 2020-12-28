#include <cstdint>

#define umaxfixint 0x7F
#define umax8 0xFF
#define umax16 0xFFFF
#define umax32 0XFFFFFFFF
#define umax64 0xFFFFFFFFFFFFFFFF
#define maxfixint 0x20
#define max8 0x80
#define max16 0x8000
#define max32 0x80000000
#define max64 0x8000000000000000
#define fix32 0X1F
#define single_char 0xA1
#define lenient_size 0x3E8
#define compression_percent 0.4

#define ufixint 0x00
#define fixmap 0x80
#define fixarray 0x90
#define fixstr 0xA0
#define nil 0xC0
#define flse 0xC2
#define tru 0xC3
#define bin8 0xC4
#define bin16 0xC5
#define bin32 0xC6
#define ext8 0xC7
#define ext16 0xC8
#define ext32 0xC9
#define float32 0xCA
#define float64 0xCB
#define uint8 0xCC
#define uint16 0xCD
#define uint32 0xCE
#define uint64 0xCF
#define int8 0xD0
#define int16 0xD1
#define int32 0xD2
#define int64 0xD3
#define fixext1 0xD4
#define fixext2 0xD5
#define fixext4 0xD6
#define fixext8 0xD7
#define fixext16 0xD8
#define str8 0xD9
#define str16 0xDA
#define str32 0xDB
#define arr16 0xDC
#define arr32 0xDD
#define map16 0xDE
#define map32 0xDF
#define fixint 0xFF

template<typename T>
static inline uint8_t ufixint_t(T n) {
	return uint8_t(n);
}

template<typename T>
static inline uint8_t fixint_t(T n) {
	return uint8_t(fixint - n + 1);
}

static inline uint8_t fixmap_t(size_t n) {
	return uint8_t(fixmap + n);
}

static inline uint8_t fixarray_t(size_t n) {
	return uint8_t(fixarray + n);
}
static inline uint8_t fixstr_t(size_t n) {
	return uint8_t(fixstr + n);
}