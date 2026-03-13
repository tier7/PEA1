//
// Created by ja on 11.03.2026.
//

#include "Generator.h"
#include "Matrix.h"
#include <random>

using namespace std;
void Generator::generateRandom(Matrix &matrix, int minVal, int maxVal) {
    if (minVal > maxVal) {
        throw invalid_argument("minVal > maxVal");
    }
    if (minVal < 0 || maxVal < 0)
        throw invalid_argument("MinVal lub maxVal < 0");
    int n = matrix.getSize();
    if (n <= 0) {
        throw invalid_argument("Liczba miast musi byc wieksza od 0");
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(minVal, maxVal);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i==j) {
                matrix.set(i, j, -1);
            }
            else {
                matrix.set(i, j, dist(gen));
            }
        }
    }
}
