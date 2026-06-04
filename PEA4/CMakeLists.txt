cmake_minimum_required(VERSION 3.27)
project(PEA4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(PEA4
        main.cpp
        Matrix.cpp
        Generator.cpp
        FileReader.cpp
        NearestNeighbour.cpp
        GeneticAlgorithm.cpp
)