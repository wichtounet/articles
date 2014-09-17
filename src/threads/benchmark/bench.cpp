//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

#define OPERATIONS 250000
#define REPEAT 5

template<int Threads>
void bench_lock(){
    std::mutex mutex;
    unsigned long throughput = 0;

    for(int i = 0; i < REPEAT; ++i){
        std::vector<std::thread> threads;
        int counter = 0;

        Clock::time_point t0 = Clock::now();

        for(int i = 0; i < Threads; ++i){
            threads.push_back(std::thread([&](){
                for(int i = 0; i < OPERATIONS; ++i){
                    mutex.lock();
                    ++counter;
                    mutex.unlock();
                }
            }));
        }

        for(auto& thread : threads){
            thread.join();
        }

        Clock::time_point t1 = Clock::now();

        milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
        throughput += (Threads * OPERATIONS) / ms.count();
    }

    std::cout << "lock with " << Threads << " threads throughput = " << (throughput / REPEAT) << std::endl;
}

template<int Threads>
void bench_lock_guard(){
    std::mutex mutex;
    unsigned long throughput = 0;

    for(int i = 0; i < REPEAT; ++i){
        int counter = 0;

        std::vector<std::thread> threads;

        Clock::time_point t0 = Clock::now();

        for(int i = 0; i < Threads; ++i){
            threads.push_back(std::thread([&](){
                for(int i = 0; i < OPERATIONS; ++i){
                    std::lock_guard<std::mutex> guard(mutex);
                    ++counter;
                }
            }));
        }

        for(auto& thread : threads){
            thread.join();
        }

        Clock::time_point t1 = Clock::now();

        milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
        throughput += (Threads * OPERATIONS) / ms.count();
    }

    std::cout << "lock_guard with " << Threads << " threads throughput = " << (throughput / REPEAT) << std::endl;
}

template<int Threads>
void bench_atomic(){
    std::mutex mutex;
    unsigned long throughput = 0;

    for(int i = 0; i < REPEAT; ++i){
        std::atomic<int> counter;
        counter.store(0);

        std::vector<std::thread> threads;

        Clock::time_point t0 = Clock::now();

        for(int i = 0; i < Threads; ++i){
            threads.push_back(std::thread([&](){
                for(int i = 0; i < OPERATIONS; ++i){
                    ++counter;
                }
            }));
        }

        for(auto& thread : threads){
            thread.join();
        }

        Clock::time_point t1 = Clock::now();

        milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
        throughput += (Threads * OPERATIONS) / ms.count();
    }
    std::cout << "atomic with " << Threads << " threads throughput = " << (throughput / REPEAT) << std::endl;
}

#define bench(name)\
    name<1>();\
    name<2>();\
    name<4>();\
    name<8>();\
    name<16>();\
    name<32>();\
    name<64>();\
    name<128>();

int main(){
    bench(bench_lock);
    bench(bench_lock_guard);
    bench(bench_atomic);

    return 0;
}
