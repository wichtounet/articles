//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <thread>
#include <iostream>
#include <vector>
#include <mutex>

struct Counter {
    int value;

    Counter() : value(0) {}

    void increment(){
        ++value;
    }

    void decrement(){
        if(value == 0){
            throw "Value cannot be less than 0";
        }

        --value;
    }
};

struct ConcurrentSafeCounter {
    std::mutex mutex;
    Counter counter;

    void increment(){
        std::lock_guard<std::mutex> guard(mutex);
        counter.increment();
    }

    void decrement(){
        std::lock_guard<std::mutex> guard(mutex);
        counter.decrement();
        //mutex.unlock();
    }
};

int main(){
    Counter counter;

    std::vector<std::thread> threads;

    for(int i = 0; i < 5; ++i){
        threads.push_back(std::thread([&](){
            for(int i = 0; i < 100; ++i){
                counter.increment();
            }
        }));
    }

    for(auto& thread : threads){
        thread.join();
    }

    std::cout << counter.value << std::endl;

    return 0;
}
