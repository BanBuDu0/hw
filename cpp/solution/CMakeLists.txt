cmake_minimum_required(VERSION 3.15)
project(solution)

set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,1000000000")

add_executable(solution
        #        test0417.cpp
        #        commit0417_2.cpp
        #        test0418_4.cpp
        #        commit0418_3.cpp
        #        commit0419_2.cpp
        test0419_4.cpp
        )
find_package(Threads REQUIRED)
target_link_libraries(solution Threads::Threads)
