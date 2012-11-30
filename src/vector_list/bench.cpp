#include <random>
#include <array>
#include <vector>
#include <list>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <deque>

#include "graphs.hpp"

//Chrono typedefs
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;
typedef std::chrono::microseconds microseconds;

static const std::size_t REPEAT = 19;

namespace {

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

bool operator>=(const Small& s1, const Small& s2){
    return s1.a >= s2.a;
}

#define CREATE(name, size)\
struct name {\
    std::size_t a = 0;\
    std::size_t b[size];\
    name(){}\
    name(std::size_t a) : a(a){}\
};\
bool operator==(const name& s1, const name& s2){ return s1.a == s2.a; }\
bool operator<(const name& s1, const name& s2){ return s1.a < s2.a; }\
bool operator>=(const name& s1, const name& s2){ return s1.a >= s2.a; }

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
    std::vector<T> container;

    for(std::size_t i = 0; i < size; ++i){
        container.insert(container.begin(), {i});
    }
}

/* Random sorted insert */

template<typename Container>
void random_sorted_insert(std::size_t size){
    Container container;

    std::mt19937 generator;
    std::uniform_int_distribution<std::size_t> distribution(0, std::numeric_limits<std::size_t>::max() - 1);

    for(std::size_t i = 0; i < size; ++i){
        typename Container::value_type v = {distribution(generator)};

        auto it = container.begin();
        auto end = container.end();

        while(it != end){
            if(*it >= v){
                break;
            }

            ++it;
        }

        container.insert(it, v);
    }
}

/* Find */

template<typename Container>
void find(std::size_t size, Container& container){
    //std::mt19937 generator;
    //std::uniform_int_distribution<std::size_t> distribution(0, size);

    for(std::size_t i = 0; i < size; ++i){
        typename Container::value_type v = {i};//distribution(generator)};
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
void remove(Container& container){
    for(std::size_t i = 0; i < 1000; ++i){
        typename Container::value_type v = {i};
        auto it = std::find(container.begin(), container.end(), v);
        container.erase(it);
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
void bench_crunching(Function function, const std::string& type){
    std::vector<std::size_t> sizes = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
    for(auto size : sizes){
        Clock::time_point t0 = Clock::now();

        //Repeat twice
        for(std::size_t i = 0; i < 2; ++i){
            function(size);
        }

        Clock::time_point t1 = Clock::now();
        milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);

        std::cout << type << ":" << size << ":" << (ms.count() / 2) << "ms" << std::endl;
    }
}

template<typename Function>
void bench_small(Function function, const std::string& type){
    std::vector<std::size_t> sizes = {100, 1000, 10000, 50000, 100000, 200000, 300000};
    for(auto size : sizes){
        Clock::time_point t0 = Clock::now();

        //Repeat twice
        for(std::size_t i = 0; i < 2; ++i){
            function(size);
        }

        Clock::time_point t1 = Clock::now();
        milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);

        std::cout << type << ":" << size << ":" << (ms.count() / 2) << "ms" << std::endl;
    }
}

template<typename Function>
void bench(Function function, const std::string& type){
    std::vector<std::size_t> sizes = {1000, 10000, 100000, 1000000};
    for(auto size : sizes){
        Clock::time_point t0 = Clock::now();

        for(std::size_t i = 0; i < REPEAT; ++i){
            function(size);
        }

        Clock::time_point t1 = Clock::now();
        milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);

        graphs::new_result(type, std::to_string(size), ms.count());
    }
}

template<typename Container, typename Function>
void bench_find(Function function, const std::string& type){
    std::vector<std::size_t> sizes = {100, 1000, 5000, 10000, 20000};
    for(auto size : sizes){
        Container container;
        
        std::mt19937 generator;
        std::uniform_int_distribution<std::size_t> distribution(0, size - 1);

        for(std::size_t i = 0; i < size; ++i){
            container.push_back({distribution(generator)});
        }

        Clock::time_point t0 = Clock::now();

        for(std::size_t i = 0; i < REPEAT; ++i){
            function(size, container);
        }

        Clock::time_point t1 = Clock::now();
        auto d = std::chrono::duration_cast<microseconds>(t1 - t0);

        std::cout << type << ":" << size << ":" << (d.count() /REPEAT) << "us" << std::endl;
    }
}

template<typename Container, typename Function>
void bench_insert(Function function, const std::string& type){
    std::vector<std::size_t> sizes = {1000, 2000, 4000, 6000, 8000, 10000};
    for(auto size : sizes){
        std::vector<typename Container::value_type> temp;

        for(std::size_t i = 0; i < size; ++i){
            temp.push_back({i});
        }

        std::random_shuffle(temp.begin(), temp.end());

        Container container(temp.begin(), temp.end());

        Clock::time_point t0 = Clock::now();

        for(std::size_t i = 0; i < REPEAT; ++i){
            function(size, container);
        }

        Clock::time_point t1 = Clock::now();
        auto d = std::chrono::duration_cast<milliseconds>(t1 - t0);

        std::cout << type << ":" << size << ":" << (d.count() /REPEAT) << "ms" << std::endl;
    }
}

template<typename Container, typename Function>
void bench_remove(Function function, const std::string& type){
    std::vector<std::size_t> sizes = {1000, 2000, 4000, 6000, 8000, 10000};
    for(auto size : sizes){
        std::vector<typename Container::value_type> temp;

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

template<typename Container, typename Function>
void bench_sort(Function function, const std::string& type){
    std::vector<std::size_t> sizes = {1000, 10000, 100000, 1000000/*, 10000000*/};
    for(auto size : sizes){
        std::vector<typename Container::value_type> temp;

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
            auto ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
            ms_tot += ms;
        }

        std::cout << type << ":" << size << ":" << ms_tot.count() << "ms" << std::endl;
    }
}

template<typename Container>
void bench_destruction(const std::string& type){
    std::vector<std::size_t> sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
    for(auto size : sizes){
        microseconds us_tot;

        for(std::size_t i = 0; i < REPEAT; ++i){
            Container* container = new Container();

            for(std::size_t i = 0; i < size; ++i){
                container->push_back(i);
            }

            Clock::time_point t0 = Clock::now();
            delete container;
            Clock::time_point t1 = Clock::now();
            
            auto us = std::chrono::duration_cast<microseconds>(t1 - t0);
            us_tot += us;
        }
    
        std::cout << type << ":" << size << ":" << (us_tot.count() / REPEAT) << "us" << std::endl;
    }
}

template<typename T>
void bench(){
    std::cout << "Bench " << sizeof(T) << std::endl;
    std::string size_str = std::to_string(sizeof(T));

    graphs::new_graph("fill_back_" + size_str, "fill_back - "  + size_str + " byte", "ms");

    bench(fill_back_vector<T>, "vector_pre");
    bench(fill_back<std::vector<T>>, "vector");
    bench(fill_back<std::list<T>>, "list");
    bench(fill_back<std::deque<T>>, "deque");
    
    /*std::cout << "Fill front" << std::endl;
    bench_small(fill_front_vector<T>, "vector");
    bench_small(fill_front_list<T>, "list");
    
    std::cout << "Find all" << std::endl;
    bench_find<std::vector<T>>(find<std::vector<T>>, "vector");
    bench_find<std::list<T>>(find<std::list<T>>, "list");
    
    std::cout << "Insert" << std::endl;
    bench_insert<std::vector<T>>(insert<std::vector<T>>, "vector");
    bench_insert<std::list<T>>(insert<std::list<T>>, "list");
    
    std::cout << "Remove" << std::endl;
    bench_remove<std::vector<T>>(remove<std::vector<T>>, "vector");
    bench_remove<std::list<T>>(remove<std::list<T>>, "list");
    
    std::cout << "Sort" << std::endl;
    bench_sort<std::vector<T>>(sort_vector<T>, "vector");
    bench_sort<std::list<T>>(sort_list<T>, "list");
    
    std::cout << "Destruction" << std::endl;
    bench_destruction<std::vector<T>>("vector");
    bench_destruction<std::list<T>>("list");
    bench_destruction<std::deque<T>>("deque");*/
}

} //end of anonymous namespace

int main(){
    bench<Small>();
    /*bench<Medium>();
    bench<Large>();
    bench<Huge>();

    std::cout << "Random Sorted Insert" << std::endl;
    bench_crunching(random_sorted_insert<std::vector<Small>>, "vector");
    bench_crunching(random_sorted_insert<std::list<Small>>, "list");
    bench_crunching(random_sorted_insert<std::deque<Small>>, "deque");*/

    graphs::output(graphs::Output::GOOGLE);

    return 0;
}
