cmake_minimum_required(VERSION 3.10)
project(monte_carlo CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(thread_lib STATIC src/threads.cpp)


target_include_directories(thread_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(thread_lib PUBLIC Threads::Threads)

add_executable(monte_carlo main.cpp)

target_link_libraries(monte_carlo PRIVATE thread_lib)