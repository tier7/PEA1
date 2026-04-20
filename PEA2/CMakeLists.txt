cmake_minimum_required(VERSION 3.27)
project(PEA2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(PEA2
        main.cpp
        Matrix.cpp
        Generator.cpp
        FileReader.cpp
        BruteForce.cpp
        NearestNeighbour.cpp
        BranchAndBound.cpp
)
