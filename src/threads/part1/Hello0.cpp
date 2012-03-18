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
