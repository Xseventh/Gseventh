//
// Created by seventh on 2020/2/17.
//

#ifndef GSEVENTH_COMMON_BITSET_H_
#define GSEVENTH_COMMON_BITSET_H_
#include <cstdint>
#include <cstring>
#include <iostream>
namespace common {
template<uint32_t Size>
class BitSet {
  protected:
    uint32_t marray[Size / 32 + (Size % 32 != 0)];
  public:
    BitSet() {
        memset(marray, 0, sizeof(marray));
    }
    inline void set(uint32_t x) { marray[x >> 5] |= 1U << (x & 31); }
    inline void reset(uint32_t x) { marray[x >> 5] &= ~(1U << (x & 31)); }
    inline void flip(uint32_t x) { marray[x >> 5] ^= 1U << (x & 31); }
    inline bool get(uint32_t x) const { return marray[x >> 5] >> (x & 31) & 1; }
};
template<uint32_t Size>
class Bit2Set : protected BitSet<Size * 2> {
  public:
    void set0(uint32_t x) {
        BitSet<Size * 2>::reset(x << 1);
        BitSet<Size * 2>::reset(x << 1 | 1);
    }
    inline void set1(uint32_t x) {
        BitSet<Size * 2>::set(x << 1);
        BitSet<Size * 2>::reset(x << 1 | 1);
    }
    inline void set2(uint32_t x) {
        BitSet<Size * 2>::reset(x << 1);
        BitSet<Size * 2>::set(x << 1 | 1);
    }
    inline void set3(uint32_t x) {
        BitSet<Size * 2>::set(x << 1);
        BitSet<Size * 2>::set(x << 1 | 1);
    }
    const uint32_t operator[](uint32_t x) const {
        return BitSet<Size * 2>::marray[x >> 4] >> ((x & 15) << 1) & 3;
    }
    const uint32_t gethash(uint32_t pos) const {
        return BitSet<Size * 2>::marray[pos];
    }
};
}

#endif //GSEVENTH_COMMON_BITSET_H_
