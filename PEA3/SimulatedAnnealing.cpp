//
// Created by ja on 16.05.2026.
//

#include "SimulatedAnnealing.h"
#include "Matrix.h"
#include <vector>
#include <random>
#include "NearestNeighbour.h"
#include <algorithm>
#include <filesystem>
#include <limits>
#include "AlgResults.h"
#include <chrono>
#include <cmath>


std::vector<int> SimulatedAnnealing::initialSolution(InitialSolutionType type, int start, const Matrix &matrix) {
    std::vector<int> path;
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

int SimulatedAnnealing::calculateCost(const Matrix &matrix, const std::vector<int> &path) {
    if (path.front() != path.back()) {
        return std::numeric_limits<int>::max();
    }

    int cost = 0;
    for (int i = 0; i < static_cast<int>(path.size()) - 1; i++) {
        int edge_cost = matrix.get(path[i], path[i + 1]);

        if (edge_cost == -1) {
            return std::numeric_limits<int>::max();
        }
        cost += edge_cost;
    }

    return cost;
}

std::vector<int> generatePermutation(const std::vector<int> &path, std::mt19937 &gen) {
    std::vector<int> new_path = path;
    if (path.size() <= 3) {
        return new_path;
    }
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
    if (new_cost == std::numeric_limits<int>::max()) {
        return 0.0;
    }

    if (current_cost == std::numeric_limits<int>::max()) {
        return 1.0;
    }

    if (new_cost < current_cost) {
        return 1.0;
    }

    if (temperature <= 0.0) {
        return 0.0;
    }

    return std::exp(-(static_cast<double>(new_cost - current_cost) / temperature));
}

double coolTemp(const Parameters &params, double temperature) {
    if (params.cooling_scheme == LINEAR) {
        if (params.cooling <= 0.0) {
            throw invalid_argument("bledna wartosc chlodzenia");
        }
        temperature -= params.cooling;
        return temperature;
    }
    else if (params.cooling_scheme == EXPONENTIAL) {
        if (params.cooling <= 0.0 || params.cooling >= 1.0) {
            throw invalid_argument("bledna wartosc chlodzenia");
        }
        temperature *= params.cooling;
        return temperature;
    }

    return temperature;
}

AlgResults SimulatedAnnealing::SA(const Matrix &matrix, int start, InitialSolutionType initType, const Parameters &params) {
    auto start_time = std::chrono::high_resolution_clock::now();
    bool time_exceeded = false;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> val(0.0, 1.0);
    std::vector<int> current_path = initialSolution(initType, start, matrix);
    int current_cost = calculateCost(matrix, current_path);
    std::vector<int> best_path = current_path;
    int best_cost = current_cost;
    double temperature = params.start_temperature;
    while (temperature > params.end_temperature && !time_exceeded ) {
        for (int i=0; i<params.epochs; i++) {
            std::vector<int> new_path = generatePermutation(current_path, gen);
            int new_cost = calculateCost(matrix, new_path);
            double accept_chance = acceptProbability(current_cost, new_cost, temperature);
            double random = val(gen);
            if (accept_chance > random) {
                current_cost = new_cost;
                current_path = new_path;
                if (current_cost < best_cost) {
                    best_cost = current_cost;
                    best_path = current_path;
                }
            }
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();

            if (elapsed >= params.time_limit) {
                time_exceeded = true;
                break;
            }
        }
        temperature = coolTemp(params, temperature);
    }
    return AlgResults(best_cost, best_path, time_exceeded);

}
