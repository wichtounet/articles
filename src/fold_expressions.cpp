#include <iostream>

auto old_sum(){
    return 0;
}

template<typename T1, typename... T>
auto old_sum(T1 s, T... ts){
    return s + old_sum(ts...);;
}

auto old_mul(){
    return 1;
}

template<typename T1, typename... T>
auto old_mul(T1 s, T... ts){
    return s + old_mul(ts...);;
}

template<typename... T>
auto fold_sum_1(T... s){
    return (s + ...);
}

template<typename... T>
auto fold_sum_2(T... s){
    return (s + ... + 10);
}

template<typename... T>
auto fold_mul_1(T... s){
    return (s * ...);
}

template<typename... T>
auto fold_mul_2(T... s){
    return (s * ... * 10);
}

template<std::size_t... V>
struct fold_tmp_mul : std::integral_constant<std::size_t, (V * ...)> {};

template<bool... B>
struct fold_and : std::integral_constant<bool, (B && ...)> {};

template<bool... B>
struct fold_or : std::integral_constant<bool, (B || ...)> {};

template<typename ...Args>
void print_1(Args&&... args) {
    (std::cout << ... << args) << '\n';
}

template<typename ...Args>
void print_2(Args&&... args) {
    (print_1(args), ...);
}

int main(){
    std::cout << old_sum(1, 2, 3, 4, 5) << std::endl;
    std::cout << old_sum() << std::endl;

    std::cout << fold_sum_1(1, 2, 3, 4, 5) << std::endl;
    std::cout << fold_sum_2(1, 2, 3, 4, 5) << std::endl;
    std::cout << fold_sum_1() << std::endl;
    std::cout << fold_sum_2() << std::endl;

    std::cout << old_mul(1, 2, 3, 4, 5) << std::endl;
    std::cout << old_mul() << std::endl;

    std::cout << fold_mul_1(1, 2, 3, 4, 5) << std::endl;
    std::cout << fold_mul_2(1, 2, 3, 4, 5) << std::endl;
    std::cout << fold_mul_1() << std::endl;
    std::cout << fold_mul_2() << std::endl;

    std::cout << fold_tmp_mul<1, 2, 3, 4, 5>::value << std::endl;

    std::cout << fold_and<true, false, true>::value << std::endl;
    std::cout << fold_or<true, false, true>::value << std::endl;

    print_1(1,"cout",'Z',1.0f);
    print_2(1,"cout",'Z',1.0f);

    return 0;
}
