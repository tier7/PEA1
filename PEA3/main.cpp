#include <iostream>
#include "Matrix.h"
#include "Generator.h"
#include "AlgResults.h"
#include "BruteForce.h"
#include "FileReader.h"
#include <string>
#include <chrono>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>
#include "SimulatedAnnealing.h"

using namespace std;

int main() {
    Matrix matrix;
    matrix.resize(10);
    Generator gen;
    gen.generateRandom(matrix, 1, 100);
    std::vector<int> path = SimulatedAnnealing::initialSolution(0, 0, matrix);
    for (int i = 0; i < path.size(); i++) {
        cout << path[i] << " ";
    }
    cout << endl;
    std::vector<int> path2 = SimulatedAnnealing::initialSolution(1, 0, matrix);
    for (int i = 0; i < path2.size(); i++) {
        cout << path2[i] << " ";
    }

    return 0;

}
