#include <thread>
#include <mutex>

struct Complex {
    std::recursive_mutex mutex;
    int i;

    Complex() : i(0) {}

    void mul(int x){
        std::lock_guard<std::recursive_mutex> lock(mutex);
        i *= x;
    }

    void div(int x){
        std::lock_guard<std::recursive_mutex> lock(mutex);
        i /= x;
    } 

    void both(int x, int y){
        std::lock_guard<std::recursive_mutex> lock(mutex);
        mul(x);
        div(y); 
    }
};

int main(){
    Complex complex;
    complex.both(32, 23);

    return 0;
}
