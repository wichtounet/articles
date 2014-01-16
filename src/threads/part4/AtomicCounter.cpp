#include <thread>
#include <atomic>
#include <iostream>
#include <vector>

struct AtomicCounter {
    bool flag = false;
    std::atomic<int> value;

    void increment(){
       if(!flag) {
         value = 0;
         flag = true;
       }
        ++value;
    }

    void decrement(){
        if(!flag) {
	  value = 0;
	  flag = true;
	}
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
