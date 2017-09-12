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
    auto f1 = std::async(std::launch::async, [](){
        std::this_thread::sleep_for(std::chrono::seconds(9));
        return 42;
    });

    auto f2 = std::async(std::launch::async, [](){
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return 13;
    });

    auto f3 = std::async(std::launch::async, [](){
        std::this_thread::sleep_for(std::chrono::seconds(6));
        return 666;
    });

    auto timeout = std::chrono::milliseconds(10);

    while(f1.valid() || f2.valid() || f3.valid()){
        if(f1.valid() && f1.wait_for(timeout) == std::future_status::ready){
            std::cout << "Task1 is done! " << f1.get() << std::endl;
        }

        if(f2.valid() && f2.wait_for(timeout) == std::future_status::ready){
            std::cout << "Task2 is done! " << f2.get() << std::endl;
        }

        if(f3.valid() && f3.wait_for(timeout) == std::future_status::ready){
            std::cout << "Task3 is done! " << f3.get() << std::endl;
        }

        std::cout << "I'm doing my own work!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "I'm done with my own work!" << std::endl;
    }

    std::cout << "Everything is done, let's go back to the tutorial" << std::endl;

    return 0;
}
