//
// Created by ja on 16.05.2026.
//

#ifndef SIMULATEDANNEALING_H
#define SIMULATEDANNEALING_H

#include <vector>
#include "Matrix.h"

enum CoolingScheme {
    LINEAR,
    EXPONENTIAL
};

enum InitialSolutionType {
    RANDOM,
    NEARESTNEIGHBOUR
};

struct Parameters {
    double start_temperature = 1000.0;
    double end_temperature = 0.001;
    double cooling = 0.995;
    int epochs = 1000;
    long long time_limit = 900000;
    InitialSolutionType initial_solution_type = NEARESTNEIGHBOUR;
    CoolingScheme cooling_scheme = LINEAR;
};


class SimulatedAnnealing {
public:
    static std::vector<int> initialSolution(InitialSolutionType type, int start, const Matrix &matrix);
    int calculateCost(const Matrix &matrix, const std::vector<int> &path);
    static std::vector<int> SA(const Matrix &matrix, int start, InitialSolutionType initType);
};



#endif //SIMULATEDANNEALING_H
