cmake_minimum_required(VERSION 2.8)

project("bw.com Articles")

#Set the version number
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

#Activate warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread -std=c++0x -pedantic -Wall -Wextra -Wno-long-long")

set(EXECUTABLE_OUTPUT_PATH bin)

add_executable(threads_p1_hello0 src/threads/part1/Hello0.cpp)
add_executable(threads_p1_hello1 src/threads/part1/Hello1.cpp)
add_executable(threads_p1_hello2 src/threads/part1/Hello2.cpp)

add_executable(threads_p2_counter1 src/threads/part2/Unsafe.cpp)
add_executable(threads_p2_counter2 src/threads/part2/Safe.cpp)
add_executable(threads_p2_counter3 src/threads/part2/Exception.cpp)
add_executable(threads_p2_counter4 src/threads/part2/SafeGuard.cpp)

target_link_libraries(threads_p1_hello0 rt)
target_link_libraries(threads_p1_hello1 rt)
target_link_libraries(threads_p1_hello2 rt)

target_link_libraries(threads_p2_counter1 rt)
target_link_libraries(threads_p2_counter2 rt)
target_link_libraries(threads_p2_counter3 rt)
target_link_libraries(threads_p2_counter4 rt)
