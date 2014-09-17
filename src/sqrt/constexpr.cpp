//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <cstddef>
#include <iostream>

constexpr std::size_t ct_sqrt(std::size_t n, std::size_t i = 1){
    return n == i ? n : (i * i < n ? ct_sqrt(n, i + 1) : i);
}

static constexpr const std::size_t result = ct_sqrt(SQRT_VALUE);

int main(){
    std::cout << result << std::endl;
}