#include <cstddef>
#include <iostream>

#define MID(a, b) ((a+b)/2)
#define POW(a) (a*a)

template<std::size_t res, std::size_t l = 1, std::size_t r = res>
struct ct_sqrt;

template<std::size_t res, std::size_t r>
struct ct_sqrt<res, r, r> : std::integral_constant<std::size_t, r> {};

template <std::size_t res, std::size_t l, std::size_t r>
struct ct_sqrt : std::integral_constant<std::size_t, ct_sqrt<res, (POW(MID(r, l)) >= res ? l : MID(r, l)+1), (POW(MID(r, l)) >= res ? MID(r, l) : r)>::value> {};

int main(){
    std::cout << ct_sqrt<SQRT_VALUE>::value << std::endl;
}