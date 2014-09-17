//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <thread>
#include <chrono>
#include <iostream>
#include <mutex>

std::once_flag flag;

void do_something(){
    std::call_once(flag, [](){std::cout << "Called once" << std::endl;});

    std::cout << "Called each time" << std::endl;
}

int main(){
    std::thread t1(do_something);
    std::thread t2(do_something);
    std::thread t3(do_something);
    std::thread t4(do_something);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}
