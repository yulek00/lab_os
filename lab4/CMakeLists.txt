cmake_minimum_required(VERSION 3.10)
project(Lab4_DynamicLibraries)

set(CMAKE_CXX_STANDARD 17)
set(HEADER_DIR "include")

set(GCF_EUCLID_SRC src/gcf_euclid.cpp)
set(GCF_NAIVE_SRC src/gcf_naive.cpp)
set(SQUARE_RECT_SRC src/square_rect.cpp)
set(SQUARE_TRIANGLE_SRC src/square_triangle.cpp)

add_library(gcf_euclid SHARED ${GCF_EUCLID_SRC})
add_library(gcf_naive SHARED ${GCF_NAIVE_SRC})
add_library(square_rect SHARED ${SQUARE_RECT_SRC})
add_library(square_triangle SHARED ${SQUARE_TRIANGLE_SRC})

target_include_directories(gcf_euclid PUBLIC ${HEADER_DIR})
target_include_directories(gcf_naive PUBLIC ${HEADER_DIR})
target_include_directories(square_rect PUBLIC ${HEADER_DIR})
target_include_directories(square_triangle PUBLIC ${HEADER_DIR})

add_executable(prog_static main_static.cpp)
target_link_libraries(prog_static gcf_euclid square_rect)
target_include_directories(prog_static PUBLIC ${HEADER_DIR})

add_executable(prog_dynamic main_dynamic.cpp)
target_link_libraries(prog_dynamic dl)
target_include_directories(prog_dynamic PUBLIC ${HEADER_DIR})