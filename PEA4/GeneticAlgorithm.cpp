//
// Created by ja on 22.05.2026.
//

#include "GeneticAlgorithm.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <limits>

std::vector<int> GeneticAlgorithm::initialSolution(int start, const Matrix &matrix) {
    std::vector<int> path;
    std::vector<int> vertex_list;

    path.push_back(start);

    for (int i = 0; i < matrix.getSize(); i++) {
        if (i != start) {
            vertex_list.push_back(i);
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(vertex_list.begin(), vertex_list.end(), gen);
    path.insert(path.end(), vertex_list.begin(), vertex_list.end());
    path.push_back(start);
    return path;
}

int GeneticAlgorithm::calculateCost(const Matrix &matrix, const std::vector<int> &path) {
    if (path.empty()) {
        return std::numeric_limits<int>::max();
    }

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

std::vector<Individual> GeneticAlgorithm::generateInitialPopulation(const Matrix &matrix, int start, int population_size) {
    std::vector<Individual> population;

    for (int i = 0; i < population_size; i++) {
        Individual individual;
        individual.path = initialSolution(start, matrix);
        individual.cost = calculateCost(matrix, individual.path);
        population.push_back(individual);
    }

    return population;
}

Individual GeneticAlgorithm::tournamentSelection(const std::vector<Individual> &population, const GeneticParameters &params) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> range(0, static_cast<int>(population.size()) - 1);

    Individual best = population[range(gen)];

    for (int i = 1; i < params.tournament_size; i++) {
        Individual candidate = population[range(gen)];
        if (candidate.cost < best.cost) {
            best = candidate;
        }
    }

    return best;
}

std::vector<int> GeneticAlgorithm::orderCrossover(const std::vector<int> &parent1, const std::vector<int> &parent2, int start) {
    int path_size = static_cast<int>(parent1.size());
    std::vector<int> child(path_size, -1);
    child[0] = start;
    child[path_size - 1] = start;

    if (path_size <= 3) {
        return parent1;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> range(1, path_size - 2);

    int cut1 = range(gen);
    int cut2 = range(gen);
    if (cut1 > cut2) {
        std::swap(cut1, cut2);
    }

    for (int i = cut1; i <= cut2; i++) {
        child[i] = parent1[i];
    }

    int child_index = cut2 + 1;
    if (child_index == path_size - 1) {
        child_index = 1;
    }

    for (int i = 1; i < path_size - 1; i++) {
        int parent2_index = cut2 + i;
        if (parent2_index >= path_size - 1) {
            parent2_index -= (path_size - 2);
        }
        int city = parent2[parent2_index];

        bool already_exists = false;
        for (int j = 1; j < path_size - 1; j++) {
            if (child[j] == city) {
                already_exists = true;
                break;
            }
        }
        if (!already_exists) {
            child[child_index] = city;
            child_index++;

            if (child_index == path_size - 1) {
                child_index = 1;
            }
        }
    }

    return child;
}

void GeneticAlgorithm::swapMutation(std::vector<int> &path) {
    if (path.size() <= 3) {
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> range(1, static_cast<int>(path.size()) - 2);

    int index1 = range(gen);
    int index2 = range(gen);
    while (index1 == index2) {
        index2 = range(gen);
    }

    std::swap(path[index1], path[index2]);
}

void GeneticAlgorithm::inversionMutation(std::vector<int> &path) {
    if (path.size() <= 3) {
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> range(1, static_cast<int>(path.size()) - 2);

    int index1 = range(gen);
    int index2 = range(gen);
    if (index1 > index2) {
        std::swap(index1, index2);
    }

    std::reverse(path.begin() + index1, path.begin() + index2 + 1);
}

double GeneticAlgorithm::randomVal() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> range(0.0, 1.0);

    return range(gen);
}

AlgResults GeneticAlgorithm::GA(const Matrix &matrix, int start, const GeneticParameters &params) {
    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<Individual> population = generateInitialPopulation(matrix, start, params.population_size);

    Individual best_individual;
    best_individual.cost = std::numeric_limits<int>::max();

    for (int i = 0; i < static_cast<int>(population.size()); i++) {
        if (population[i].cost < best_individual.cost) {
            best_individual = population[i];
        }
    }

    auto after_initial = std::chrono::high_resolution_clock::now();
    long long time_to_best_ms = std::chrono::duration_cast<std::chrono::milliseconds>(after_initial - start_time).count();
    bool stop = false;

    if (params.optimal_cost > 0 && best_individual.cost <= params.optimal_cost) {
        stop = true;
    }

    while (!stop) {
        std::vector<Individual> new_population;
        new_population.push_back(best_individual);

        while (static_cast<int>(new_population.size()) < params.population_size) {
            Individual parent1 = tournamentSelection(population, params);
            Individual parent2 = tournamentSelection(population, params);
            Individual child;

            if (randomVal() < params.crossover_rate) {
                child.path = orderCrossover(parent1.path, parent2.path, start);
            }
            else {
                child.path = parent1.path;
            }

            if (randomVal() < params.mutation_rate) {
                if (params.mutation_type == SWAP) {
                    swapMutation(child.path);
                }
                else if (params.mutation_type == INVERSION) {
                    inversionMutation(child.path);
                }
            }

            child.cost = calculateCost(matrix, child.path);
            new_population.push_back(child);

            if (child.cost < best_individual.cost) {
                best_individual = child;

                auto now_best = std::chrono::high_resolution_clock::now();
                time_to_best_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now_best - start_time).count();

                if (params.optimal_cost > 0 && best_individual.cost <= params.optimal_cost) {
                    stop = true;
                    break;
                }

            }

            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();

            if (elapsed >= params.time_limit) {
                stop = true;
                break;
            }
        }

        population = new_population;

        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();

        if (elapsed >= params.time_limit) {
            stop = true;
        }

    }

    double avg = 0.0;
    int count = 0;
    for (int i = 0; i < static_cast<int>(population.size()); i++) {
        if (population[i].cost != std::numeric_limits<int>::max()) {
            avg += population[i].cost;
            count++;
        }
    }
    if (count > 0) {
        avg /= count;
    }
    else {
        avg = static_cast<double>(best_individual.cost);
    }

    return AlgResults(best_individual.cost, avg, time_to_best_ms, best_individual.path, false);
}
