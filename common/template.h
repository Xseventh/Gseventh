//
// Created by seventh on 2020/3/5.
//

#ifndef GSEVENTH_COMMON_TEMPLATE_H_
#define GSEVENTH_COMMON_TEMPLATE_H_

template<int ...Array>
struct Multiplicative;
template<int First, int... Array>
struct Multiplicative<First, Array...> {
    enum { value = First * Multiplicative<Array...>::value };
};
template<int First>
struct Multiplicative<First> {
    enum { value = First };
};

template<int ...Array>
struct Count;
template<int First, int... Array>
struct Count<First, Array...> {
    enum { value = Count<Array...>::value + 1 };
};
template<int First>
struct Count<First> {
    enum { value = 1 };
};

#endif //GSEVENTH_COMMON_TEMPLATE_H_
