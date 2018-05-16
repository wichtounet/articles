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

double my_pow(double x, size_t n){
    double r = 1.0;

    while(n > 0){
        r *= x;
        --n;
    }

    return r;
}

double second_pow(double x, size_t n){
    double r = 1.0;
    double tmp = 1.0;
    int curr = 1;

    while(n){
        tmp = tmp * x;
        if (n & curr){
            r *= tmp;
            n = n & ~curr;
        }

        ++curr;
    }

    return r;
}

double third_pow(double x, size_t y) {
    double z = 1;
    double base = x;

    while (y) {
        if (y & 1) {
            z *= base;
        }
        y >>= 1;
        base *= base;
    }

    return z;
}

template<size_t It, size_t Exp>
double bench_my_pow(){
    timer::time_point t0 = timer::now();

    double result = 0;

    for(size_t i = 0; i < It; ++i){
        result += my_pow(double(i), int(Exp));
    }

    timer::time_point t1 = timer::now();
    auto us = std::chrono::duration_cast<microseconds>(t1 - t0);

    std::cout << "my_pow(x, " << Exp << "): " << us.count() << "us" << std::endl;

    return result;
}

template<size_t It, size_t Exp>
double bench_second_pow(){
    timer::time_point t0 = timer::now();

    double result = 0;

    for(size_t i = 0; i < It; ++i){
        result += second_pow(double(i), int(Exp));
    }

    timer::time_point t1 = timer::now();
    auto us = std::chrono::duration_cast<microseconds>(t1 - t0);

    std::cout << "second_pow(x, " << Exp << "): " << us.count() << "us" << std::endl;

    return result;
}

template<size_t It, size_t Exp>
double bench_third_pow(){
    timer::time_point t0 = timer::now();

    double result = 0;

    for(size_t i = 0; i < It; ++i){
        result += third_pow(double(i), int(Exp));
    }

    timer::time_point t1 = timer::now();
    auto us = std::chrono::duration_cast<microseconds>(t1 - t0);

    std::cout << "third_pow(x, " << Exp << "): " << us.count() << "us" << std::endl;

    return result;
}


template<size_t It, size_t Exp>
double bench_std_pow(){
    timer::time_point t0 = timer::now();

    double result = 0;

    for(size_t i = 0; i < It; ++i){
        result += std::pow(double(i), int(Exp));
    }

    timer::time_point t1 = timer::now();
    auto us = std::chrono::duration_cast<microseconds>(t1 - t0);

    std::cout << "std::pow(x, " << Exp << "): " << us.count() << "us" << std::endl;

    return result;
}

template<size_t It, size_t Exp>
double bench_pow(){
    double result = 0;

    result += bench_my_pow<It, Exp>();
    result += bench_second_pow<It, Exp>();
    result += bench_third_pow<It, Exp>();
    result += bench_std_pow<It, Exp>();

    return result;
}

int main(){
    double result = 0;

    result += bench_pow<5000, 1>();
    result += bench_pow<5000, 5>();
    result += bench_pow<5000, 10>();
    result += bench_pow<5000, 20>();
    result += bench_pow<5000, 30>();
    result += bench_pow<5000, 40>();
    result += bench_pow<5000, 50>();
    result += bench_pow<5000, 60>();
    result += bench_pow<5000, 70>();
    result += bench_pow<5000, 80>();
    result += bench_pow<5000, 90>();
    result += bench_pow<5000, 100>();
    result += bench_pow<5000, 110>();
    result += bench_pow<5000, 120>();
    result += bench_pow<5000, 130>();
    result += bench_pow<5000, 140>();
    result += bench_pow<5000, 150>();
    result += bench_pow<5000, 160>();
    result += bench_pow<5000, 170>();
    result += bench_pow<5000, 180>();
    result += bench_pow<5000, 190>();
    result += bench_pow<5000, 200>();
    result += bench_pow<5000, 300>();
    result += bench_pow<5000, 400>();
    result += bench_pow<5000, 500>();
    result += bench_pow<5000, 600>();
    result += bench_pow<5000, 700>();
    result += bench_pow<5000, 800>();
    result += bench_pow<5000, 900>();
    result += bench_pow<5000, 1000>();

    return (int) result;
}
