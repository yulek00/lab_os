cmake_minimum_required(VERSION 3.10)
project(ProcessDivision)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

if(WIN32)
    add_executable(parent 
        app/parent_main.cpp
        src/parent.cpp
        src/os_win.cpp 
    )

    add_executable(child 
        app/child_main.cpp
        src/child.cpp
    )
else()
    add_executable(parent 
        app/parent_main.cpp
        src/parent.cpp
        src/os.cpp    
    )

    add_executable(child 
        app/child_main.cpp
        src/child.cpp
    )
endif()