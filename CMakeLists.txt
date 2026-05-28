cmake_minimum_required(VERSION 3.20)

project(wichtounet_articles LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------
# Options / defines
# -------------------------
set(SQRT_VALUE 5000000)
add_compile_definitions(SQRT_VALUE=${SQRT_VALUE})

include_directories(
    include
    Catch/include
    plf_colony_alpha
)

# -------------------------
# Boost
# -------------------------
find_package(Boost CONFIG REQUIRED COMPONENTS program_options)

# -------------------------
# Common flags
# -------------------------
if (MSVC)
    # not used here, but keeps CMake happy
endif()

# pthread equivalent handling
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# -------------------------
# Helper function
# -------------------------
function(add_benchmark name)
    add_executable(${name} ${ARGN})
    target_compile_options(${name}
      PRIVATE
        # -Wall -Wextra # noisy
        # -Wno-missing-field-initializers
    )
    target_link_libraries(${name}
        PRIVATE
        Threads::Threads
        Boost::headers
    )
endfunction()

# -------------------------
# vector_list executables
# -------------------------
add_benchmark(vector_list
    src/vector_list/bench.cpp
    src/graphs.cpp
    src/demangle.cpp
)

add_benchmark(vector_list_update_1
    src/vector_list_update_1/bench.cpp
    src/graphs.cpp
    src/demangle.cpp
)

# -------------------------
# intrusive_list
# -------------------------
add_benchmark(intrusive_list
    src/intrusive_list/bench.cpp
    src/graphs.cpp
    src/demangle.cpp
)

# -------------------------
# boost_po
# -------------------------
add_benchmark(boost_po_v1 src/boost_po/v1.cpp)
target_link_libraries(boost_po_v1 PRIVATE Boost::program_options)

# -------------------------
# threads benchmarks
# -------------------------
add_benchmark(threads_bench src/threads/benchmark/bench.cpp)

add_benchmark(threads_p1_hello0 src/threads/part1/Hello0.cpp)
add_benchmark(threads_p1_hello1 src/threads/part1/Hello1.cpp)
add_benchmark(threads_p1_hello2 src/threads/part1/Hello2.cpp)

add_benchmark(threads_p2_counter1 src/threads/part2/Unsafe.cpp)
add_benchmark(threads_p2_counter2 src/threads/part2/Safe.cpp)
add_benchmark(threads_p2_counter3 src/threads/part2/Exception.cpp)
add_benchmark(threads_p2_counter4 src/threads/part2/SafeGuard.cpp)

add_benchmark(threads_p3_recursive src/threads/part3/recursive_problem.cpp)
add_benchmark(threads_p3_recursive2 src/threads/part3/recursive_mutex.cpp)
add_benchmark(threads_p3_timed src/threads/part3/timed_mutex.cpp)
add_benchmark(threads_p3_call_once src/threads/part3/call_once.cpp)
add_benchmark(threads_p3_condition_variables src/threads/part3/condition_variables.cpp)

add_benchmark(threads_p4_atomic_counter src/threads/part4/AtomicCounter.cpp)

add_benchmark(threads_p5_futures src/threads/part5/futures.cpp)
add_benchmark(threads_p5_futures_ret src/threads/part5/futures_ret.cpp)
add_benchmark(threads_p5_futures_wait_for src/threads/part5/futures_wait_for.cpp)
add_benchmark(threads_p5_futures_loop src/threads/part5/futures_loop.cpp)

# -------------------------
# sqrt benchmarks
# -------------------------
add_benchmark(sqrt_constexpr src/sqrt/constexpr.cpp)
# fix: error: ‘constexpr’ evaluation depth exceeds maximum of 512
target_compile_options(sqrt_constexpr PRIVATE -fconstexpr-depth=10000)
add_benchmark(sqrt_smart_constexpr src/sqrt/smart_constexpr.cpp)
add_benchmark(sqrt_tmp src/sqrt/tmp.cpp)
add_benchmark(sqrt_smart_tmp src/sqrt/smart_tmp.cpp)

# -------------------------
# linear sorting
# -------------------------
add_benchmark(linear_sorting src/linear_sorting/bench.cpp)

# -------------------------
# named templates
# -------------------------
add_benchmark(named_tmp src/named_template_par/configurable.cpp)
