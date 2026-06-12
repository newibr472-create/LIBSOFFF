/*
 * core/xstr.h — Compile-time XOR string encryption
 * Strings are encrypted at compile time and decrypted at runtime on stack
 * Prevents static string analysis by AC
 */
#pragma once
#include <cstdint>
#include <cstring>

namespace xstr_detail {

constexpr uint8_t key_byte(int i, uint64_t seed) {
    uint64_t h = seed ^ (i * 0x45D9F3B37A82C4EF);
    h = ((h >> 16) ^ h) * 0x45D9F3B;
    h = ((h >> 16) ^ h) * 0x45D9F3B;
    h = (h >> 16) ^ h;
    return (uint8_t)(h & 0xFF);
}

template <int N, uint64_t Seed>
struct Encrypted {
    char data[N];
    
    constexpr Encrypted(const char (&str)[N]) : data{} {
        for (int i = 0; i < N; i++) {
            data[i] = str[i] ^ key_byte(i, Seed);
        }
    }
    
    void decrypt(char *out) const {
        for (int i = 0; i < N; i++) {
            out[i] = data[i] ^ key_byte(i, Seed);
        }
    }
};

template <int N, uint64_t Seed>
struct Decrypted {
    char buf[N];
    Decrypted(const Encrypted<N, Seed> &enc) { enc.decrypt(buf); }
    operator const char *() const { return buf; }
};

} // namespace xstr_detail

// Usage: XS("libUE4.so") → decrypted on stack, zero in .rodata
#define XS_SEED (__LINE__ * 7919ULL + 0xDEADBEEFCAFE)
#define XS(str) \
    ([]() -> const char * { \
        static constexpr xstr_detail::Encrypted<sizeof(str), XS_SEED> _enc(str); \
        xstr_detail::Decrypted<sizeof(str), XS_SEED> _dec(_enc); \
        return _dec.buf; \
    }())
