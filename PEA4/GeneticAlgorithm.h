//
// Created by ja on 22.05.2026.
//

#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H

#include <vector>
#include "Matrix.h"
#include "AlgResults.h"

enum MutationType {
    SWAP,
    INVERSION
};

struct GeneticParameters {
    int population_size = 300;
    double mutation_rate = 0.1;
    double crossover_rate = 0.8;
    long long time_limit = 900000;
    int optimal_cost = 0;
    MutationType mutation_type = SWAP;
    int tournament_size = 3;
};

struct Individual {
    std::vector<int> path;
    int cost;
};

class GeneticAlgorithm {
public:
    static std::vector<int> initialSolution(int start, const Matrix &matrix);
    static int calculateCost(const Matrix &matrix, const std::vector<int> &path);
    static AlgResults GA(const Matrix &matrix, int start, const GeneticParameters &params);

private:
    static std::vector<Individual> generateInitialPopulation(const Matrix &matrix, int start, int population_size);
    static Individual tournamentSelection(const std::vector<Individual> &population, const GeneticParameters &params);
    static std::vector<int> orderCrossover(const std::vector<int> &parent1, const std::vector<int> &parent2, int start);
    static void swapMutation(std::vector<int> &path);
    static void inversionMutation(std::vector<int> &path);
    static double randomVal();
};

#endif //GENETICALGORITHM_H
