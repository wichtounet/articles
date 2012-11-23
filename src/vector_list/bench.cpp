#include <random>
#include <array>
#include <vector>
#include <list>
#include <iostream>
#include <algorithm>
#include <chrono>

//Chrono typedefs
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

static const std::size_t REPEAT = 1;

struct Small {
    std::size_t a = 0;

    Small(){}
    Small(std::size_t a) : a(a){}
};

bool operator==(const Small& s1, const Small& s2){
    return s1.a == s2.a;
}

bool operator<(const Small& s1, const Small& s2){
    return s1.a < s2.a;
}

#define CREATE(name, size)\
struct name {\
    std::size_t a = 0;\
    std::size_t b[size];\
    name(){}\
    name(std::size_t a) : a(a){}\
};\
bool operator==(const name& s1, const name& s2){ return s1.a == s2.a; }\
bool operator<(const name& s1, const name& s2){ return s1.a < s2.a; }

CREATE(Medium, 3) //4*8B = 32B
CREATE(Large, 15) //16*8B = 128B
CREATE(Huge, 127) //128*8B = 1KB

/* Fill back */

template<typename Container>
void fill_back(std::size_t size){
    Container container;

    for(std::size_t i = 0; i < size; ++i){
        container.push_back({i});
    }
}

template<typename T>
void fill_back_vector(std::size_t size){
    std::vector<T> container(size);

    for(std::size_t i = 0; i < size; ++i){
        container[i] = {i};
    }
}

/* Fill front  */

template<typename T>
void fill_front_list(std::size_t size){
    std::list<T> container;

    for(std::size_t i = 0; i < size; ++i){
        container.push_front({i});
    }
}

template<typename T>
void fill_front_vector(std::size_t size){
    std::list<T> container;

    for(std::size_t i = 0; i < size; ++i){
        container.insert(container.begin(), {i});
    }
}

/* Find */

template<typename Container>
void find(std::size_t size, Container& container){
    std::mt19937 generator;
    std::uniform_int_distribution<std::size_t> distribution(0, size);

    for(std::size_t i = 0; i < 1000; ++i){
        typename Container::value_type v = {distribution(generator)};
        std::find(container.begin(), container.end(), v);
    }
}

/* Insert */

template<typename Container>
void insert(std::size_t size, Container& container){
    std::mt19937 generator;
    std::uniform_int_distribution<std::size_t> distribution(0, size - 1);

    for(std::size_t i = 0; i < 1000; ++i){
        typename Container::value_type v = {distribution(generator)};
        auto it = std::find(container.begin(), container.end(), v);
        container.insert(it, {size + i});
    }
}

/* Remove */

template<typename Container>
void remove(std::size_t size, Container& container){
    std::mt19937 generator;
    std::uniform_int_distribution<std::size_t> distribution(0, size - 1);

    for(std::size_t i = 0; i < 1000; ++i){
        typename Container::value_type v = {distribution(generator)};
        auto it = std::find(container.begin(), container.end(), v);
        if(it != container.end()){
            container.erase(it);
        }
    }
}

/* Sort */

template<typename T>
void sort_vector(std::vector<T>& container){
    std::sort(container.begin(), container.end());
}

template<typename T>
void sort_list(std::list<T>& container){
    container.sort();
}

/* Bench functions */

template<typename Function>
void bench(Function function, const std::string& type){
    std::vector<std::size_t> sizes = {1000, 10000, 100000, 1000000/*, 10000000*/};
    for(auto size : sizes){
        Clock::time_point t0 = Clock::now();

        for(std::size_t i = 0; i < REPEAT; ++i){
            function(size);
        }

        Clock::time_point t1 = Clock::now();
        milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);

        std::cout << type << ":" << size << ":" << ms.count() << "ms" << std::endl;
    }
}

template<typename Container, typename Function>
void bench_pre(Function function, const std::string& type){
    std::vector<std::size_t> sizes = {1000, 10000, 100000, 1000000/*, 10000000*/};
    for(auto size : sizes){
        Container container;

        for(std::size_t i = 0; i < size; ++i){
            container.push_back({i});
        }

        Clock::time_point t0 = Clock::now();

        for(std::size_t i = 0; i < REPEAT; ++i){
            function(size, container);
        }

        Clock::time_point t1 = Clock::now();
        milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);

        std::cout << type << ":" << size << ":" << ms.count() << "ms" << std::endl;
    }
}

template<typename Container, typename Function>
void bench_sort(Function function, const std::string& type){
    std::vector<std::size_t> sizes = {1000, 10000, 100000, 1000000/*, 10000000*/};
    for(auto size : sizes){
        std::vector<typename Container::value_type> temp(size);

        for(std::size_t i = 0; i < size; ++i){
            temp.push_back({i});
        }

        std::random_shuffle(temp.begin(), temp.end());

        milliseconds ms_tot;

        Container container;

        for(std::size_t i = 0; i < REPEAT; ++i){
            container.clear();
            container.insert(container.begin(), temp.begin(), temp.end());

            Clock::time_point t0 = Clock::now();
            function(container);
            Clock::time_point t1 = Clock::now();
            milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
            ms_tot += ms;
        }

        std::cout << type << ":" << size << ":" << ms_tot.count() << "ms" << std::endl;
    }
}

template<typename T>
void bench(){
    std::cout << "Bench " << sizeof(T) << std::endl;

    std::cout << "Fill back" << std::endl;
    bench(fill_back_vector<T>, "vector_pre");
    bench(fill_back<std::vector<T>>, "vector");
    bench(fill_back<std::list<T>>, "list");
    
    std::cout << "Fill front" << std::endl;
    bench(fill_front_vector<T>, "vector");
    bench(fill_front_list<T>, "list");
    
    std::cout << "Find all" << std::endl;
    bench_pre<std::vector<T>>(find<std::vector<T>>, "vector");
    bench_pre<std::list<T>>(find<std::list<T>>, "list");
    
    std::cout << "Insert" << std::endl;
    bench_pre<std::vector<T>>(insert<std::vector<T>>, "vector");
    bench_pre<std::list<T>>(insert<std::list<T>>, "list");
    
    std::cout << "Remove" << std::endl;
    bench_pre<std::vector<T>>(remove<std::vector<T>>, "vector");
    bench_pre<std::list<T>>(remove<std::list<T>>, "list");
    
    std::cout << "Sort" << std::endl;
    bench_sort<std::vector<T>>(sort_vector<T>, "vector");
    bench_sort<std::list<T>>(sort_list<T>, "list");
}

int main(){
    bench<Small>();
    bench<Medium>();
    bench<Large>();
    bench<Huge>();

    return 0;
}
