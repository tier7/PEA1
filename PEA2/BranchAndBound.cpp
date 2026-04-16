//
// Created by ja on 16.04.2026.
//
#include <limits>
#include "BranchAndBound.h"

int BranchAndBound::calculateLowerBound(Matrix &matrix) {
    int n = matrix.getSize();
    int lowerBound = 0;
    // znajdywanie najmniejszego kosztu w wierszu
    for (int i = 0; i < n; i++) {
        int lowestCost = std::numeric_limits<int>::max();
        for (int j = 0; j < n; j++) {
            if (i != j && matrix.get(i,j) != -1 && matrix.get(i, j) < lowestCost) {
                lowestCost = matrix.get(i, j);
            }
        }
        // odejmowanie znalezionej najmniejszej wartosci od kazdego elementu wiersza
        if (lowestCost != numeric_limits<int>::max()) {
            lowerBound += lowestCost;
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    matrix.set(i, j, matrix.get(i, j) - lowestCost);
                }
            }
        }

    }
    // znalezienie najmniejszego kosztu w kolumnie
    for (int j = 0; j < n; j++) {
        int lowestCost = std::numeric_limits<int>::max();
        for (int i = 0; i < n; i++) {
            if (i != j && matrix.get(i,j) != -1 && matrix.get(i, j) < lowestCost) {
                lowestCost = matrix.get(i, j);
            }
        }
        // odejmowanie najmniejszego kosztu od elementow kolumny
        if (lowestCost != numeric_limits<int>::max()) {
            lowerBound += lowestCost;
            for (int i = 0; i < n; i++) {
                if (i != j) {
                    matrix.set(i, j, matrix.get(i, j) - lowestCost);
                }

            }
        }

    }
    return lowerBound;
};