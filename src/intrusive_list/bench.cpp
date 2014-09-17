//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <random>
#include <array>
#include <vector>
#include <list>
#include <algorithm>
#include <deque>
#include <thread>
#include <iostream>
#include <chrono>

#include <boost/intrusive/list.hpp>

#include "bench.hpp"
#include "policies.hpp"

template<unsigned int Size>
struct Normal {
    static const unsigned int size = Size;

    std::size_t a;
    std::array<unsigned char, Size-sizeof(a)> b;

    Normal<Size>(){}
    Normal<Size>(std::size_t a) : a(a) {}

    friend bool operator<(const Normal<Size>& s1, const Normal<Size>& s2){ return s1.a < s2.a; }
};

template<>
struct Normal<sizeof(std::size_t)> {
    static const unsigned int size = sizeof(std::size_t);

    std::size_t a;

    Normal(){}
    Normal(std::size_t a) : a(a) {}

    friend bool operator<(const Normal& s1, const Normal& s2){ return s1.a < s2.a; }
};

template<unsigned int Size, boost::intrusive::link_mode_type LinkMode = boost::intrusive::safe_link>
struct Intrusive : public boost::intrusive::list_base_hook<boost::intrusive::link_mode<LinkMode>>, public Normal<Size> {
    Intrusive(){}
    Intrusive(std::size_t a) : Normal<Size>(a) {}
};

static_assert(sizeof(Normal<8>) == 8,       "Invalid type");
static_assert(sizeof(Normal<32>) == 32,     "Invalid type");
static_assert(sizeof(Normal<128>) == 128,   "Invalid type");
static_assert(sizeof(Normal<1024>) == 1024, "Invalid type");
static_assert(sizeof(Normal<4096>) == 4096, "Invalid type");

static_assert(sizeof(Intrusive<8>)      == sizeof(Normal<8>) + 2 * sizeof(std::size_t),         "Invalid type");
static_assert(sizeof(Intrusive<32>)     == sizeof(Normal<32>) + 2 * sizeof(std::size_t),        "Invalid type");
static_assert(sizeof(Intrusive<128>)    == sizeof(Normal<128>) + 2 * sizeof(std::size_t),       "Invalid type");
static_assert(sizeof(Intrusive<1024>)   == sizeof(Normal<1024>) + 2 * sizeof(std::size_t),      "Invalid type");
static_assert(sizeof(Intrusive<4096>)   == sizeof(Normal<4096>) + 2 * sizeof(std::size_t),      "Invalid type");

namespace {

template<unsigned int Size>
struct intrusive_list_type {
    typedef Intrusive<Size, boost::intrusive::normal_link> I1;
    typedef Intrusive<Size, boost::intrusive::safe_link> I2;
    typedef Intrusive<Size, boost::intrusive::auto_unlink> I3;

    typedef boost::intrusive::list<I1, boost::intrusive::constant_time_size<false>> L1;
    typedef boost::intrusive::list<I2, boost::intrusive::constant_time_size<false>> L2;
    typedef boost::intrusive::list<I3, boost::intrusive::constant_time_size<false>> L3;
};

template<typename T>
struct bench_destruction {
    static void run(){
        new_graph<T>("destruction", "us");

        auto sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
        bench<std::list<T>,                                 microseconds, SmartFilled,          SmartDelete>("list",               sizes);
        bench<typename intrusive_list_type<T::size>::L1,    microseconds, BackupSmartFilled,    SmartDelete>("normal_ilist",       sizes);
        bench<typename intrusive_list_type<T::size>::L2,    microseconds, BackupSmartFilled,    SmartDelete>("safe_list",          sizes);
        bench<typename intrusive_list_type<T::size>::L3,    microseconds, BackupSmartFilled,    SmartDelete>("auto_unlink_list",   sizes);
    }
};

template<typename T>
struct bench_sort {
    static void run(){
        new_graph<T>("sort", "ms");

        auto sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
        bench<std::list<T>,                                 microseconds, FilledRandom,    Sort>("list",               sizes);
        bench<typename intrusive_list_type<T::size>::L1,    microseconds, FilledRandom,    Sort>("normal_ilist",       sizes);
        bench<typename intrusive_list_type<T::size>::L2,    microseconds, FilledRandom,    Sort>("safe_list",          sizes);
        bench<typename intrusive_list_type<T::size>::L3,    microseconds, FilledRandom,    Sort>("auto_unlink_list",   sizes);
    }
};

template<typename T>
struct bench_reverse {
    static void run(){
        new_graph<T>("reverse", "us");

        auto sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
        bench<std::list<T>,                                 microseconds, FilledRandom,    Reverse>("list",               sizes);
        bench<typename intrusive_list_type<T::size>::L1,    microseconds, FilledRandom,    Reverse>("normal_ilist",       sizes);
        bench<typename intrusive_list_type<T::size>::L2,    microseconds, FilledRandom,    Reverse>("safe_list",          sizes);
        bench<typename intrusive_list_type<T::size>::L3,    microseconds, FilledRandom,    Reverse>("auto_unlink_list",   sizes);
    }
};

template<typename T>
struct bench_fill_back {
    static void run(){
        new_graph<T>("fill_back", "us");

        auto sizes = { 100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000 };
        bench<std::list<T>,                                 microseconds, EmptyPrepareBackup, FillBackBackup>("list",   sizes);
        bench<typename intrusive_list_type<T::size>::L1,    microseconds, EmptyPrepareBackup, FillBackBackup>("normal_ilist",       sizes);
        bench<typename intrusive_list_type<T::size>::L2,    microseconds, EmptyPrepareBackup, FillBackBackup>("safe_list",          sizes);
        bench<typename intrusive_list_type<T::size>::L3,    microseconds, EmptyPrepareBackup, FillBackBackup>("auto_unlink_list",   sizes);
    }
};

template<typename T>
struct bench_linear_search {
    static void run(){
        new_graph<T>("linear_search", "us");

        auto sizes = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000};
        bench<std::list<T>,                                 microseconds, FilledRandom, Find>("list",               sizes);
        bench<typename intrusive_list_type<T::size>::L1,    microseconds, FilledRandom, Find>("normal_ilist",       sizes);
        bench<typename intrusive_list_type<T::size>::L2,    microseconds, FilledRandom, Find>("safe_list",          sizes);
        bench<typename intrusive_list_type<T::size>::L3,    microseconds, FilledRandom, Find>("auto_unlink_list",   sizes);
    }
};

template<typename T>
struct bench_write {
    static void run(){
        new_graph<T>("write", "us");

        auto sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
        bench<std::list<T>,                                 microseconds, FilledRandom, Write>("list",               sizes);
        bench<typename intrusive_list_type<T::size>::L1,    microseconds, FilledRandom, Write>("normal_ilist",       sizes);
        bench<typename intrusive_list_type<T::size>::L2,    microseconds, FilledRandom, Write>("safe_list",          sizes);
        bench<typename intrusive_list_type<T::size>::L3,    microseconds, FilledRandom, Write>("auto_unlink_list",   sizes);
    }
};

template<typename T>
struct bench_iterate {
    static void run(){
        new_graph<T>("iterate", "us");

        auto sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
        bench<std::list<T>,                                 microseconds, FilledRandom, Iterate>("list",               sizes);
        bench<typename intrusive_list_type<T::size>::L1,    microseconds, FilledRandom, Iterate>("normal_ilist",       sizes);
        bench<typename intrusive_list_type<T::size>::L2,    microseconds, FilledRandom, Iterate>("safe_list",          sizes);
        bench<typename intrusive_list_type<T::size>::L3,    microseconds, FilledRandom, Iterate>("auto_unlink_list",   sizes);
    }
};

template<typename T>
struct bench_random_insert {
    static void run(){
        new_graph<T>("random_insert", "us");

        auto sizes = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
        bench<std::list<T>,                                 microseconds, FilledRandom, Insert>("list",               sizes);
        bench<typename intrusive_list_type<T::size>::L1,    microseconds, FilledRandom, Insert>("normal_ilist",       sizes);
        bench<typename intrusive_list_type<T::size>::L2,    microseconds, FilledRandom, Insert>("safe_list",          sizes);
        bench<typename intrusive_list_type<T::size>::L3,    microseconds, FilledRandom, Insert>("auto_unlink_list",   sizes);
    }
};

//Launch the benchmark

template<typename ...Types>
void bench_all(){
    bench_types<bench_destruction,      Types...>();
    bench_types<bench_sort,             Types...>();
    bench_types<bench_reverse,          Types...>();
    bench_types<bench_fill_back,        Types...>();
    bench_types<bench_linear_search,    Types...>();
    bench_types<bench_write,            Types...>();
    bench_types<bench_iterate,          Types...>();
    bench_types<bench_random_insert,    Types...>();
}

} //end of anonymous namespace

int main(){
    bench_all<
        Normal<8>,
        Normal<32>,
        Normal<128>,
        Normal<1024>>();
        //Normal<4096>>();

    graphs::output(graphs::Output::PLUGIN);

    return 0;
}
