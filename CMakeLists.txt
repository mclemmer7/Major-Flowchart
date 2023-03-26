cmake_minimum_required(VERSION 3.16.3)
project(flowchart_the_fellowship)

set(CMAKE_CXX_STANDARD 14)

<<<<<<< HEAD
add_executable(flowchart_the_fellowship
        main.cpp)
include_directories(lemon-1.3.1)
include_directories(lemon-1.3.1/build)
=======
include_directories(./lemon-1.3.1)

include_directories(./lemon-1.3.1/build)

add_executable(flowchart_the_fellowship
        main.cpp coursenode.h coursenode.cpp)
>>>>>>> Mark's-Work
