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
        int lowestCost = std::numeric_limits<int>::max();
        for (int j = 0; j < n; j++) {
            int value = matrix.get(i, j);
            if (i != j && value != -1 && value < lowestCost) {
                lowestCost = value;
                if (lowestCost == 0) {
                    break;
                }
            }
        }

        if (lowestCost != std::numeric_limits<int>::max()) {
            lowerBound += lowestCost;
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
        int lowestCost = std::numeric_limits<int>::max();

        for (int i = 0; i < n; i++) {
            int value = matrix.get(i, j);
            if (i != j && value != -1 && value < lowestCost) {
                lowestCost = value;
                if (lowestCost == 0) {
                    break;
                }
            }
        }

        if (lowestCost != std::numeric_limits<int>::max()) {
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

Node createRoot(const Matrix &matrix, int start) {
    Node root;
    int n = matrix.getSize();

    root.level = 1;
    root.currentIndex = start;
    root.currentCost = 0;
    root.lowerBound = 0;
    root.currentPath.push_back(start);
    root.visited = std::vector<bool>(n, false);
    root.visited[start] = true;
    root.reducedMatrix = matrix;
    root.lowerBound = BranchAndBound::calculateLowerBound(root.reducedMatrix);

    return root;
}

Node createNode(const Node &parent, const Matrix &matrix, int next) {
    Node child = parent;

    child.level += 1;
    child.visited[next] = true;
    child.currentPath.push_back(next);
    child.currentCost += matrix.get(parent.currentIndex, next);

    // blokowanie wiersza i kolumny
    for (int i = 0; i < child.reducedMatrix.getSize(); i++) {
        child.reducedMatrix.set(i, next, -1);
        child.reducedMatrix.set(parent.currentIndex, i, -1);
    }

    // blokowanie powrotu z next do parent
    child.reducedMatrix.set(next, parent.currentIndex, -1);
    child.currentIndex = next;
    child.lowerBound = child.currentCost + BranchAndBound::calculateLowerBound(child.reducedMatrix);

    return child;
}

void updateIfBest(const Node &currentNode, const Matrix &matrix, int start,
                  int &bestCost, std::vector<int> &bestPath) {
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
                          int &bestCost, std::vector<int> &bestPath) {
    bestCost = std::numeric_limits<int>::max();
    bestPath.clear();

    if (useInitialSolution) {
        AlgResults initial = NearestNeighbour::NN(matrix, start);
        if (!initial.path.empty() && initial.total_cost != std::numeric_limits<int>::max()) {
            bestCost = initial.total_cost;
            bestPath = initial.path;
        }
    }
}

AlgResults BranchAndBound::breadthFirstSearch(const Matrix &matrix, int start, bool useInitialSolution, long long timeLimitMs) {
    auto timeStart = std::chrono::high_resolution_clock::now();

    Node root = createRoot(matrix, start);
    std::queue<Node> nodes;
    nodes.push(root);

    int bestCost;
    std::vector<int> bestPath;
    initializeUpperBound(matrix, start, useInitialSolution, bestCost, bestPath);

    while (!nodes.empty()) {
        auto now = std::chrono::high_resolution_clock::now();
        long long elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - timeStart).count();
        if (elapsedMs >= timeLimitMs) {
            return AlgResults(bestCost, bestPath, true);
        }

        Node currentNode = nodes.front();
        nodes.pop();

        if (currentNode.lowerBound >= bestCost) {
            continue;
        }

        if (currentNode.level == matrix.getSize()) {
            updateIfBest(currentNode, matrix, start, bestCost, bestPath);
        }
        else {
            for (int i = 0; i < currentNode.reducedMatrix.getSize(); i++) {
                if (canVisit(currentNode, i)) {
                    int partialCost = currentNode.currentCost + matrix.get(currentNode.currentIndex, i);
                    if (partialCost >= bestCost) {
                        continue;
                    }

                    Node child = createNode(currentNode, matrix, i);
                    if (child.lowerBound < bestCost) {
                        nodes.push(child);
                    }
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
    auto timeStart = std::chrono::high_resolution_clock::now();

    Node root = createRoot(matrix, start);
    std::priority_queue<Node, std::vector<Node>, CompareNode> nodes;
    nodes.push(root);

    int bestCost;
    std::vector<int> bestPath;
    initializeUpperBound(matrix, start, useInitialSolution, bestCost, bestPath);

    while (!nodes.empty()) {
        auto now = std::chrono::high_resolution_clock::now();
        long long elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - timeStart).count();
        if (elapsedMs >= timeLimitMs) {
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
                if (canVisit(currentNode, i)) {
                    int partialCost = currentNode.currentCost + matrix.get(currentNode.currentIndex, i);
                    if (partialCost >= bestCost) {
                        continue;
                    }

                    Node child = createNode(currentNode, matrix, i);
                    if (child.lowerBound < bestCost) {
                        nodes.push(child);
                    }
                }
            }
        }
    }

    return AlgResults(bestCost, bestPath, false);
}
