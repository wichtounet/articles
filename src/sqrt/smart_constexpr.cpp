//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <cstddef>
#include <iostream>

static constexpr std::size_t ct_mid(std::size_t a, std::size_t b){
    return (a+b) / 2;
}

static constexpr std::size_t ct_pow(std::size_t a){
    return a*a;
}

static constexpr std::size_t ct_sqrt(std::size_t res, std::size_t l, std::size_t r){
    return
        l == r ? r
        : ct_sqrt(res, ct_pow(ct_mid(r, l)) >= res ? l : ct_mid(r, l) + 1, ct_pow(ct_mid(r, l)) >= res ? ct_mid(r, l) : r);
}

static constexpr std::size_t ct_sqrt(std::size_t res){
    return ct_sqrt(res, 1, res);
}

static constexpr std::size_t ct_sqrt_14(std::size_t res, std::size_t l, std::size_t r){
    if(l == r){
        return r;
    } else {
        const auto mid = (r + l) / 2;

        if(mid * mid >= res){
            return ct_sqrt(res, l, mid);
        } else {
            return ct_sqrt(res, mid + 1, r);
        }
    }
}

static constexpr const std::size_t result = ct_sqrt(SQRT_VALUE);

int main(){
    std::cout << result << std::endl;
}