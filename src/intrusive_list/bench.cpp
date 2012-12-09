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
typedef std::chrono::microseconds microseconds;

static const std::size_t REPEAT = 7;

namespace {

template<unsigned int Size> 
struct filler {
    std::size_t dummy[Size];
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
    friend bool operator<=(const Normal<Size>& s1, const Normal<Size>& s2){ return s1.a <= s2.a; }
    friend bool operator>(const Normal<Size>& s1, const Normal<Size>& s2){ return s1.a > s2.a; }
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

/* Iterate */

template<typename Container>
inline void iterate(Container& container){
    std::size_t a = 0;

    for(auto& i : container){
        a += i.a;
    }
}

template<typename Container>
void bench_iterate(const std::string& type){
    std::vector<std::size_t> sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
    for(auto size : sizes){
        std::vector<typename Container::value_type> objects;
        objects.reserve(size);

        std::mt19937 generator;
        std::uniform_int_distribution<std::size_t> distribution(0, size - 1);
        
        Container container;

        for(std::size_t i = 0; i < size; ++i){
            objects.push_back({distribution(generator)});
            container.push_back(objects[i]);
        }

        Clock::time_point t0 = Clock::now();

        for(std::size_t i = 0; i < REPEAT; ++i){
            iterate<Container>(container);
        }

        Clock::time_point t1 = Clock::now();
        auto d = std::chrono::duration_cast<microseconds>(t1 - t0);

        graphs::new_result(type, std::to_string(size), d.count() / REPEAT);
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
        std::vector<typename Container::value_type> objects;
        objects.reserve(size);

        std::mt19937 generator;
        std::uniform_int_distribution<std::size_t> distribution(0, size - 1);
        
        Container container;

        for(std::size_t i = 0; i < size; ++i){
            objects.push_back({distribution(generator)});
            container.push_back(objects[i]);
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
void bench_insert_standard(const std::string& type){
    std::vector<std::size_t> sizes = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
    for(auto size : sizes){
        std::vector<typename Container::value_type> temp;
        Container container;

        for(std::size_t i = 0; i < size; ++i){
            temp.push_back({i});
            container.push_back(temp[i]);
        }
        
        std::mt19937 generator;
        std::uniform_int_distribution<std::size_t> distribution(0, size - 1);

        Clock::time_point t0 = Clock::now();

        for(std::size_t i = 0; i < REPEAT; ++i){
            for(std::size_t j = 0; j < 1000; ++j){
                auto v = distribution(generator);
                container.insert(
                        std::find(container.begin(), container.end(), temp[v]), 
                        {size + j});
            }
        }

        Clock::time_point t1 = Clock::now();
        auto d = std::chrono::duration_cast<microseconds>(t1 - t0);

        graphs::new_result(type, std::to_string(size), d.count() / REPEAT);
    }
}

template<typename Container>
void bench_insert_intrusive(const std::string& type){
    std::vector<std::size_t> sizes = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
    for(auto size : sizes){
        std::vector<typename Container::value_type> temp;
        temp.reserve(size + REPEAT * 1000);
        Container container;

        for(std::size_t i = 0; i < size; ++i){
            temp.push_back({i});
            container.push_back(temp[i]);
        }
    
        std::mt19937 generator;
        std::uniform_int_distribution<std::size_t> distribution(0, size - 1);

        Clock::time_point t0 = Clock::now();

        for(std::size_t i = 0; i < REPEAT; ++i){
            for(std::size_t j = 0; j < 1000; ++j){
                auto v = distribution(generator);
                temp.push_back({size + 1});
                container.insert(
                        container.iterator_to(temp[v]), 
                        temp.back());
            }
        }

        Clock::time_point t1 = Clock::now();
        auto d = std::chrono::duration_cast<microseconds>(t1 - t0);

        graphs::new_result(type, std::to_string(size), d.count() / REPEAT);
    }
}

/* Reverse */

template<typename Container>
void bench_reverse(const std::string& type){
    std::vector<std::size_t> sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
    for(auto size : sizes){
        std::vector<typename Container::value_type> temp;

        for(std::size_t i = 0; i < size; ++i){
            temp.push_back({i});
        }

        std::random_shuffle(temp.begin(), temp.end());

        microseconds ms_tot;

        Container container;
        for(std::size_t j = 0; j < temp.size(); ++j){
            container.push_back(temp[j]);
        }

        for(std::size_t i = 0; i < REPEAT; ++i){
            Clock::time_point t0 = Clock::now();
            container.reverse();
            Clock::time_point t1 = Clock::now();
            auto ms = std::chrono::duration_cast<microseconds>(t1 - t0);
            ms_tot += ms;
        }

        graphs::new_result(type, std::to_string(size), ms_tot.count() / REPEAT);
    }
}

/* Write */

//Necessary to not double the memory usage
template<typename Container>
void bench_write_standard(const std::string& type){
    std::vector<std::size_t> sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
    for(auto size : sizes){
        microseconds ms_tot;

        Container container;
        for(std::size_t j = 0; j < size; ++j){
            container.push_back({j});
        }

        for(std::size_t i = 0; i < REPEAT; ++i){
            Clock::time_point t0 = Clock::now();
            
            auto it = container.begin();
            auto end = container.end();

            for(; it != end; ++it){
                ++(it->a);
            }

            Clock::time_point t1 = Clock::now();
            auto ms = std::chrono::duration_cast<microseconds>(t1 - t0);
            ms_tot += ms;
        }

        graphs::new_result(type, std::to_string(size), ms_tot.count() / REPEAT);
    }
}

template<typename Container>
void bench_write_intrusive(const std::string& type){
    std::vector<std::size_t> sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
    for(auto size : sizes){
        std::vector<typename Container::value_type> temp;

        for(std::size_t i = 0; i < size; ++i){
            temp.push_back({i});
        }

        microseconds ms_tot;

        Container container;
        for(std::size_t j = 0; j < temp.size(); ++j){
            container.push_back(temp[j]);
        }

        for(std::size_t i = 0; i < REPEAT; ++i){
            Clock::time_point t0 = Clock::now();
            
            auto it = container.begin();
            auto end = container.end();

            for(; it != end; ++it){
                ++(it->a);
            }

            Clock::time_point t1 = Clock::now();
            auto ms = std::chrono::duration_cast<microseconds>(t1 - t0);
            ms_tot += ms;
        }

        graphs::new_result(type, std::to_string(size), ms_tot.count() / REPEAT);
    }
}

/* Sort */

template<typename Container>
void bench_sort(const std::string& type){
    std::vector<std::size_t> sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
    for(auto size : sizes){
        std::vector<typename Container::value_type> temp;

        for(std::size_t i = 0; i < size; ++i){
            temp.push_back({i});
        }

        std::random_shuffle(temp.begin(), temp.end());

        microseconds ms_tot;

        Container container;
        for(std::size_t j = 0; j < temp.size(); ++j){
            container.push_back(temp[j]);
        }

        for(std::size_t i = 0; i < REPEAT; ++i){
            Clock::time_point t0 = Clock::now();

            if(!(i % 2)){
                container.sort(std::greater<typename Container::value_type>());
            } else {
                container.sort(std::less<typename Container::value_type>());
            }
            
            Clock::time_point t1 = Clock::now();
            auto ms = std::chrono::duration_cast<microseconds>(t1 - t0);
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
            auto* container = new Container();

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

template<typename Container>
void bench_destruction_intrusive(const std::string& type){
    std::vector<std::size_t> sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
    for(auto size : sizes){
        microseconds us_tot;

        for(std::size_t i = 0; i < REPEAT; ++i){
            auto* objects = new std::vector<typename Container::value_type>();
            objects->reserve(size);
            auto* container = new Container();

            for(std::size_t i = 0; i < size; ++i){
                objects->push_back({i});
                container->push_back(objects->at(i));
            }

            Clock::time_point t0 = Clock::now();
            delete container;
            delete objects;
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
    
    graphs::new_graph("linear_search_" + size_str, "linear_search - "  + size_str + " byte", "us");
    
    bench_find<std::list<T>>("list");
    bench_find<boost::intrusive::list<I1, boost::intrusive::constant_time_size<false>>>("normal_ilist");
    bench_find<boost::intrusive::list<I2, boost::intrusive::constant_time_size<false>>>("safe_ilist");
    bench_find<boost::intrusive::list<I3, boost::intrusive::constant_time_size<false>>>("auto_unlink_ilist");
    
    graphs::new_graph("iterate_" + size_str, "iterate - "  + size_str + " byte", "us");
    
    bench_iterate<std::list<T>>("list");
    bench_iterate<boost::intrusive::list<I1, boost::intrusive::constant_time_size<false>>>("normal_ilist");
    bench_iterate<boost::intrusive::list<I2, boost::intrusive::constant_time_size<false>>>("safe_ilist");
    bench_iterate<boost::intrusive::list<I3, boost::intrusive::constant_time_size<false>>>("auto_unlink_ilist");
     
    graphs::new_graph("random_insert_" + size_str, "random_insert - "  + size_str + " byte", "us");
    
    bench_insert_standard<std::list<T>>("list");
    bench_insert_intrusive<boost::intrusive::list<I1, boost::intrusive::constant_time_size<false>>>("normal_ilist");
    bench_insert_intrusive<boost::intrusive::list<I2, boost::intrusive::constant_time_size<false>>>("safe_ilist");
    bench_insert_intrusive<boost::intrusive::list<I3, boost::intrusive::constant_time_size<false>>>("auto_unlink_ilist");
    
    graphs::new_graph("write_" + size_str, "write - "  + size_str + " byte", "us");

    bench_write_standard<std::list<T>>("list");
    bench_write_intrusive<boost::intrusive::list<I1, boost::intrusive::constant_time_size<false>>>("normal_ilist");
    bench_write_intrusive<boost::intrusive::list<I2, boost::intrusive::constant_time_size<false>>>("safe_ilist");
    bench_write_intrusive<boost::intrusive::list<I3, boost::intrusive::constant_time_size<false>>>("auto_unlink_ilist");
    
    graphs::new_graph("reverse_" + size_str, "reverse - "  + size_str + " byte", "us");

    bench_reverse<std::list<T>>("list");
    bench_reverse<boost::intrusive::list<I1, boost::intrusive::constant_time_size<false>>>("normal_ilist");
    bench_reverse<boost::intrusive::list<I2, boost::intrusive::constant_time_size<false>>>("safe_ilist");
    bench_reverse<boost::intrusive::list<I3, boost::intrusive::constant_time_size<false>>>("auto_unlink_ilist");
    
    graphs::new_graph("sort_" + size_str, "sort - "  + size_str + " byte", "us");

    bench_sort<std::list<T>>("list");
    bench_sort<boost::intrusive::list<I1, boost::intrusive::constant_time_size<false>>>("normal_ilist");
    bench_sort<boost::intrusive::list<I2, boost::intrusive::constant_time_size<false>>>("safe_ilist");
    bench_sort<boost::intrusive::list<I3, boost::intrusive::constant_time_size<false>>>("auto_unlink_ilist");
    
    graphs::new_graph("destruction_" + size_str, "destruction - "  + size_str + " byte", "us");
    
    bench_destruction<std::list<T>>("list");
    bench_destruction_intrusive<boost::intrusive::list<I1, boost::intrusive::constant_time_size<false>>>("normal_ilist");
    bench_destruction_intrusive<boost::intrusive::list<I2, boost::intrusive::constant_time_size<false>>>("safe_ilist");
    bench_destruction_intrusive<boost::intrusive::list<I3, boost::intrusive::constant_time_size<false>>>("auto_unlink_ilist");
}

} //end of anonymous namespace

int main(){
    bench<0>();         //8 bytes
    bench<3>();         //32 bytes
    bench<15>();        //128 bytes  
    bench<127>();       //1024 bytes
    bench<511>();       //4096 bytes

    graphs::output(graphs::Output::PLUGIN);

    return 0;
}
