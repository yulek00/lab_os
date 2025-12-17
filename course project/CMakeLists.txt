cmake_minimum_required(VERSION 3.10)

project(BullsAndCowsServerClient CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

set(COMMON_SOURCES
    src/common/GameMessages.cpp
    src/common/NumberLogic.cpp
    src/common/PosixPipe.cpp
    src/common/threads.cpp
)

set(SERVER_SOURCES
    src/server/GameServer.cpp
    src/server/main_server.cpp
    ${COMMON_SOURCES}
)

add_executable(main_server ${SERVER_SOURCES})
target_link_libraries(main_server pthread)

set(CLIENT_SOURCES
    src/client/GameClient.cpp
    src/client/main_client.cpp
    ${COMMON_SOURCES}
)

add_executable(main_client ${CLIENT_SOURCES})
target_link_libraries(main_client pthread)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")