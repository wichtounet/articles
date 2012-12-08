#include "graphs.hpp"

#include <random>
#include <array>
#include <vector>
#include <list>
#include <algorithm>
#include <deque>
#include <thread>
#include <iostream>
#include <chrono>
#include <cstdint>
#include <typeinfo>
#include <memory>


static const std::size_t REPEAT = 5;

namespace {

  struct deleter_free {
    template<class T>
    void operator()(T *p) const { free(p); }
  };

#ifdef __GNUC__

#include <cxxabi.h>

  std::string demangle(const char *name)
  {
    int status = 0;
    std::unique_ptr<char, deleter_free> demName(abi::__cxa_demangle(name, nullptr, nullptr, &status));
    return (status==0) ? demName.get() : name;
  }
  
#else
  
  std::string demangle(const char *name) { return name; }
  
#endif

  bool istag(int c)
  {
    return std::isalnum(c) || c == '_';
  }
  
  std::string tag(std::string name)
  {
    std::replace_if(begin(name), end(name), [](char c){ return !istag(c); }, '_');
    std::string res;
    res.swap(name);
    return res;
  }

  template<typename T>
  void new_graph(const std::string &testName, const std::string &unit)
  {
    std::string title(testName + " - " + demangle(typeid(T).name()));
    graphs::new_graph(tag(title), title, unit);
  }
  
  template<typename T>
  constexpr bool is_trivial_of_size(std::size_t size)
  { return std::is_trivial<T>::value && sizeof(T) == size; }
  
  template<typename T>
  constexpr bool is_non_trivial_nothrow_movable()
  { return !std::is_trivial<T>::value &&
      std::is_nothrow_move_constructible<T>::value &&
      std::is_nothrow_move_assignable<T>::value; }
  
  template<typename T>
  constexpr bool is_non_trivial_non_nothrow_movable()
  { return !std::is_trivial<T>::value  &&
      std::is_move_constructible<T>::value &&
      std::is_move_assignable<T>::value &&
      !std::is_nothrow_move_constructible<T>::value &&
      !std::is_nothrow_move_assignable<T>::value; }
  
  template<typename T>
  constexpr bool is_non_trivial_non_movable()
  { return !std::is_trivial<T>::value  &&
      std::is_copy_constructible<T>::value &&
      std::is_copy_assignable<T>::value &&
      !std::is_move_constructible<T>::value &&
      !std::is_move_assignable<T>::value; }
  
  template<typename T>
  constexpr bool is_non_trivial_of_size(std::size_t size)
  { return !std::is_trivial<T>::value && sizeof(T) == size &&
      std::is_copy_constructible<T>::value &&
      std::is_copy_assignable<T>::value &&
      std::is_move_constructible<T>::value &&
      std::is_move_assignable<T>::value; }

}


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
class NonTrivialString {
  std::string data{"some pretty long string to make sure it is not optimized with SSO"};
public: 
  std::size_t a{0};
  NonTrivialString() = default;
  NonTrivialString(std::size_t a): a(a) {}
  ~NonTrivialString() = default;
  bool operator<(const NonTrivialString &other) const { return a < other.a; }
};
  
// non trivial, quite expensive to copy but easy to move (with noexcept)
class NonTrivialStringNoExcept {
  std::string data{"some pretty long string to make sure it is not optimized with SSO"};
public: 
  std::size_t a{0};
  NonTrivialStringNoExcept() = default;
  NonTrivialStringNoExcept(std::size_t a): a(a) {}
  NonTrivialStringNoExcept(const NonTrivialStringNoExcept &) = default;
  NonTrivialStringNoExcept(NonTrivialStringNoExcept &&) noexcept = default;
  ~NonTrivialStringNoExcept() = default;
  NonTrivialStringNoExcept &operator=(const NonTrivialStringNoExcept &) = default;
  NonTrivialStringNoExcept &operator=(NonTrivialStringNoExcept &&other) noexcept
  {
    using namespace std;
    swap(data, other.data);
    swap(a, other.a);
    return *this;
  }
  bool operator<(const NonTrivialStringNoExcept &other) const { return a < other.a; }
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

static_assert(is_non_trivial_nothrow_movable<NonTrivialStringNoExcept>(), "Invalid type");
static_assert(is_non_trivial_non_nothrow_movable<NonTrivialString>(), "Invalid type");

using NonTrivialArrayMedium = NonTrivialArray<32>;
static_assert(is_non_trivial_of_size<NonTrivialArrayMedium>(32), "Invalid type");

  
// create policies
template<class Container>
struct Empty {
  inline static Container make(std::size_t) { return Container(); }
};
   
template<class Container>
struct Filled {
  inline static Container make(std::size_t size) { return Container(size); }
};
  
template<class Container>
struct FilledRandom {
  static std::vector<size_t> v;
  inline static Container make(std::size_t size)
  {
    // prepare randomized data that will have all the integers from the range
    if(v.size() != size) {
      v.clear();
      v.resize(size);
      std::iota(begin(v), end(v), 0);
      std::shuffle(begin(v), end(v), std::mt19937());
    }
      
    // fill with randomized data
    Container c;
    for(auto val : v)
      c.push_back({val});
    return c;
  }
};
template<class Container> std::vector<size_t> FilledRandom<Container>::v;
  
template<class Container>
struct SmartFilled {
  inline static std::unique_ptr<Container> make(std::size_t size)
  { return std::unique_ptr<Container>(new Container(size)); }
};
  
  
// testing policies
template<class Container>
struct NoOp {
  inline static void run(Container &, std::size_t) {}
};
  
template<class Container>
struct ReserveSize {
  inline static void run(Container &c, std::size_t size) { c.reserve(size); }
};
  
template<class Container>
struct FillBack {
  static const typename Container::value_type value;
  inline static void run(Container &c, std::size_t size)
  { std::fill_n(std::back_inserter(c), size, value); }
};
template<class Container> const typename Container::value_type FillBack<Container>::value{};
  
template<class Container>
struct EmplaceBack {
  inline static void run(Container &c, std::size_t size)
  { for(size_t i=0; i<size; ++i) c.emplace_back(); }
};
  
template<class Container>
struct FillFront {
  static const typename Container::value_type value;
  inline static void run(Container &c, std::size_t size)
  { std::fill_n(std::front_inserter(c), size, value); }
};
template<class Container> const typename Container::value_type FillFront<Container>::value{};
  
template<class T>
struct FillFront<std::vector<T> > {
  static const T value;
  inline static void run(std::vector<T> &c, std::size_t size)
  {
    for(std::size_t i=0; i<size; ++i)
      c.insert(begin(c), value);
  }
};
template<class T> const T FillFront<std::vector<T> >::value{};
  
template<class Container>
struct EmplaceFront {
  inline static void run(Container &c, std::size_t size)
  { for(size_t i=0; i<size; ++i) c.emplace_front(); }
};
  
template<class T>
struct EmplaceFront<std::vector<T> > {
  inline static void run(std::vector<T> &c, std::size_t size)
  { for(std::size_t i=0; i<size; ++i) c.emplace(begin(c)); }
};
  
template<class Container>
struct Find {
  inline static void run(Container &c, std::size_t size)
  {
    for(std::size_t i=0; i<size; ++i) {
      // hand written comparison to eliminate temporary object creation
      std::find_if(begin(c), end(c), [&](decltype(*begin(c)) v){ return v.a == i; });
    }
  }
};
  
template<class Container>
struct Insert {
  inline static void run(Container &c, std::size_t size)
  {
    for(std::size_t i=0; i<1000; ++i) {
      // hand written comparison to eliminate temporary object creation
      auto it = std::find_if(begin(c), end(c), [&](decltype(*begin(c)) v){ return v.a == i; });
      c.insert(it, {size + i});
    }
  }
};
  
template<class Container>
struct Erase {
  inline static void run(Container &c, std::size_t size)
  {
    for(std::size_t i=0; i<1000; ++i) {
      // hand written comparison to eliminate temporary object creation
      c.erase(std::find_if(begin(c), end(c), [&](decltype(*begin(c)) v){ return v.a == i; }));
    }
  }
};
  
template<class Container>
struct RemoveErase {
  inline static void run(Container &c, std::size_t size)
  {
    for(std::size_t i=0; i<1000; ++i) {
      // hand written comparison to eliminate temporary object creation
      c.erase(std::remove_if(begin(c), end(c), [&](decltype(*begin(c)) v){ return v.a == i; }), end(c));
    }
  }
};
  
template<class Container>
struct Sort {
  inline static void run(Container &c, std::size_t size)
  {
    std::sort(c.begin(), c.end());
  }
};
  
template<class T>
struct Sort<std::list<T> > {
  inline static void run(std::list<T> &c, std::size_t size)
  {
    c.sort();
  }
};

template<class Container>
struct SmartDelete {
  inline static void run(Container &c, std::size_t size) { c.reset(); }
};

template<class Container>
struct RandomSortedInsert {
  static std::mt19937 generator;
  static std::uniform_int_distribution<std::size_t> distribution;
  inline static void run(Container &c, std::size_t size)
  {
    for(std::size_t i=0; i<size; ++i){
      auto val = distribution(generator);
      // hand written comparison to eliminate temporary object creation
      c.insert(std::find_if(begin(c), end(c), [&](decltype(*begin(c)) v){ return v.a >= val; }), {val});
    }
  }
};
template<class Container> std::mt19937 RandomSortedInsert<Container>::generator;
template<class Container> std::uniform_int_distribution<std::size_t> RandomSortedInsert<Container>::distribution(0, std::numeric_limits<std::size_t>::max() - 1);
  
  
// variadic policy runner
template<class Container>
inline static void run(Container &, std::size_t)
{
}
  
template<template<class> class Test, template<class> class ...Rest, class Container>
inline static void run(Container &container, std::size_t size)
{
  Test<Container>::run(container, size);
  run<Rest...>(container, size);
}
  
  
using std::chrono::milliseconds;
using std::chrono::microseconds;

// benchmarking procedure
template<typename Container,
         typename DurationUnit,
         template<class> class CreatePolicy,
         template<class> class ...TestPolicy>
void bench(const std::string& type, const std::initializer_list<int> &sizes)
{
  using Clock = std::chrono::high_resolution_clock;

  // create an element to copy so the temporary creation
  // and initialization will not be accounted in a benchmark
  typename Container::value_type value;
  for(auto size : sizes) {
    Clock::duration duration;
      
    for(std::size_t i=0; i<REPEAT; ++i) {
      auto container = CreatePolicy<Container>::make(size);
        
      Clock::time_point t0 = Clock::now();
        
      // run test
      run<TestPolicy...>(container, size);
        
      Clock::time_point t1 = Clock::now();
      duration += t1 - t0;
    }
      
    graphs::new_result(type, std::to_string(size),
                       std::chrono::duration_cast<DurationUnit>(duration).count() / REPEAT);
  }
}


// benchmarks
template<typename T>
struct bench_fill_back { 
  static void run()
  {
    new_graph<T>("fill_back", "us");
    auto sizes = { 100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000 };
    bench<std::vector<T>, microseconds, Empty, ReserveSize, FillBack>("vector_pre", sizes);
    bench<std::vector<T>, microseconds, Empty, FillBack>("vector", sizes);
    bench<std::list<T>,   microseconds, Empty, FillBack>("list",   sizes);
    bench<std::deque<T>,  microseconds, Empty, FillBack>("deque",  sizes);
  }
};

template<typename T>
struct bench_emplace_back {
  static void run()
  {
    new_graph<T>("emplace_back", "us");
    auto sizes = { 100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000 };
    bench<std::vector<T>, microseconds, Empty, EmplaceBack>("vector", sizes);
    bench<std::list<T>,   microseconds, Empty, EmplaceBack>("list",   sizes);
    bench<std::deque<T>,  microseconds, Empty, EmplaceBack>("deque",  sizes);
  }
};

template<typename T>
struct bench_fill_front {
  static void run()
  {
    new_graph<T>("fill_front", "us");
    auto sizes = { 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000 };
    bench<std::vector<T>, microseconds, Empty, FillFront>("vector", sizes);
    bench<std::list<T>,   microseconds, Empty, FillFront>("list",   sizes);
    bench<std::deque<T>,  microseconds, Empty, FillFront>("deque",  sizes);
  }
};

template<typename T>
struct bench_emplace_front {
  static void run()
  {
    new_graph<T>("emplace_front", "us");
    auto sizes = { 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000 };
    bench<std::vector<T>, microseconds, Empty, EmplaceFront>("vector", sizes);
    bench<std::list<T>,   microseconds, Empty, EmplaceFront>("list",   sizes);
    bench<std::deque<T>,  microseconds, Empty, EmplaceFront>("deque",  sizes);
  }
};

template<typename T>
struct bench_linear_search {
  static void run()
  {
    new_graph<T>("linear_search", "us");
    auto sizes = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000};
    bench<std::vector<T>, microseconds, FilledRandom, Find>("vector", sizes);
    bench<std::list<T>,   microseconds, FilledRandom, Find>("list",   sizes);
    bench<std::deque<T>,  microseconds, FilledRandom, Find>("deque",  sizes);
  }
};

template<typename T>
struct bench_random_insert {
  static void run()
  {
    new_graph<T>("random_insert", "ms");
    auto sizes = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
    bench<std::vector<T>, milliseconds, FilledRandom, Insert>("vector", sizes);
    bench<std::list<T>,   milliseconds, FilledRandom, Insert>("list",   sizes);
    bench<std::deque<T>,  milliseconds, FilledRandom, Insert>("deque",  sizes);
  }
};

template<typename T>
struct bench_random_remove {
  static void run()
  {
    new_graph<T>("random_remove", "ms");
    auto sizes = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
    bench<std::vector<T>, milliseconds, FilledRandom, Erase>("vector", sizes);
    bench<std::vector<T>, milliseconds, FilledRandom, RemoveErase>("vector_rem", sizes);
    bench<std::list<T>,   milliseconds, FilledRandom, Erase>("list",   sizes);
    bench<std::deque<T>,  milliseconds, FilledRandom, Erase>("deque",  sizes);
  }
};

template<typename T>
struct bench_sort {
  static void run()
  {
    new_graph<T>("sort", "ms");
    auto sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
    bench<std::vector<T>, milliseconds, FilledRandom, Sort>("vector", sizes);
    bench<std::list<T>,   milliseconds, FilledRandom, Sort>("list",   sizes);
    bench<std::deque<T>,  milliseconds, FilledRandom, Sort>("deque",  sizes);
  }
};

template<typename T>
struct bench_destruction {
  static void run()
  {
    new_graph<T>("destruction", "us");
    auto sizes = {100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
    bench<std::vector<T>, microseconds, SmartFilled, SmartDelete>("vector", sizes);
    bench<std::list<T>,   microseconds, SmartFilled, SmartDelete>("list",   sizes);
    bench<std::deque<T>,  microseconds, SmartFilled, SmartDelete>("deque",  sizes);
  }
};
  
template<typename T>
struct bench_number_crunching {
  static void run()
  {
    new_graph<T>("number_crunching", "ms");
    auto sizes = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
    bench<std::vector<T>, milliseconds, Empty, RandomSortedInsert>("vector", sizes);
    bench<std::list<T>,   milliseconds, Empty, RandomSortedInsert>("list",   sizes);
    bench<std::deque<T>,  milliseconds, Empty, RandomSortedInsert>("deque",  sizes);
  }
};


// benchmark a list of types
template<template<class> class Benchmark>
void bench_types()
{  
}

template<template<class> class Benchmark, typename T, typename ...Types>
void bench_types()
{  
  Benchmark<T>::run();
  bench_types<Benchmark, Types...>();
}


// run all benchmarks
template<typename ...Types>
void bench_all()
{
  bench_types<bench_fill_back, Types...>();
  bench_types<bench_emplace_back, Types...>();
  bench_types<bench_fill_front, Types...>();
  bench_types<bench_emplace_front, Types...>();
  bench_types<bench_linear_search, Types...>();
  bench_types<bench_random_insert, Types...>();
  bench_types<bench_random_remove, Types...>();
  bench_types<bench_sort, Types...>();
  bench_types<bench_destruction, Types...>();
  bench_types<bench_number_crunching, Types...>();
}


int main()
{
  bench_all<TrivialSmall, TrivialMedium, TrivialLarge, TrivialHuge, TrivialMonster,
            NonTrivialString, NonTrivialStringNoExcept, NonTrivialArray<32> >();
  graphs::output(graphs::Output::GOOGLE);
}
