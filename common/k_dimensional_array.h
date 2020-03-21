//
// Created by seventh on 2020/3/5.
//

#ifndef GSEVENTH_COMMON_K_DIMENSIONAL_ARRAY_H_
#define GSEVENTH_COMMON_K_DIMENSIONAL_ARRAY_H_

#include <algorithm>
#include <cassert>
#include "template.h"

template<typename DataType, int ...>
struct KDArrayBlock;
template<typename DataType, int size>
struct KDArrayBlock<DataType, size> {
    DataType *ptr;
    DataType &operator[](const int &x) {
        return *(ptr + x);
    }
    KDArrayBlock(DataType *ptr) : ptr(ptr) {}
};
template<typename DataType, int size, int ...rest>
struct KDArrayBlock<DataType, size, rest...> {
    DataType *ptr;
    KDArrayBlock<DataType, rest...> operator[](const int &x) {
        assert(x < size);
        auto offset = x * Multiplicative<rest...>::value;
        return KDArrayBlock<DataType, rest...>(ptr + offset);
    }

    KDArrayBlock(DataType *ptr) : ptr(ptr) {}
};

template<typename DataType, int size, int ... rest>
class KDArray {
  private:
    KDArrayBlock<DataType, size, rest...> marray;
  public:
    KDArray() : marray(nullptr) {}
    ~KDArray() {
        if (marray.ptr != nullptr)
            delete (marray.ptr);
    }
    void Init() {
        if (marray.ptr != nullptr) delete (marray.ptr);
        marray.ptr = new DataType[Multiplicative<size, rest...>::value];
    }

    KDArray<DataType, rest...> operator[](const int &x) {
        return marray[x];
    }
};

template<typename DataType, int size>
struct KDArray<DataType, size> {
  private:
    KDArrayBlock<DataType, size> marray;
  public:
    KDArray() : marray(nullptr) {}
    ~KDArray() {
        if (marray.ptr != nullptr)
            delete (marray.ptr);
    }

    void Init() {
        if (marray.ptr != nullptr) delete (marray.ptr);
        marray.ptr = new DataType[size];
    }

    DataType& operator[](const int& x) {
        return marray[x];
    }
};

#endif //GSEVENTH_COMMON_K_DIMENSIONAL_ARRAY_H_
