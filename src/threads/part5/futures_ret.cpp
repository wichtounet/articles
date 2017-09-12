//=======================================================================
// Copyright (c) 2017 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <thread>
#include <future>
#include <iostream>
#include <chrono>

int main(){
    auto future = std::async(std::launch::async, [](){
        std::this_thread::sleep_for(std::chrono::seconds(5));
        return 42;
    });

    future.wait();
    future.wait();
    future.wait();

    std::cout << future.get() << std::endl;

    return 0;
}
