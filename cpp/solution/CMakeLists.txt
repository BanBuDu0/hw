cmake_minimum_required(VERSION 3.15)
project(solution)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,10000000")

add_executable(solution
        #        tarjan_baseline.cpp
        commit0409.cpp
#        commit0408.cpp
        #        update_v1.cpp
        )
