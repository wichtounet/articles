//=======================================================================
// Copyright (c) 2017 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <thread>
#include <future>
#include <iostream>

int main(){
    auto future = std::async(std::launch::async, [](){
        std::cout << "I'm a thread" << std::endl;
    });

    future.get();

    return 0;
}
