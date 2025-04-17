cmake_minimum_required(VERSION 3.27)
project(FlixHabit)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(FlixHabit
        src/User.h
        test/test.cpp
        src/Graph.cpp
        src/Graph.h
        src/MinHeap.h
        src/MinHeap.cpp
        src/main.cpp)
