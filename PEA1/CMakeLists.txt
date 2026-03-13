cmake_minimum_required(VERSION 3.30)
project(PEA1)

set(CMAKE_CXX_STANDARD 20)

add_executable(PEA1 main.cpp
        Matrix.cpp
        Matrix.h
        Generator.cpp
        Generator.h
        NearestNeighbour.cpp
        NearestNeighbour.h
        AlgResults.h)
