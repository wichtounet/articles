//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <boost/intrusive/list.hpp>

// create policies

//Create empty container

template<class Container>
struct Empty {
    inline static Container make(std::size_t) {
        return Container();
    }
    inline static void clean(){}
};

//Prepare data for fill back

template<class Container>
struct EmptyPrepareBackup {
    static std::vector<typename Container::value_type> v;
    inline static Container make(std::size_t size) {
        if(v.size() != size){
            v.clear();
            v.reserve(size);
            for(std::size_t i = 0; i < size; ++i){
                v.push_back({i});
            }
        }

        return Container();
    }

    inline static void clean(){
        v.clear();
        v.shrink_to_fit();
    }
};

template<class Container>
std::vector<typename Container::value_type> EmptyPrepareBackup<Container>::v;

template<class Container>
struct Filled {
    inline static Container make(std::size_t size) {
        return Container(size);
    }
    inline static void clean(){}
};

template<class Container>
struct FilledRandom {
    static std::vector<typename Container::value_type> v;
    inline static Container make(std::size_t size){
        // prepare randomized data that will have all the integers from the range
        if(v.size() != size){
            v.clear();
            v.reserve(size);
            for(std::size_t i = 0; i < size; ++i){
                v.push_back({i});
            }
            std::shuffle(begin(v), end(v), std::mt19937());
        }

        // fill with randomized data
        Container container;
        for(std::size_t i = 0; i < size; ++i){
            container.push_back(v[i]);
        }

        return container;
    }

    inline static void clean(){
        v.clear();
        v.shrink_to_fit();
    }
};

template<class Container>
std::vector<typename Container::value_type> FilledRandom<Container>::v;

template<class Container>
struct FilledRandomInsert {
    static std::vector<typename Container::value_type> v;
    inline static Container make(std::size_t size){
        // prepare randomized data that will have all the integers from the range
        if(v.size() != size){
            v.clear();
            v.reserve(size);
            for(std::size_t i = 0; i < size; ++i){
                v.push_back({i});
            }
            std::shuffle(begin(v), end(v), std::mt19937());
        }

        // fill with randomized data
        Container container;
        for(std::size_t i = 0; i < size; ++i){
            container.insert(v[i]);
        }

        return container;
    }

    inline static void clean(){
        v.clear();
        v.shrink_to_fit();
    }
};

template<class Container>
std::vector<typename Container::value_type> FilledRandomInsert<Container>::v;

template<class Container>
struct SmartFilled {
    inline static std::unique_ptr<Container> make(std::size_t size){
        return std::unique_ptr<Container>(new Container(size, typename Container::value_type()));
    }

    inline static void clean(){}
};

template<class Container>
struct BackupSmartFilled {
    static std::vector<typename Container::value_type> v;
    inline static std::unique_ptr<Container> make(std::size_t size){
        if(v.size() != size){
            v.clear();
            v.reserve(size);
            for(std::size_t i = 0; i < size; ++i){
                v.push_back({i});
            }
        }

        std::unique_ptr<Container> container(new Container());

        for(std::size_t i = 0; i < size; ++i){
            container->push_back(v[i]);
        }

        return container;
    }

    inline static void clean(){
        v.clear();
        v.shrink_to_fit();
    }
};

template<class Container>
std::vector<typename Container::value_type> BackupSmartFilled<Container>::v;

// testing policies

template<class Container>
struct NoOp {
    inline static void run(Container &, std::size_t) {
        //Nothing
    }
};

template<class Container>
struct ReserveSize {
    inline static void run(Container &c, std::size_t size){
        c.reserve(size);
    }
};

template<class Container>
struct InsertSimple {
    static const typename Container::value_type value;
    inline static void run(Container &c, std::size_t size){
        for(size_t i=0; i<size; ++i){
            c.insert(value);
        }
    }
};

template<class Container>
const typename Container::value_type InsertSimple<Container>::value{};

template<class Container>
struct FillBack {
    static const typename Container::value_type value;
    inline static void run(Container &c, std::size_t size){
        for(size_t i=0; i<size; ++i){
            c.push_back(value);
        }
    }
};

template <class Container>
const typename Container::value_type FillBack<Container>::value{};

template<class Container>
struct FillBackBackup {
    inline static void run(Container &c, std::size_t size){
        for(size_t i=0; i<size; ++i){
            c.push_back(EmptyPrepareBackup<Container>::v[i]);
        }
    }
};

template<class Container>
struct FillBackInserter {
    static const typename Container::value_type value;
    inline static void run(Container &c, std::size_t size){
        std::fill_n(std::back_inserter(c), size, value);
    }
};

template<class Container> const typename Container::value_type FillBackInserter<Container>::value{};

template<class Container>
struct EmplaceBack {
    inline static void run(Container &c, std::size_t size){
        for(size_t i=0; i<size; ++i){
            c.emplace_back();
        }
    }
};

template<class Container>
struct EmplaceInsertSimple {
    inline static void run(Container &c, std::size_t size){
        for(size_t i=0; i<size; ++i){
            c.emplace();
        }
    }
};

template<class Container>
struct FillFront {
    static const typename Container::value_type value;
    inline static void run(Container &c, std::size_t size){
        std::fill_n(std::front_inserter(c), size, value);
    }
};

template<class Container> const typename Container::value_type FillFront<Container>::value{};

template<class T>
struct FillFront<std::vector<T> > {
    static const T value;
    inline static void run(std::vector<T> &c, std::size_t size){
        for(std::size_t i=0; i<size; ++i){
            c.insert(begin(c), value);
        }
    }
};

template<class T> const T FillFront<std::vector<T> >::value{};

template<class Container>
struct EmplaceFront {
    inline static void run(Container &c, std::size_t size){
        for(size_t i=0; i<size; ++i){
            c.emplace_front();
        }
    }
};

template<class T>
struct EmplaceFront<std::vector<T> > {
    inline static void run(std::vector<T> &c, std::size_t size){
        for(std::size_t i=0; i<size; ++i){
            c.emplace(begin(c));
        }
    }
};

template<class Container>
struct Find {
    static size_t X;
    inline static void run(Container &c, std::size_t size){
        for(std::size_t i=0; i<size; ++i) {
            // hand written comparison to eliminate temporary object creation
            if(std::find_if(std::begin(c), std::end(c), [&](decltype(*std::begin(c)) v){ return v.a == i; }) == std::end(c)){
                ++X;
            }
        }
    }
};

template<class Container>
size_t Find<Container>::X = 0;

template<class Container>
struct Insert {
    static std::array<typename Container::value_type, 1000> values;
    inline static void run(Container &c, std::size_t){
        for(std::size_t i=0; i<1000; ++i) {
            // hand written comparison to eliminate temporary object creation
            auto it = std::find_if(std::begin(c), std::end(c), [&](decltype(*std::begin(c)) v){ return v.a == i; });
            c.insert(it, values[i]);
        }
    }
};

template<class Container>
std::array<typename Container::value_type, 1000> Insert<Container>::values {};

template<class Container>
struct Write {
    inline static void run(Container &c, std::size_t){
        auto it = std::begin(c);
        auto end = std::end(c);

        for(; it != end; ++it){
            ++(it->a);
        }
    }
};

template<class Container>
struct Iterate {
    inline static void run(Container &c, std::size_t){
        auto it = std::begin(c);
        auto end = std::end(c);

        while(it != end){
            ++it;
        }
    }
};

template<class Container>
struct Erase {
    inline static void run(Container &c, std::size_t){
        for(std::size_t i=0; i<1000; ++i) {
            // hand written comparison to eliminate temporary object creation
            c.erase(std::find_if(std::begin(c), std::end(c), [&](decltype(*std::begin(c)) v){ return v.a == i; }));
        }
    }
};

template<class Container>
struct RemoveErase {
    inline static void run(Container &c, std::size_t){
        // hand written comparison to eliminate temporary object creation
        c.erase(std::remove_if(begin(c), end(c), [&](decltype(*begin(c)) v){ return v.a < 1000; }), end(c));
    }
};

//Sort the container

template<class Container>
struct Sort {
    inline static void run(Container &c, std::size_t){
        std::sort(c.begin(), c.end());
    }
};

template<class T>
struct Sort<std::list<T> > {
    inline static void run(std::list<T> &c, std::size_t){
        c.sort();
    }
};

template<class T>
struct Sort<plf::colony<T> > {
    inline static void run(plf::colony<T> &c, std::size_t){
        c.sort();
    }
};

template<class T>
struct Sort<boost::intrusive::list<T, boost::intrusive::constant_time_size<false>>> {
    inline static void run(boost::intrusive::list<T, boost::intrusive::constant_time_size<false>>& c, std::size_t){
        c.sort();
    }
};

template<class Container>
struct TimSort {
    inline static void run(Container &c, std::size_t){
        c.timsort();
    }
};

//Reverse the container

template<class Container>
struct Reverse {
    inline static void run(Container &c, std::size_t){
        std::reverse(c.begin(), c.end());
    }
};

template<class T>
struct Reverse<std::list<T> > {
    inline static void run(std::list<T> &c, std::size_t){
        c.reverse();
    }
};

template<class T>
struct Reverse<boost::intrusive::list<T, boost::intrusive::constant_time_size<false>>> {
    inline static void run(boost::intrusive::list<T, boost::intrusive::constant_time_size<false>>& c, std::size_t){
        c.reverse();
    }
};

//Destroy the container

template<class Container>
struct SmartDelete {
    inline static void run(Container &c, std::size_t) { c.reset(); }
};

template<class Container>
struct RandomSortedInsert {
    static std::mt19937 generator;
    static std::uniform_int_distribution<std::size_t> distribution;

    inline static void run(Container &c, std::size_t size){
        for(std::size_t i=0; i<size; ++i){
            auto val = distribution(generator);
            // hand written comparison to eliminate temporary object creation
            c.insert(std::find_if(begin(c), end(c), [&](decltype(*begin(c)) v){ return v.a >= val; }), {val});
        }
    }
};

template<class Container> std::mt19937 RandomSortedInsert<Container>::generator;
template<class Container> std::uniform_int_distribution<std::size_t> RandomSortedInsert<Container>::distribution(0, std::numeric_limits<std::size_t>::max() - 1);

template<class Container>
struct RandomErase1 {
    static std::mt19937 generator;
    static std::uniform_int_distribution<std::size_t> distribution;

    inline static void run(Container &c, std::size_t /*size*/){
        auto it = c.begin();

        while(it != c.end()){
            if(distribution(generator) > 9900){
                it = c.erase(it);
            } else {
                ++it;
            }
        }

        std::cout << c.size() << std::endl;
    }
};

template<class Container> std::mt19937 RandomErase1<Container>::generator;
template<class Container> std::uniform_int_distribution<std::size_t> RandomErase1<Container>::distribution(0, 10000);

template<class Container>
struct RandomErase10 {
    static std::mt19937 generator;
    static std::uniform_int_distribution<std::size_t> distribution;

    inline static void run(Container &c, std::size_t /*size*/){
        auto it = c.begin();

        while(it != c.end()){
            if(distribution(generator) > 9000){
                it = c.erase(it);
            } else {
                ++it;
            }
        }
    }
};

template<class Container> std::mt19937 RandomErase10<Container>::generator;
template<class Container> std::uniform_int_distribution<std::size_t> RandomErase10<Container>::distribution(0, 10000);

template<class Container>
struct RandomErase25 {
    static std::mt19937 generator;
    static std::uniform_int_distribution<std::size_t> distribution;

    inline static void run(Container &c, std::size_t /*size*/){
        auto it = c.begin();

        while(it != c.end()){
            if(distribution(generator) > 7500){
                it = c.erase(it);
            } else {
                ++it;
            }
        }
    }
};

template<class Container> std::mt19937 RandomErase25<Container>::generator;
template<class Container> std::uniform_int_distribution<std::size_t> RandomErase25<Container>::distribution(0, 10000);

template<class Container>
struct RandomErase50 {
    static std::mt19937 generator;
    static std::uniform_int_distribution<std::size_t> distribution;

    inline static void run(Container &c, std::size_t /*size*/){
        auto it = c.begin();

        while(it != c.end()){
            if(distribution(generator) > 5000){
                it = c.erase(it);
            } else {
                ++it;
            }
        }
    }
};

template<class Container> std::mt19937 RandomErase50<Container>::generator;
template<class Container> std::uniform_int_distribution<std::size_t> RandomErase50<Container>::distribution(0, 10000);

// Note: This is probably erased completely for a vector
template<class Container>
struct Traversal {
    inline static void run(Container &c, std::size_t size){
        auto it = c.begin();
        auto end = c.end();

        while(it != end){
            ++it;
        }
    }
};
