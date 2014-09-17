//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <thread>
#include <iostream>

void hello(){
    std::cout << "Hello from thread " << std::endl;
}

int main(){
    std::thread t1(hello);
    t1.join();

    return 0;
}
