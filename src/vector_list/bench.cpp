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
#include <cstdint>
#include <typeinfo>
#include <memory>

#include "bench.hpp"
#include "policies.hpp"

namespace {

template<typename T>
constexpr bool is_trivial_of_size(std::size_t size){
    return std::is_trivial<T>::value && sizeof(T) == size;
}

template<typename T>
constexpr bool is_non_trivial_of_size(std::size_t size){
    return
            !std::is_trivial<T>::value
        &&  sizeof(T) == size
        &&  std::is_copy_constructible<T>::value
        &&  std::is_copy_assignable<T>::value
        &&  std::is_move_constructible<T>::value
        &&  std::is_move_assignable<T>::value;
}

template<typename T>
constexpr bool is_non_trivial_nothrow_movable(){
    return
            !std::is_trivial<T>::value
        &&  std::is_nothrow_move_constructible<T>::value
        &&  std::is_nothrow_move_assignable<T>::value;
}

template<typename T>
constexpr bool is_non_trivial_non_nothrow_movable(){
    return
            !std::is_trivial<T>::value
        &&  std::is_move_constructible<T>::value
        &&  std::is_move_assignable<T>::value
        &&  !std::is_nothrow_move_constructible<T>::value
        &&  !std::is_nothrow_move_assignable<T>::value;
}

template<typename T>
constexpr bool is_non_trivial_non_movable(){
    return
            !std::is_trivial<T>::value
        &&  std::is_copy_constructible<T>::value
        &&  std::is_copy_assignable<T>::value
        &&  !std::is_move_constructible<T>::value
        &&  !std::is_move_assignable<T>::value;
}

template<typename T>
constexpr bool is_small(){
   return sizeof(T) <= sizeof(std::size_t);
}

} //end of anonymous namespace

// tested types

// trivial type with parametrized size
template<int N>
struct Trivial {
    std::size_t a;
    std::array<unsigned char, N-sizeof(a)> b;
    bool operator<(const Trivial &other) const { return a < other.a; }
};

template<>
struct Trivial<sizeof(std::size_t)> {
    std::size_t a;
    bool operator<(const Trivial &other) const { return a < other.a; }
};

// non trivial, quite expensive to copy but easy to move (noexcept not set)
class NonTrivialStringMovable {
    private:
        std::string data{"some pretty long string to make sure it is not optimized with SSO"};

    public:
        std::size_t a{0};
        NonTrivialStringMovable() = default;
        NonTrivialStringMovable(std::size_t a): a(a) {}
        ~NonTrivialStringMovable() = default;
        bool operator<(const NonTrivialStringMovable &other) const { return a < other.a; }
};

// non trivial, quite expensive to copy but easy to move (with noexcept)
class NonTrivialStringMovableNoExcept {
    private:
        std::string data{"some pretty long string to make sure it is not optimized with SSO"};

    public:
        std::size_t a{0};
        NonTrivialStringMovableNoExcept() = default;
        NonTrivialStringMovableNoExcept(std::size_t a): a(a) {}
        NonTrivialStringMovableNoExcept(const NonTrivialStringMovableNoExcept &) = default;
        NonTrivialStringMovableNoExcept(NonTrivialStringMovableNoExcept &&) noexcept = default;
        ~NonTrivialStringMovableNoExcept() = default;
        NonTrivialStringMovableNoExcept &operator=(const NonTrivialStringMovableNoExcept &) = default;
        NonTrivialStringMovableNoExcept &operator=(NonTrivialStringMovableNoExcept &&other) noexcept {
            std::swap(data, other.data);
            std::swap(a, other.a);
            return *this;
        }
        bool operator<(const NonTrivialStringMovableNoExcept &other) const { return a < other.a; }
};

// non trivial, quite expensive to copy and move
template<int N>
class NonTrivialArray {
    public:
        std::size_t a = 0;

    private:
        std::array<unsigned char, N-sizeof(a)> b;

    public:
        NonTrivialArray() = default;
        NonTrivialArray(std::size_t a): a(a) {}
        ~NonTrivialArray() = default;
        bool operator<(const NonTrivialArray &other) const { return a < other.a; }
};

// type definitions for testing and invariants check
using TrivialSmall   = Trivial<8>;       static_assert(is_trivial_of_size<TrivialSmall>(8),        "Invalid type");
using TrivialMedium  = Trivial<32>;      static_assert(is_trivial_of_size<TrivialMedium>(32),      "Invalid type");
using TrivialLarge   = Trivial<128>;     static_assert(is_trivial_of_size<TrivialLarge>(128),      "Invalid type");
using TrivialHuge    = Trivial<1024>;    static_assert(is_trivial_of_size<TrivialHuge>(1024),      "Invalid type");
using TrivialMonster = Trivial<4*1024>;  static_assert(is_trivial_of_size<TrivialMonster>(4*1024), "Invalid type");

static_assert(is_non_trivial_nothrow_movable<NonTrivialStringMovableNoExcept>(), "Invalid type");
static_assert(is_non_trivial_non_nothrow_movable<NonTrivialStringMovable>(), "Invalid type");

using NonTrivialArrayMedium = NonTrivialArray<32>;
static_assert(is_non_trivial_of_size<NonTrivialArrayMedium>(32), "Invalid type");

// Define all benchmarks

template<typename T>
struct bench_fill_back {
    static void run(){
        new_graph<T>("fill_back", "us");

        auto sizes = { 100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000 };
        bench<std::vector<T>, microseconds, Empty, FillBack>("vector", sizes);
        bench<std::list<T>,   microseconds, Empty, FillBack>("list",   sizes);
        bench<std::deque<T>,  microseconds, Empty, FillBack>("deque",  sizes);
        bench<std::vector<T>, microseconds, Empty, ReserveSize, FillBack>("vector_reserve", sizes);
        bench<std::vector<T>, microseconds, Empty, FillBackInserter>("vector_inserter", sizes);
        bench<std::list<T>,   microseconds, Empty, FillBackInserter>("list_inserter",   sizes);
        bench<std::deque<T>,  microseconds, Empty, FillBackInserter>("deque_inserter",  sizes);
    }
};

template<typename T>
struct bench_emplace_back {
    static void run(){
        new_graph<T>("emplace_back", "us");

        auto sizes = { 100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000 };
        bench<std::vector<T>, microseconds, Empty, EmplaceBack>("vector", sizes);
        bench<std::list<T>,   microseconds, Empty, EmplaceBack>("list",   sizes);
        bench<std::deque<T>,  microseconds, Empty, EmplaceBack>("deque",  sizes);
    }
};

template<typename T>
struct bench_fill_front {
    static void run(){
        new_graph<T>("fill_front", "us");

        auto sizes = { 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000 };

        // it is too slow with bigger data types
        if(is_small<T>()){
            bench<std::vector<T>, microseconds, Empty, FillFront>("vector", sizes);
        }

        bench<std::list<T>,   microseconds, Empty, FillFront>("list",   sizes);
        bench<std::deque<T>,  microseconds, Empty, FillFront>("deque",  sizes);
    }
};

template<typename T>
struct bench_emplace_front {
    static void run(){
        new_graph<T>("emplace_front", "us");

        auto sizes = { 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000 };

        // it is too slow with bigger data types
        if(is_small<T>()){
            bench<std::vector<T>, microseconds, Empty, EmplaceFront>("vector", sizes);
        }

        bench<std::list<T>,   microseconds, Empty, EmplaceFront>("list",   sizes);
        bench<std::deque<T>,  microseconds, Empty, EmplaceFront>("deque",  sizes);
    }
};

template<typename T>
struct bench_linear_search {
    static void run(){
        new_graph<T>("linear_search", "us");

        auto sizes = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000};
        bench<std::vector<T>, microseconds, FilledRandom, Find>("vector", sizes);
        bench<std::list<T>,   microseconds, FilledRandom, Find>("list",   sizes);
        bench<std::deque<T>,  microseconds, FilledRandom, Find>("deque",  sizes);
    }
};

template<typename T>
struct bench_random_insert {
    static void run(){
        new_graph<T>("random_insert", "ms");

        auto sizes = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
        bench<std::vector<T>, milliseconds, FilledRandom, Insert>("vector", sizes);
        bench<std::list<T>,   milliseconds, FilledRandom, Insert>("list",   sizes);
        bench<std::deque<T>,  milliseconds, FilledRandom, Insert>("deque",  sizes);
    }
};

template<typename T>
struct bench_random_remove {
    static void run(){
        new_graph<T>("random_remove", "ms");

        auto sizes = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
        bench<std::vector<T>, milliseconds, FilledRandom, Erase>("vector", sizes);
        bench<std::list<T>,   milliseconds, FilledRandom, Erase>("list",   sizes);
        bench<std::deque<T>,  milliseconds, FilledRandom, Erase>("deque",  sizes);
        bench<std::vector<T>, milliseconds, FilledRandom, RemoveErase>("vector_rem", sizes);
    }
};

template<typename T>
struct bench_sort {
    static void run(){
        new_graph<T>("sort", "ms");

        auto sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
        bench<std::vector<T>, milliseconds, FilledRandom, Sort>("vector", sizes);
        bench<std::list<T>,   milliseconds, FilledRandom, Sort>("list",   sizes);
        bench<std::deque<T>,  milliseconds, FilledRandom, Sort>("deque",  sizes);
    }
};

template<typename T>
struct bench_destruction {
    static void run(){
        new_graph<T>("destruction", "us");

        auto sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
        bench<std::vector<T>, microseconds, SmartFilled, SmartDelete>("vector", sizes);
        bench<std::list<T>,   microseconds, SmartFilled, SmartDelete>("list",   sizes);
        bench<std::deque<T>,  microseconds, SmartFilled, SmartDelete>("deque",  sizes);
    }
};

template<typename T>
struct bench_number_crunching {
    static void run(){
        new_graph<T>("number_crunching", "ms");

        auto sizes = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
        bench<std::vector<T>, milliseconds, Empty, RandomSortedInsert>("vector", sizes);
        bench<std::list<T>,   milliseconds, Empty, RandomSortedInsert>("list",   sizes);
        bench<std::deque<T>,  milliseconds, Empty, RandomSortedInsert>("deque",  sizes);
    }
};

//Launch the benchmark

template<typename ...Types>
void bench_all(){
    bench_types<bench_fill_back,        Types...>();
    bench_types<bench_emplace_back,     Types...>();
    bench_types<bench_fill_front,       Types...>();
    bench_types<bench_emplace_front,    Types...>();
    bench_types<bench_linear_search,    Types...>();
    bench_types<bench_random_insert,    Types...>();
    bench_types<bench_random_remove,    Types...>();
    bench_types<bench_sort,             Types...>();
    bench_types<bench_destruction,      Types...>();

    // it is really slow so run only for limited set of data
    bench_types<bench_number_crunching, TrivialSmall, TrivialMedium>();
}

int main(){
    //Launch all the graphs
    bench_all<
        TrivialSmall,
        TrivialMedium,
        TrivialLarge,
        TrivialHuge,
        TrivialMonster,
        NonTrivialStringMovable,
        NonTrivialStringMovableNoExcept,
        NonTrivialArray<32> >();

    //Generate the graphs
    graphs::output(graphs::Output::GOOGLE);

    return 0;
}
