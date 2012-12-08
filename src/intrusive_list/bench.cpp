#include <random>
#include <array>
#include <vector>
#include <list>
#include <algorithm>
#include <deque>
#include <thread>
#include <iostream>
#include <chrono>

#include "graphs.hpp"
#include <boost/intrusive/list.hpp>

//Chrono typedefs
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;
typedef std::chrono::microseconds microseconds;

static const std::size_t REPEAT = 7;

namespace {

struct NonTrivial {
    std::size_t a = 0;
    std::size_t b = 0;

    NonTrivial(){}
    NonTrivial(std::size_t a) : a(a) {}

    NonTrivial& operator=(const NonTrivial& rhs){
        double x = sqrt(b * (rhs.a / 3.14 + 73.44 * b / a) / a);
        double y = sqrt(a * (rhs.b / 73.2198 * rhs.b * rhs.a) / b);

        a = sqrt((x * tanh(x) * log10(x)) / y);
        b = sqrt((y * y * cos(y) * y) / (sin(x) * x));

        a = rhs.a;
        b = rhs.b;

        return *this;
    }

    NonTrivial(const NonTrivial& rhs){
        double x = sqrt(b * (rhs.a / 3.14 + 73.44 * b / a) / a);
        double y = sqrt(a * (rhs.b / 73.2198 * rhs.b * rhs.a) / b);

        a = sqrt((x * tanh(x) * log10(x)) / y);
        b = sqrt((y * y * cos(y) * y) / (sin(x) * x));

        a = rhs.a;
        b = rhs.b;
    }
};

bool operator==(const NonTrivial& s1, const NonTrivial& s2){
    return s1.a == s2.a;
}

bool operator<(const NonTrivial& s1, const NonTrivial& s2){
    return s1.a < s2.a;
}

bool operator>=(const NonTrivial& s1, const NonTrivial& s2){
    return s1.a >= s2.a;
}

template<unsigned int Size> 
struct filler {
    int dummy[Size];
};

template <>
struct filler<0> {
    //Nothing
};

template<unsigned int Size> 
struct Normal : private filler<Size> {
    std::size_t a = 0;

    Normal(){}
    Normal(std::size_t a) : a(a){}

    friend bool operator==(const Normal<Size>& s1, const Normal<Size>& s2){ return s1.a == s2.a; }
    friend bool operator<(const Normal<Size>& s1, const Normal<Size>& s2){ return s1.a < s2.a; }
    friend bool operator>=(const Normal<Size>& s1, const Normal<Size>& s2){ return s1.a >= s2.a; }
};

template<unsigned int Size, boost::intrusive::link_mode_type LinkMode>
struct Intrusive : public boost::intrusive::list_base_hook<boost::intrusive::link_mode<LinkMode>>, public Normal<Size> {
    Intrusive(){}
    Intrusive(std::size_t a) : Normal<Size>(a) {}
};

/* Fill back */

template<typename Container>
void bench_fill_back_standard(const std::string& type){
    std::vector<std::size_t> sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
    for(auto size : sizes){
        Container container;

        microseconds us_tot;

        for(std::size_t i = 0; i < REPEAT; ++i){
            Clock::time_point t0 = Clock::now();
            for(std::size_t j = 0; j < size; ++j){
                container.push_back({j});
            }
            Clock::time_point t1 = Clock::now();
            auto duration = std::chrono::duration_cast<microseconds>(t1 - t0);
            us_tot += duration;

            //For the next iteration
            container.clear();
        }

        graphs::new_result(type, std::to_string(size), us_tot.count() / REPEAT);
    }
}

template<typename Container>
void bench_fill_back_intrusive(const std::string& type){
    std::vector<std::size_t> sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
    for(auto size : sizes){
        std::vector<typename Container::value_type> objects;
        objects.reserve(size);
        Container container;

        microseconds us_tot;

        for(std::size_t i = 0; i < REPEAT; ++i){
            Clock::time_point t0 = Clock::now();
            
            for(std::size_t j = 0; j < size; ++j){
                objects.push_back({j});
                container.push_back(objects[j]);
            }

            Clock::time_point t1 = Clock::now();
            auto duration = std::chrono::duration_cast<microseconds>(t1 - t0);
            us_tot += duration;

            //For the next iteration
            container.clear();
            objects.clear();
        }

        graphs::new_result(type, std::to_string(size), us_tot.count() / REPEAT);
    }
}

/* Fill front  */

template<typename T>
inline void fill_front_list(std::size_t size, std::list<T>& container){
    for(std::size_t i = 0; i < size; ++i){
        container.push_front({i});
    }
}

template<typename T>
inline void fill_front_deque(std::size_t size, std::deque<T>& container){
    for(std::size_t i = 0; i < size; ++i){
        container.push_front({i});
    }
}

template<typename T>
inline void fill_front_vector(std::size_t size, std::vector<T>& container){
    for(std::size_t i = 0; i < size; ++i){
        container.insert(container.begin(), {i});
    }
}

template<typename Container, typename Function>
void bench_fill_front(Function function, const std::string& type){
    std::vector<std::size_t> sizes = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
    for(auto size : sizes){
        Container container;

        milliseconds ms_tot;

        //Repeat twice
        for(std::size_t i = 0; i < 2; ++i){
            Clock::time_point t0 = Clock::now();
            function(size, container);
            Clock::time_point t1 = Clock::now();
            milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
            ms_tot += ms;

            container.clear();
        }

        graphs::new_result(type, std::to_string(size), ms_tot.count() / 2);
    }
}

/* Number crunching */

template<typename Container>
inline void random_sorted_insert(std::size_t size, Container& container){
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

template<typename Container>
void bench_number_crunching(const std::string& type){
    std::vector<std::size_t> sizes = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
    for(auto size : sizes){
        Container container;

        milliseconds ms_tot;

        //Repeat twice
        for(std::size_t i = 0; i < 2; ++i){
            Clock::time_point t0 = Clock::now();
            
            random_sorted_insert(size, container); 
            
            Clock::time_point t1 = Clock::now();
            milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
            ms_tot += ms;

            //For the next iteration
            container.clear();
        }

        graphs::new_result(type, std::to_string(size), ms_tot.count() / 2);
    }
}

/* Find */

template<typename Container>
inline void find(std::size_t size, Container& container){
    for(std::size_t i = 0; i < size; ++i){
        typename Container::value_type v = {i};
        std::find(container.begin(), container.end(), v);
    }
}

template<typename Container>
void bench_find(const std::string& type){
    std::vector<std::size_t> sizes = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000};
    for(auto size : sizes){
        Container container;
        
        std::mt19937 generator;
        std::uniform_int_distribution<std::size_t> distribution(0, size - 1);

        for(std::size_t i = 0; i < size; ++i){
            container.push_back({distribution(generator)});
        }

        Clock::time_point t0 = Clock::now();

        for(std::size_t i = 0; i < REPEAT; ++i){
            find<Container>(size, container);
        }

        Clock::time_point t1 = Clock::now();
        auto d = std::chrono::duration_cast<microseconds>(t1 - t0);

        graphs::new_result(type, std::to_string(size), d.count() / REPEAT);
    }
}

/* Insert */

template<typename Container>
inline void insert(std::size_t size, Container& container){
    std::mt19937 generator;
    std::uniform_int_distribution<std::size_t> distribution(0, size - 1);

    for(std::size_t i = 0; i < 1000; ++i){
        typename Container::value_type v = {distribution(generator)};
        auto it = std::find(container.begin(), container.end(), v);
        container.insert(it, {size + i});
    }
}

template<typename Container>
void bench_insert(const std::string& type){
    std::vector<std::size_t> sizes = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
    for(auto size : sizes){
        std::vector<typename Container::value_type> temp;

        for(std::size_t i = 0; i < size; ++i){
            temp.push_back({i});
        }

        std::random_shuffle(temp.begin(), temp.end());

        Container container(temp.begin(), temp.end());

        Clock::time_point t0 = Clock::now();

        for(std::size_t i = 0; i < REPEAT; ++i){
            insert<Container>(size, container);
        }

        Clock::time_point t1 = Clock::now();
        auto d = std::chrono::duration_cast<milliseconds>(t1 - t0);

        graphs::new_result(type, std::to_string(size), d.count() / REPEAT);
    }
}

/* Remove */

template<typename Container>
inline void remove(Container& container){
    for(std::size_t i = 0; i < 1000; ++i){
        typename Container::value_type v = {i};
        auto it = std::find(container.begin(), container.end(), v);
        container.erase(it);
    }
}

template<typename Container>
void bench_remove(const std::string& type){
    std::vector<std::size_t> sizes = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
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
            remove<Container>(container);
            Clock::time_point t1 = Clock::now();
            milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
            ms_tot += ms;
        }

        graphs::new_result(type, std::to_string(size), ms_tot.count() / REPEAT);
    }
}

/* Sort */

template<typename T>
inline void sort_vector(std::vector<T>& container){
    std::sort(container.begin(), container.end());
}

template<typename T>
inline void sort_deque(std::deque<T>& container){
    std::sort(container.begin(), container.end());
}

template<typename T>
inline void sort_list(std::list<T>& container){
    container.sort();
}

template<typename Container, typename Function>
void bench_sort(Function function, const std::string& type){
    std::vector<std::size_t> sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
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

        graphs::new_result(type, std::to_string(size), ms_tot.count() / REPEAT);
    }
}

/* Destruction */

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
    
        graphs::new_result(type, std::to_string(size), us_tot.count() / REPEAT);
    }
}

/* Launch benchmarks on different sizes */

template<unsigned int Size>
void bench(){
    typedef Normal<Size> T;
    typedef Intrusive<Size, boost::intrusive::normal_link> I1;
    typedef Intrusive<Size, boost::intrusive::safe_link> I2;
    typedef Intrusive<Size, boost::intrusive::auto_unlink> I3;

    std::string size_str = std::to_string(sizeof(T));

    graphs::new_graph("fill_back_" + size_str, "fill_back - "  + size_str + " byte", "us");

    bench_fill_back_standard<std::list<T>>("list");
    bench_fill_back_intrusive<boost::intrusive::list<I1, boost::intrusive::constant_time_size<false>>>("normal_ilist");
    bench_fill_back_intrusive<boost::intrusive::list<I2, boost::intrusive::constant_time_size<false>>>("safe_ilist");
    bench_fill_back_intrusive<boost::intrusive::list<I3, boost::intrusive::constant_time_size<false>>>("auto_unlink_ilist");
    
    //Result are clear enough with very small size
    /*if(sizeof(T) == sizeof(long)){
        graphs::new_graph("fill_front_" + size_str, "fill_front - "  + size_str + " byte", "ms");

        bench_fill_front<std::vector<T>>(fill_front_vector<T>, "vector");
        bench_fill_front<std::list<T>>(fill_front_list<T>, "list");
        bench_fill_front<std::deque<T>>(fill_front_deque<T>, "deque");
    }
    
    graphs::new_graph("linear_search_" + size_str, "linear_search - "  + size_str + " byte", "us");
    
    bench_find<std::vector<T>>("vector");
    bench_find<std::list<T>>("list");
    bench_find<std::deque<T>>("deque");
    
    graphs::new_graph("random_insert_" + size_str, "random_insert - "  + size_str + " byte", "ms");
    
    bench_insert<std::vector<T>>("vector");
    bench_insert<std::list<T>>("list");
    bench_insert<std::deque<T>>("deque");
    
    graphs::new_graph("random_remove_" + size_str, "random_remove - "  + size_str + " byte", "ms");
    
    bench_remove<std::vector<T>>("vector");
    bench_remove<std::list<T>>("list");
    bench_remove<std::deque<T>>("deque");
    
    graphs::new_graph("sort_" + size_str, "sort - "  + size_str + " byte", "ms");

    bench_sort<std::vector<T>>(sort_vector<T>, "vector");
    bench_sort<std::list<T>>(sort_list<T>, "list");
    bench_sort<std::deque<T>>(sort_deque<T>, "deque");
    
    graphs::new_graph("destruction_" + size_str, "destruction - "  + size_str + " byte", "us");
    
    bench_destruction<std::vector<T>>("vector");
    bench_destruction<std::list<T>>("list");
    bench_destruction<std::deque<T>>("deque");*/
}

} //end of anonymous namespace

int main(){
    bench<0>();
    /*bench<Medium>();
    bench<Large>();
    bench<Huge>();
    bench<Monster>();
    bench<NonTrivial>();
    
    graphs::new_graph("number_crunching", "number_crunching", "ms");

    bench_number_crunching<std::vector<Small>>("vector");
    bench_number_crunching<std::list<Small>>("list");
    bench_number_crunching<std::deque<Small>>("deque");

    graphs::output(graphs::Output::GOOGLE);*/

    return 0;
}
