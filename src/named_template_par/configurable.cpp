//=======================================================================
// Copyright (c) 2015 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <type_traits>
#include <iostream>

enum class type {
    AAA,
    BBB,
    CCC
};

struct watcher_1 {
    static std::string name(){
        return "watcher_1";
    }
};
struct watcher_2 {
    static std::string name(){
        return "watcher_2";
    }
};

template<typename C>
struct trainer_1 {
    static std::string name(){
        return "trainer_1";
    }
};

template<typename C>
struct trainer_2 {
    static std::string name(){
        return "trainer_2";
    }
};

template<int T_A = 1, char T_B = 'b', type T_C = type::BBB, bool T_D = false, typename T_E = watcher_1, template<typename> class T_F = trainer_1>
struct configurable_v1 {
    static constexpr const int A = T_A;
    static constexpr const char B = T_B;
    static constexpr const type C = T_C;
    static constexpr const bool D = T_D;

    using E = T_E;

    template<typename C>
    using F = T_F<C>;

    //Something useful
};

struct a_id;
struct b_id;
struct c_id;
struct d_id;
struct e_id;
struct f_id;

//template<int value>
//struct a : std::integral_constant<int, value> {
    //using type_id = a_id;
//};

//template<int value>
//struct b : std::integral_constant<char, value> {
    //using type_id = b_id;
//};

//template<type value>
//struct c : std::integral_constant<type, value> {
    //using type_id = c_id;
//};

//struct d {
    //using type_id = d_id;
//};

template<typename ID, typename T, T value>
struct value_conf_t : std::integral_constant<T, value> {
    using type_id = ID;
};

template<typename ID>
struct basic_conf_t {
    using type_id = ID;
};

template<typename ID, typename T>
struct type_conf_t {
    using type_id = ID;
    using value = T;
};

template<typename ID, template<typename> class T>
struct template_type_conf_t {
    using type_id = ID;
    
    template<typename C>
    using value = T<C>;
};

template<int value>
struct a : value_conf_t<a_id, int, value> {};

template<char value>
struct b : value_conf_t<b_id, char, value> {};

template<type value>
struct c : value_conf_t<c_id, type, value> {};

struct d : basic_conf_t<d_id> {};

template<typename T>
struct e : type_conf_t<e_id, T> {};

template<template<typename> class T>
struct f : template_type_conf_t<f_id, T> {};


template<typename D, typename... Args>
struct get_value_int;

template<typename D>
struct get_value_int<D> : std::integral_constant<int, D::value> {};

template<typename D, typename T2, typename... Args>
struct get_value_int<D, T2, Args...> {
    template<typename D2, typename T22, typename Enable = void>
    struct impl 
        : std::integral_constant<int, get_value_int<D, Args...>::value> {};

    template<typename D2, typename T22>
    struct impl <D2, T22, std::enable_if_t<std::is_same<typename D2::type_id, typename T22::type_id>::value>> 
        : std::integral_constant<int, T22::value> {};

    static constexpr const int value = impl<D, T2>::value;
};


template<typename D, typename... Args>
struct get_value;

template<typename D, typename T2, typename... Args>
struct get_value<D, T2, Args...> {
    template<typename D2, typename T22, typename Enable = void>
    struct impl 
        : std::integral_constant<decltype(D::value), get_value<D, Args...>::value> {};

    template<typename D2, typename T22>
    struct impl <D2, T22, std::enable_if_t<std::is_same<typename D2::type_id, typename T22::type_id>::value>> 
        : std::integral_constant<decltype(D::value), T22::value> {};

    static constexpr const auto value = impl<D, T2>::value;
};

template<typename D>
struct get_value<D> : std::integral_constant<decltype(D::value), D::value> {};


template<typename D, typename... Args>
struct get_type;

template<typename D, typename T2, typename... Args>
struct get_type<D, T2, Args...> {
    template<typename D2, typename T22, typename Enable = void>
    struct impl {
        using value = typename get_type<D, Args...>::value;
    };

    template<typename D2, typename T22>
    struct impl <D2, T22, std::enable_if_t<std::is_same<typename D2::type_id, typename T22::type_id>::value>> {
        using value = typename T22::value;
    };

    using value = typename impl<D, T2>::value;
};

template<typename D>
struct get_type<D> {
    using value = typename D::value;
};


template<typename D, typename... Args>
struct get_template_type;

template<typename D, typename T2, typename... Args>
struct get_template_type<D, T2, Args...> {
    template<typename D2, typename T22, typename Enable = void>
    struct impl {
        template<typename C>
        using value = typename get_template_type<D, Args...>::template value<C>;
    };

    template<typename D2, typename T22>
    struct impl <D2, T22, std::enable_if_t<std::is_same<typename D2::type_id, typename T22::type_id>::value>> {
        template<typename C>
        using value = typename T22::template value<C>;
    };

    template<typename C>
    using value = typename impl<D, T2>::template value<C>;
};

template<typename D>
struct get_template_type<D> {
    template<typename C>
    using value = typename D::template value<C>;
};



template<typename T1, typename... Args>
struct is_present;

template<typename T1, typename T2, typename... Args>
struct is_present<T1, T2, Args...> : std::integral_constant<bool, std::is_same<T1, T2>::value || is_present<T1, Args...>::value> {};

template<typename T1>
struct is_present<T1> : std::false_type {};


template<typename... Valid>
struct tmp_list {
    template<typename T>
    struct contains : std::integral_constant<bool, is_present<typename T::type_id, Valid...>::value> {};
};

template<typename L, typename... Args>
struct is_valid;

template<typename L, typename T1, typename... Args>
struct is_valid <L, T1, Args...> : std::integral_constant<bool, L::template contains<T1>::value && is_valid<L, Args...>::value> {};

template<typename L>
struct is_valid <L> : std::true_type {};



template<typename... Args>
struct configurable_v2 {
    //static constexpr const int A = get_value_int<a<1>, Args...>::value;

    static constexpr const auto A = get_value<a<1>, Args...>::value;
    static constexpr const auto B = get_value<b<'b'>, Args...>::value;
    static constexpr const auto C = get_value<c<type::BBB>, Args...>::value;
    static constexpr const auto D = is_present<d, Args...>::value;

    using E = typename get_type<e<watcher_1>, Args...>::value;

    template<typename C>
    using F = typename get_template_type<f<trainer_1>, Args...>::template value<C>;

    static_assert(
        is_valid<tmp_list<a_id, b_id, c_id, d_id, e_id, f_id>, Args...>::value,
        "Invalid parameters type");

    //Something useful
};

int main(){
    using configurable_v1_t = configurable_v1<100, 'z', type::CCC, true, watcher_2, trainer_2>;
    using configurable_v2_t = configurable_v2<a<100>, b<'z'>, c<type::CCC>, d, e<watcher_2>, f<trainer_2>>;

    std::cout << configurable_v1_t::A << std::endl;
    std::cout << configurable_v2_t::A << std::endl;
    std::cout << configurable_v1_t::B << std::endl;
    std::cout << configurable_v2_t::B << std::endl;
    std::cout << static_cast<std::size_t>(configurable_v1_t::C) << std::endl;
    std::cout << static_cast<std::size_t>(configurable_v2_t::C)<< std::endl;
    std::cout << configurable_v1_t::D << std::endl;
    std::cout << configurable_v2_t::D << std::endl;
    std::cout << configurable_v1_t::E::name() << std::endl;
    std::cout << configurable_v2_t::E::name() << std::endl;
    std::cout << configurable_v1_t::template F<int>::name() << std::endl;
    std::cout << configurable_v2_t::template F<int>::name() << std::endl;

    //TODO

    return 0;
}
