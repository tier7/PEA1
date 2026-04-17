//
// Created by ja on 16.04.2026.
//
#include <limits>
#include "BranchAndBound.h"
#include "Node.h"

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
        if (lowestCost != std::numeric_limits<int>::max()) {
            lowerBound += lowestCost;
            for (int j = 0; j < n; j++) {
                if (i != j && matrix.get(i,j) != -1) {
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
        if (lowestCost != std::numeric_limits<int>::max()) {
            lowerBound += lowestCost;
            for (int i = 0; i < n; i++) {
                if (i != j && matrix.get(i,j) != -1) {
                    matrix.set(i, j, matrix.get(i, j) - lowestCost);
                }

            }
        }

    }
    return lowerBound;
};

Node createNode(const Node &parent, const Matrix &matrix, int next) {
    Node child = parent;
    child.level += 1;
    child.visited[next] = true;
    child.currentPath.push_back(next);
    child.currentCost += matrix.get(parent.currentIndex, next);
    for (int i = 0; i<child.reducedMatrix.getSize(); i++) {
        child.reducedMatrix.set(i, next, -1);
        child.reducedMatrix.set(parent.currentIndex, i, -1);
    }
    child.reducedMatrix.set(next, parent.currentIndex, -1);
    child.currentIndex = next;
    child.lowerBound = child.currentCost + BranchAndBound::calculateLowerBound(child.reducedMatrix);
    return child;
}
