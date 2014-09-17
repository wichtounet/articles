//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <thread>
#include <atomic>
#include <iostream>
#include <vector>

struct AtomicCounter {
    std::atomic<int> value;

    AtomicCounter() : value(0) {}

    void increment(){
        ++value;
    }

    void decrement(){
        --value;
    }

    int get(){
        return value.load();
    }
};

int main(){
    AtomicCounter counter;

    std::vector<std::thread> threads;
    for(int i = 0; i < 10; ++i){
        threads.push_back(std::thread([&counter](){
            for(int i = 0; i < 500; ++i){
                counter.increment();
            }
        }));
    }

    for(auto& thread : threads){
        thread.join();
    }

    std::cout << counter.get() << std::endl;

    return 0;
}
