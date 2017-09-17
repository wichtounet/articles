//=======================================================================
// Copyright (c) 2017 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <iostream>
#include <chrono>
#include <cmath>

using timer = std::chrono::high_resolution_clock;
using microseconds =  std::chrono::microseconds;

template<size_t It, size_t Exp>
float bench_c_pow(){
    timer::time_point t0 = timer::now();

    float result = 0;

    for(size_t i = 0; i < It; ++i){
        result += pow(float(i), int(Exp));
    }

    timer::time_point t1 = timer::now();
    auto us = std::chrono::duration_cast<microseconds>(t1 - t0);

    std::cout << "pow(x, " << Exp << "): " << us.count() << "us" << std::endl;

    return result;
}

template<size_t It, size_t Exp>
float bench_std_pow(){
    timer::time_point t0 = timer::now();

    float result = 0;

    for(size_t i = 0; i < It; ++i){
        result += std::pow(float(i), int(Exp));
    }

    timer::time_point t1 = timer::now();
    auto us = std::chrono::duration_cast<microseconds>(t1 - t0);

    std::cout << "std::pow(x, " << Exp << "): " << us.count() << "us" << std::endl;

    return result;
}

template<size_t It>
float bench_pow_2(){
    timer::time_point t0 = timer::now();

    float result = 0;

    for(size_t i = 0; i < It; ++i){
        result += float(i) * float(i);
    }

    timer::time_point t1 = timer::now();
    auto us = std::chrono::duration_cast<microseconds>(t1 - t0);

    std::cout << "x * x: " << us.count() << "us" << std::endl;

    return result;
}

template<size_t It>
float bench_pow_3(){
    timer::time_point t0 = timer::now();

    float result = 0;

    for(size_t i = 0; i < It; ++i){
        result += float(i) * float(i) * float(i);
    }

    timer::time_point t1 = timer::now();
    auto us = std::chrono::duration_cast<microseconds>(t1 - t0);

    std::cout << "x * x * x: " << us.count() << "us" << std::endl;

    return result;
}

template<size_t It>
float bench_pow_4(){
    timer::time_point t0 = timer::now();

    float result = 0;

    for(size_t i = 0; i < It; ++i){
        result += float(i) * float(i) * float(i) * float(i);
    }

    timer::time_point t1 = timer::now();
    auto us = std::chrono::duration_cast<microseconds>(t1 - t0);

    std::cout << "x * x * x * x: " << us.count() << "us" << std::endl;

    return result;
}

template<size_t It>
float bench_pow_5(){
    timer::time_point t0 = timer::now();

    float result = 0;

    for(size_t i = 0; i < It; ++i){
        result += float(i) * float(i) * float(i) * float(i) * float(i);
    }

    timer::time_point t1 = timer::now();
    auto us = std::chrono::duration_cast<microseconds>(t1 - t0);

    std::cout << "x * x * x * x * x: " << us.count() << "us" << std::endl;

    return result;
}

int main(){
    float result = 0;

    result += bench_c_pow<100, 2>();
    result += bench_c_pow<1000, 2>();
    result += bench_c_pow<10000, 2>();
    result += bench_c_pow<100000, 2>();
    result += bench_c_pow<1000000, 2>();
    result += bench_c_pow<10000000, 2>();

    result += bench_std_pow<100, 2>();
    result += bench_std_pow<1000, 2>();
    result += bench_std_pow<10000, 2>();
    result += bench_std_pow<100000, 2>();
    result += bench_std_pow<1000000, 2>();
    result += bench_std_pow<10000000, 2>();

    result += bench_pow_2<100>();
    result += bench_pow_2<1000>();
    result += bench_pow_2<10000>();
    result += bench_pow_2<100000>();
    result += bench_pow_2<1000000>();
    result += bench_pow_2<10000000>();

    result += bench_c_pow<100, 3>();
    result += bench_c_pow<1000, 3>();
    result += bench_c_pow<10000, 3>();
    result += bench_c_pow<100000, 3>();
    result += bench_c_pow<1000000, 3>();
    result += bench_c_pow<10000000, 3>();

    result += bench_std_pow<100, 3>();
    result += bench_std_pow<1000, 3>();
    result += bench_std_pow<10000, 3>();
    result += bench_std_pow<100000, 3>();
    result += bench_std_pow<1000000, 3>();
    result += bench_std_pow<10000000, 3>();

    result += bench_pow_3<100>();
    result += bench_pow_3<1000>();
    result += bench_pow_3<10000>();
    result += bench_pow_3<100000>();
    result += bench_pow_3<1000000>();
    result += bench_pow_3<10000000>();

    result += bench_c_pow<100, 4>();
    result += bench_c_pow<1000, 4>();
    result += bench_c_pow<10000, 4>();
    result += bench_c_pow<100000, 4>();
    result += bench_c_pow<1000000, 4>();
    result += bench_c_pow<10000000, 4>();

    result += bench_std_pow<100, 4>();
    result += bench_std_pow<1000, 4>();
    result += bench_std_pow<10000, 4>();
    result += bench_std_pow<100000, 4>();
    result += bench_std_pow<1000000, 4>();
    result += bench_std_pow<10000000, 4>();

    result += bench_pow_4<100>();
    result += bench_pow_4<1000>();
    result += bench_pow_4<10000>();
    result += bench_pow_4<100000>();
    result += bench_pow_4<1000000>();
    result += bench_pow_4<10000000>();

    result += bench_c_pow<100, 5>();
    result += bench_c_pow<1000, 5>();
    result += bench_c_pow<10000, 5>();
    result += bench_c_pow<100000, 5>();
    result += bench_c_pow<1000000, 5>();
    result += bench_c_pow<10000000, 5>();

    result += bench_std_pow<100, 5>();
    result += bench_std_pow<1000, 5>();
    result += bench_std_pow<10000, 5>();
    result += bench_std_pow<100000, 5>();
    result += bench_std_pow<1000000, 5>();
    result += bench_std_pow<10000000, 5>();

    result += bench_pow_5<100>();
    result += bench_pow_5<1000>();
    result += bench_pow_5<10000>();
    result += bench_pow_5<100000>();
    result += bench_pow_5<1000000>();
    result += bench_pow_5<10000000>();

    return (int) result;
}
