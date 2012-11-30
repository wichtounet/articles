cmake_minimum_required(VERSION 2.8)

project("bw.com Articles")

#Set the version number
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

#Activate warnings
set(CMAKE_CXX_FLAGS             "${CMAKE_CXX_FLAGS} -g -pthread -std=c++11 -pedantic -Wall -Wextra -Wno-long-long")
set(CMAKE_CXX_FLAGS_DEBUG       "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE     "-std=c++11 -pthread -DNDEBUG -O2 -fomit-frame-pointer -march=native")

set(EXECUTABLE_OUTPUT_PATH bin)

# All the headers are in the include directory
include_directories(include)

#Threads Part 1
add_executable(threads_p1_hello0 src/threads/part1/Hello0.cpp)
add_executable(threads_p1_hello1 src/threads/part1/Hello1.cpp)
add_executable(threads_p1_hello2 src/threads/part1/Hello2.cpp)

target_link_libraries(threads_p1_hello0 rt)
target_link_libraries(threads_p1_hello1 rt)
target_link_libraries(threads_p1_hello2 rt)

#Threads Part 2 
add_executable(threads_p2_counter1 src/threads/part2/Unsafe.cpp)
add_executable(threads_p2_counter2 src/threads/part2/Safe.cpp)
add_executable(threads_p2_counter3 src/threads/part2/Exception.cpp)
add_executable(threads_p2_counter4 src/threads/part2/SafeGuard.cpp)

target_link_libraries(threads_p2_counter1 rt)
target_link_libraries(threads_p2_counter2 rt)
target_link_libraries(threads_p2_counter3 rt)
target_link_libraries(threads_p2_counter4 rt)

#Threads Part 3
add_executable(threads_p3_recursive1 src/threads/part3/recursive_problem.cpp)
add_executable(threads_p3_recursive2 src/threads/part3/recursive_mutex.cpp)
add_executable(threads_p3_timed src/threads/part3/timed_mutex.cpp)
add_executable(threads_p3_call_once src/threads/part3/call_once.cpp)
add_executable(threads_p3_condition_variables src/threads/part3/condition_variables.cpp)

target_link_libraries(threads_p3_recursive1 rt)
target_link_libraries(threads_p3_recursive2 rt)
target_link_libraries(threads_p3_timed rt)
target_link_libraries(threads_p3_call_once rt)
target_link_libraries(threads_p3_condition_variables rt)

#Threads Part 4
add_executable(threads_p4_atomic_counter src/threads/part4/AtomicCounter.cpp)

target_link_libraries(threads_p4_atomic_counter rt)

#Threads Benchmark
add_executable(threads_bench src/threads/benchmark/bench.cpp)

target_link_libraries(threads_bench rt)

#Linear Time Sorting
add_executable(linear_bench src/linear_sorting/bench.cpp)

target_link_libraries(linear_bench rt)

#Vector vs List
add_executable(vector_list src/vector_list/bench.cpp src/graphs.cpp)

target_link_libraries(vector_list rt)

#Boost Program Options

add_executable(boost_po_v1 src/boost_po/v1.cpp)

target_link_libraries (boost_po_v1 boost_program_options)
