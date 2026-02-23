#pragma once
#include "Archive.h"

class CHashArchive : public COutputArchive {

public:

    [[nodiscard]] size_t get() const { return hash; }

    void operator+=(const size_t inHash) {
#if CXX_VERSION >= 20
        hash = std::rotl(hash, std::numeric_limits<size_t>::digits / 3) ^ distribute(inHash);
#else
        hash = rotl(hash, std::numeric_limits<size_t>::digits / 3) ^ distribute(inHash);
#endif
    }

    virtual void write(const void* inValue, const size_t inElementSize, const size_t inCount) override {
        auto bytes = static_cast<const uint8_t*>(inValue);

        size_t total = inElementSize * inCount;

        while (total >= sizeof(size_t)) {
            size_t chunk;
            std::memcpy(&chunk, bytes, sizeof(size_t));

            *this += chunk;

            bytes += sizeof(size_t);
            total -= sizeof(size_t);
        }

        if (total > 0) {
            size_t remainder = 0;
            std::memcpy(&remainder, bytes, total);
            *this += remainder;
        }
    }

    [[nodiscard]] virtual bool isBinary() override {
        return false;
    }

private:

    /*
     * Thanks to Wolfgang Brehm on Stack Overflow for this hashing implementation
     */

    template<typename TType>
    constexpr TType xorshift(const TType& n, int i) noexcept {
        return n ^ (n >> i);
    }

    // a hash function with another name as to not confuse with std::hash
    /*constexpr uint32_t distribute(const uint32_t& n) noexcept {
        return 3423571495ul * xorshift(0x55555555ul * xorshift(n,16),16);
    }*/

    // a hash function with another name as to not confuse with std::hash
    constexpr size_t distribute(const size_t& n) noexcept {
        return 17316035218449499591ull * xorshift(0x5555555555555555ull * xorshift(n,32),32);
    }

#if CXX_VERSION < 20
    template <typename TType,
        std::enable_if_t<std::is_unsigned_v<TType>, int> = 0
    >
    constexpr TType rotl(const TType& n, const int rotation) noexcept {
        const TType m = (std::numeric_limits<TType>::digits - 1);
        const TType c = rotation & m;
        return (n << c) | (n >> ((TType(0) - c) & m));
    }
#endif
    
    size_t hash = 0;
};