cmake_minimum_required(VERSION 3.15)
project(solution)

set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,1000000000")

add_executable(solution
        test0416_3.cpp)
find_package(Threads REQUIRED)
target_link_libraries(solution Threads::Threads)
