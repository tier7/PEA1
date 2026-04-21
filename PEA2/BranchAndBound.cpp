//
// Created by ja on 16.04.2026.
//

#include <limits>
#include <chrono>
#include <queue>
#include "BranchAndBound.h"
#include "Node.h"
#include "NearestNeighbour.h"

using namespace std;

int BranchAndBound::calculateLowerBound(Matrix &matrix) {
    int n = matrix.getSize();
    int lowerBound = 0;

    // redukcja wierszy
    for (int i = 0; i < n; i++) {
        int lowestCost = numeric_limits<int>::max();
        for (int j = 0; j < n; j++) {
            int value = matrix.get(i, j);
            if (i != j && value != -1 && value < lowestCost) {
                lowestCost = value;
                if (lowestCost == 0) {
                    break;
                }
            }
        }

        if (lowestCost != numeric_limits<int>::max()) {
            // dodanie znalezionej wartosci najmniejszej do lower bound
            lowerBound += lowestCost;
            // odjecie minimalnej wartosci od wszystkich elementow wiersza
            for (int j = 0; j < n; j++) {
                int value = matrix.get(i, j);
                if (i != j && value != -1) {
                    matrix.set(i, j, value - lowestCost);
                }
            }
        }
    }
    // redukcja kolumn
    for (int j = 0; j < n; j++) {
        int lowestCost = numeric_limits<int>::max();
        // szukanie minimum w kolumnie
        for (int i = 0; i < n; i++) {
            int value = matrix.get(i, j);
            if (i != j && value != -1 && value < lowestCost) {
                lowestCost = value;
                if (lowestCost == 0) {
                    break;
                }
            }
        }
        // dodanie do lower bound i odjecie od wszystkich elementow kolumny
        if (lowestCost != numeric_limits<int>::max()) {
            lowerBound += lowestCost;
            for (int i = 0; i < n; i++) {
                int value = matrix.get(i, j);
                if (i != j && value != -1) {
                    matrix.set(i, j, value - lowestCost);
                }
            }
        }
    }

    return lowerBound;
}

// tworzenie korzenia drzewa
Node createRoot(const Matrix &matrix, int start) {
    Node root;
    int n = matrix.getSize();

    root.level = 1;
    root.currentIndex = start;
    root.currentCost = 0;
    root.lowerBound = 0;
    root.currentPath.push_back(start);
    root.visited = vector<bool>(n, false);
    root.visited[start] = true;
    root.reducedMatrix = matrix;
    root.lowerBound = BranchAndBound::calculateLowerBound(root.reducedMatrix);

    return root;
}

// tworzenie nowego wezla po przejsciu z miasta do nastepnego
Node createNode(const Node &parent, const Matrix &matrix, int next, int start) {
    Node child = parent;
    int reducedEdgeCost = parent.reducedMatrix.get(parent.currentIndex, next);

    child.level += 1;
    child.visited[next] = true;
    child.currentPath.push_back(next);
    child.currentCost += matrix.get(parent.currentIndex, next);
    // blokowanie kolumny i wiersza
    for (int i = 0; i < child.reducedMatrix.getSize(); i++) {
        child.reducedMatrix.set(i, next, -1);
        child.reducedMatrix.set(parent.currentIndex, i, -1);
    }
    if (child.level < child.reducedMatrix.getSize()) {
        child.reducedMatrix.set(next, start, -1);
    }

    child.currentIndex = next;
    int reductionCost = BranchAndBound::calculateLowerBound(child.reducedMatrix);
    child.lowerBound = parent.lowerBound + reducedEdgeCost + reductionCost;

    return child;
}

void updateIfBest(const Node &currentNode, const Matrix &matrix, int start,
                  int &bestCost, vector<int> &bestPath) {
    int returnCost = matrix.get(currentNode.currentIndex, start);
    if (returnCost == -1) {
        return;
    }

    int fullCost = currentNode.currentCost + returnCost;
    if (fullCost < bestCost) {
        bestCost = fullCost;
        bestPath = currentNode.currentPath;
        bestPath.push_back(start);
    }
}

bool canVisit(const Node &currentNode, int next) {
    return !currentNode.visited[next]
           && next != currentNode.currentIndex
           && currentNode.reducedMatrix.get(currentNode.currentIndex, next) != -1;
}

void initializeUpperBound(const Matrix &matrix, int start, bool useInitialSolution,
                          int &bestCost, vector<int> &bestPath) {
    bestCost = numeric_limits<int>::max();
    bestPath.clear();

    if (useInitialSolution) {
        AlgResults initial = NearestNeighbour::NN(matrix, start);
        if (!initial.path.empty() && initial.total_cost != numeric_limits<int>::max()) {
            bestCost = initial.total_cost;
            bestPath = initial.path;
        }
    }
}

bool timeLimitReached(const chrono::high_resolution_clock::time_point &timeStart, long long timeLimitMs) {
    auto now = chrono::high_resolution_clock::now();
    long long elapsedMs = chrono::duration_cast<chrono::milliseconds>(now - timeStart).count();
    return elapsedMs >= timeLimitMs;
}

AlgResults BranchAndBound::breadthFirstSearch(const Matrix &matrix, int start, bool useInitialSolution, long long timeLimitMs) {
    auto timeStart = chrono::high_resolution_clock::now();

    Node root = createRoot(matrix, start);
    queue<Node> nodes;
    nodes.push(root);

    int bestCost;
    vector<int> bestPath;
    initializeUpperBound(matrix, start, useInitialSolution, bestCost, bestPath);

    while (!nodes.empty()) {
        if (timeLimitReached(timeStart, timeLimitMs)) {
            return AlgResults(bestCost, bestPath, true);
        }
        // pobranie wezla z kolejki
        Node currentNode = nodes.front();
        nodes.pop();
        // odcinanie galezi ktora nie poprawi najlepszego rozwiazania
        if (currentNode.lowerBound >= bestCost) {
            continue;
        }
        // zamkniecie cyklu gdy odwiedzono wszystkie miasta
        if (currentNode.level == matrix.getSize()) {
            updateIfBest(currentNode, matrix, start, bestCost, bestPath);
        }
        else {
            // generowanie dzieci dla wszystkich kolejnych dopuszczalnych miast
            for (int i = 0; i < currentNode.reducedMatrix.getSize(); i++) {
                if (timeLimitReached(timeStart, timeLimitMs)) {
                    return AlgResults(bestCost, bestPath, true);
                }

                if (!canVisit(currentNode, i)) {
                    continue;
                }

                int partialCost = currentNode.currentCost + matrix.get(currentNode.currentIndex, i);
                if (partialCost >= bestCost) {
                    continue;
                }

                Node child = createNode(currentNode, matrix, i, start);

                if (timeLimitReached(timeStart, timeLimitMs)) {
                    return AlgResults(bestCost, bestPath, true);
                }

                if (child.lowerBound < bestCost) {
                    nodes.push(child);
                }
            }
        }
    }

    return AlgResults(bestCost, bestPath, false);
}

struct CompareNode {
    bool operator()(const Node &a, const Node &b) const {
        return a.lowerBound > b.lowerBound;
    }
};

AlgResults BranchAndBound::bestFirstSearch(const Matrix &matrix, int start, bool useInitialSolution, long long timeLimitMs) {
    auto timeStart = chrono::high_resolution_clock::now();

    Node root = createRoot(matrix, start);
    priority_queue<Node, vector<Node>, CompareNode> nodes;
    nodes.push(root);

    int bestCost;
    vector<int> bestPath;
    initializeUpperBound(matrix, start, useInitialSolution, bestCost, bestPath);

    while (!nodes.empty()) {
        if (timeLimitReached(timeStart, timeLimitMs)) {
            return AlgResults(bestCost, bestPath, true);
        }

        Node currentNode = nodes.top();
        nodes.pop();

        if (currentNode.lowerBound >= bestCost) {
            continue;
        }

        if (currentNode.level == matrix.getSize()) {
            updateIfBest(currentNode, matrix, start, bestCost, bestPath);
        }
        else {
            for (int i = 0; i < currentNode.reducedMatrix.getSize(); i++) {
                if (timeLimitReached(timeStart, timeLimitMs)) {
                    return AlgResults(bestCost, bestPath, true);
                }

                if (!canVisit(currentNode, i)) {
                    continue;
                }

                int partialCost = currentNode.currentCost + matrix.get(currentNode.currentIndex, i);
                if (partialCost >= bestCost) {
                    continue;
                }

                Node child = createNode(currentNode, matrix, i, start);

                if (timeLimitReached(timeStart, timeLimitMs)) {
                    return AlgResults(bestCost, bestPath, true);
                }

                if (child.lowerBound < bestCost) {
                    nodes.push(child);
                }
            }
        }
    }

    return AlgResults(bestCost, bestPath, false);
}
