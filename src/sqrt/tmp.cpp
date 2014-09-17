//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <cstddef>
#include <iostream>

template <std::size_t N, std::size_t I=1>
struct ct_sqrt : std::integral_constant<std::size_t, (I*I<N) ? ct_sqrt<N,I+1>::value : I > {};

template<std::size_t N>
struct ct_sqrt<N,N> : std::integral_constant<std::size_t, N> {};

int main(){
    std::cout << ct_sqrt<SQRT_VALUE>::value << std::endl;
}