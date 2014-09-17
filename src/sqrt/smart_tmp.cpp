//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

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