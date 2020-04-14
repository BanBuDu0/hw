cmake_minimum_required(VERSION 3.15)
project(solution)

set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,1000000000")

add_executable(solution
        #        tarjan_baseline.cpp
        #        commit0409_2.cpp
        #        commit0411.cpp
        #        commit0411_2.cpp
        #        test0411.cpp
        #        commit0412.cpp
        #        test2.cpp
        #        test0411.cpp
        #        commit0412.cpp
        #        commit0413_!.cpp
        #        test0412_3.cpp
        #        commit0408.cpp
        #        update_v1.cpp
        #        commit0413_2.cpp
        #        commit0414_1.cpp
#        commit0414_2.cpp

        test0414.cpp)
find_package(Threads REQUIRED)
target_link_libraries(solution Threads::Threads)
