//
// Created by ja on 16.05.2026.
//

#include "SimulatedAnnealing.h"
#include "Matrix.h"
#include <vector>
#include <random>
#include "NearestNeighbour.h"
#include <algorithm>
#include <limits>
#include "AlgResults.h"


std::vector<int> SimulatedAnnealing::initialSolution(InitialSolutionType type, int start, const Matrix &matrix) {
    vector<int> path;
    path.push_back(start);

    if (type == RANDOM) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::vector<int> vertex_list;
        for (int i = 0; i < matrix.getSize(); i++) {
            if (i != start) {
                vertex_list.push_back(i);
            }
        }
        std::shuffle(vertex_list.begin(), vertex_list.end(), gen);
        path.insert(path.end(), vertex_list.begin(), vertex_list.end());
        path.push_back(start);
    }
    else {
        path = NearestNeighbour::NN(matrix, start).path;
    }
    return path;
}

int calculateCost(const Matrix &matrix, const std::vector<int> &path) {
    int cost = 0;
    for (int i = 0; i < path.size()-1; i++) {
         int edge_cost = matrix.get(path[i], path[i+1]);
         if (edge_cost == -1) {
           return std::numeric_limits<int>::max();
         }
         cost += matrix.get(path[i], path[i+1]);

    }
    return cost;
}

std::vector<int> generatePermutation(const std::vector<int> &path) {
    std::vector<int> new_path = path;
    if (path.size() <= 3) {
        return new_path;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> range(1, path.size()-2);
    int val1 = range(gen);
    int val2 = range(gen);
    while (val1 == val2) {
      val2 = range(gen);
    }
    std::swap(new_path[val1], new_path[val2]);
    return new_path;
}

double acceptProbability(int current_cost, int new_cost, double temperature){
    if (new_cost < current_cost) {
         return 1.0;

    }
    if (temperature <= current_cost) {
        return 0.0;
    }

    return std::exp(-(static_cast<double>(new_cost - current_cost) / temperature));
}

AlgResults SimulatedAnnealing::SA(const Matrix &matrix, int start, InitialSolutionType initType) {
    std::vector<int> path = initialSolution(initType, start, matrix);
    calculateCost(matrix, path);
    std::vector<int> best_path = path;

}
