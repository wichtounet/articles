//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <random>
#include <array>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>

//Chrono typedefs
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

static const std::size_t SIZE = 5000000;
static const std::size_t MAX =  SIZE * 10;
static const std::size_t REPEAT = 25;

static const bool DISPLAY = false;

void fill_random(std::vector<std::size_t>& vec, std::size_t size){
    std::mt19937 generator;
    std::uniform_int_distribution<std::size_t> distribution(0, MAX);

    for(std::size_t i = 0; i < size; ++i){
        vec.push_back(distribution(generator));
    }
}

void display_vec(std::vector<std::size_t>& vec){
    if(vec.size() > 0){
        std::cout << vec[0];

        for(std::size_t i = 1; i < vec.size(); ++i){
            std::cout << ", " << vec[i];
        }

        std::cout << std::endl;
    } else {
        std::cout << "empty" << std::endl;
    }
}

void std_sort(std::vector<std::size_t>& A){
    std::sort(A.begin(), A.end());
}

void in_place_counting_sort(std::vector<std::size_t>& A){
    std::vector<std::size_t> C(MAX + 1);

    for (std::size_t i = 0; i < SIZE; ++i){
        ++C[A[i]];
    }

    int current = 0;
    for (std::size_t i = 0; i < MAX; ++i){
        for(std::size_t j =0; j < C[i]; ++j){
            A[current++] = i;
        }
    }
}

void counting_sort(std::vector<std::size_t>& A){
    std::vector<std::size_t> B(SIZE);
    std::vector<std::size_t> C(MAX);

    for (std::size_t i = 0; i < SIZE; ++i){
        ++C[A[i]];
    }

    for (std::size_t i = 1; i <= MAX; ++i){
        C[i] += C[i - 1];
    }

    for (long i = SIZE - 1; i >= 0; --i) {
        B[C[A[i]] - 1] = A[i];
        --C[A[i]];
    }

    for (std::size_t i = 0; i < SIZE; ++i){
        A[i] = B[i];
    }
}

void binsort(std::vector<std::size_t>& A){
    std::vector<std::vector<std::size_t>> B(MAX + 1);

    for(std::size_t i = 0; i < SIZE; ++i){
        B[A[i]].push_back(A[i]);
    }

    std::size_t current = 0;
    for(std::size_t i = 0; i < MAX; ++i){
        for(auto item : B[i]){
            A[current++] = item;
        }
    }
}

//For radix sort
static const std::size_t digits = 2;        //Digits
static const std::size_t r = 16;            //Bits
static const std::size_t radix = 1 << r;    //Bins
static const std::size_t mask = radix - 1;

void radix_sort(std::vector<std::size_t>& A){
    std::vector<std::size_t> B(SIZE);
    std::vector<std::size_t> cnt(radix);

    for(std::size_t i = 0, shift = 0; i < digits; i++, shift += r){
        for(std::size_t j = 0; j < radix; ++j){
            cnt[j] = 0;
        }

        for(std::size_t j = 0; j < SIZE; ++j){
            ++cnt[(A[j] >> shift) & mask];
        }

        for(std::size_t j = 1; j < radix; ++j){
            cnt[j] += cnt[j - 1];
        }

        for(long j = SIZE - 1; j >= 0; --j){
            B[--cnt[(A[j] >> shift) & mask]] = A[j];
        }

        for(std::size_t j = 0; j < SIZE; ++j){
           A[j] = B[j];
        }
    }
}

template<typename Function>
void bench(Function sort_function){
    std::array<std::vector<std::size_t>, REPEAT> vec;

    for(std::size_t i = 0; i < REPEAT; ++i){
        fill_random(vec[i], SIZE);
    }

    Clock::time_point t0 = Clock::now();

    for(std::size_t i = 0; i < REPEAT; ++i){
        sort_function(vec[i]);
    }

    Clock::time_point t1 = Clock::now();
    milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);

    std::cout << ms.count() << "ms" << std::endl;

    if(DISPLAY){
        display_vec(vec[0]);
    }
}

int main(){
    std::cout << "std::sort" << std::endl;
    bench(&std_sort);

    std::cout << "counting_sort" << std::endl;
    bench(&counting_sort);

    std::cout << "in_place_counting_sort" << std::endl;
    bench(&in_place_counting_sort);

    std::cout << "binsort" << std::endl;
    bench(&::binsort);

    std::cout << "radix_sort" << std::endl;
    bench(&radix_sort);

    return 0;
}
