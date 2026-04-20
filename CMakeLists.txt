cmake_minimum_required(VERSION 3.30)
project(PEA1)

set(CMAKE_CXX_STANDARD 20)

add_executable(PEA1
        PEA1/main.cpp
        PEA1/Matrix.cpp
        PEA1/Generator.cpp
        PEA1/NearestNeighbour.cpp
        PEA1/Random.cpp
        PEA1/BruteForce.cpp
        PEA1/RepetativeNearestNeighbour.cpp
        PEA1/FileReader.cpp
)

add_executable(PEA2
        PEA2/main.cpp
        PEA2/Matrix.cpp
        PEA2/Generator.cpp
        PEA2/FileReader.cpp
        PEA2/BranchAndBound.cpp
        PEA2/BruteForce.cpp
        PEA2/NearestNeighbour.cpp
)