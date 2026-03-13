//
// Created by ja on 11.03.2026.
//

#include "Generator.h"
#include "Matrix.h"
#include <random>

using namespace std;
void Generator::generateRandom(int minVal, int maxVal, int n) {
    if (minVal > maxVal) {
        throw invalid_argument("minVal > maxVal");
    }
    if (minVal < 0 || maxVal < 0)
        throw invalid_argument("MinVal lub maxVal < 0");
    if (n <= 0) {
        throw invalid_argument("Liczba miast musi byc wieksza od 0");
    }

    Matrix matrix(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(minVal, maxVal);
}
