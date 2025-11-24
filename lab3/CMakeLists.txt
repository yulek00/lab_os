cmake_minimum_required(VERSION 3.10)
project(lab3)

add_executable(parent 
    app/parent_main.cpp
    src/parent.cpp
    src/os.cpp
)

add_executable(child
    app/child_main.cpp
    src/child.cpp
    src/os.cpp
)

target_include_directories(parent PRIVATE include)
target_include_directories(child PRIVATE include)

if(UNIX AND NOT APPLE)
    target_link_libraries(parent -lrt)
    target_link_libraries(child -lrt)
endif()