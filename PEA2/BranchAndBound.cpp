//
// Created by ja on 16.04.2026.
//
#include <limits>
#include "BranchAndBound.h"
#include "Node.h"
#include "AlgResults.h"
#include <stack>

int BranchAndBound::calculateLowerBound(Matrix &matrix) {
    int n = matrix.getSize();
    int lowerBound = 0;
    // znajdywanie najmniejszego kosztu w wierszu
    for (int i = 0; i < n; i++) {
        int lowestCost = std::numeric_limits<int>::max();
        for (int j = 0; j < n; j++) {
            if (i != j && matrix.get(i, j) != -1 && matrix.get(i, j) < lowestCost) {
                lowestCost = matrix.get(i, j);
            }
        }
        // odejmowanie znalezionej najmniejszej wartosci od kazdego elementu wiersza
        if (lowestCost != std::numeric_limits<int>::max()) {
            lowerBound += lowestCost;
            for (int j = 0; j < n; j++) {
                if (i != j && matrix.get(i, j) != -1) {
                    matrix.set(i, j, matrix.get(i, j) - lowestCost);
                }
            }
        }
    }
    // znalezienie najmniejszego kosztu w kolumnie
    for (int j = 0; j < n; j++) {
        int lowestCost = std::numeric_limits<int>::max();
        for (int i = 0; i < n; i++) {
            if (i != j && matrix.get(i, j) != -1 && matrix.get(i, j) < lowestCost) {
                lowestCost = matrix.get(i, j);
            }
        }
        // odejmowanie najmniejszego kosztu od elementow kolumny
        if (lowestCost != std::numeric_limits<int>::max()) {
            lowerBound += lowestCost;
            for (int i = 0; i < n; i++) {
                if (i != j && matrix.get(i, j) != -1) {
                    matrix.set(i, j, matrix.get(i, j) - lowestCost);
                }
            }
        }
    }
    return lowerBound;
};

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
    // blokowanie wiersza i i kolumny j
    for (int i = 0; i < child.reducedMatrix.getSize(); i++) {
        child.reducedMatrix.set(i, next, -1);
        child.reducedMatrix.set(parent.currentIndex, i, -1);
    }
    // blokowanie powrotu z j do i
    child.reducedMatrix.set(next, parent.currentIndex, -1);
    child.currentIndex = next;
    child.lowerBound = child.currentCost + BranchAndBound::calculateLowerBound(child.reducedMatrix);
    return child;
}

AlgResults BranchAndBound::dfs(const Matrix &matrix, int start) {
    Node root = createRoot(matrix, start);
    std::stack<Node> nodes;
    nodes.push(root);
    int bestCost = std::numeric_limits<int>::max();
    std::vector<int> bestPath;
    while (!nodes.empty()) {
        Node currentNode = nodes.top();
        nodes.pop();
        if (currentNode.lowerBound >= bestCost) {
            continue;
        }
        if (currentNode.level == matrix.getSize()){
            if (currentNode.reducedMatrix.get(currentNode.currentIndex, start) != -1) {
                currentNode.currentCost += matrix.get(currentNode.currentIndex, start);
                if (currentNode.currentCost < bestCost) {
                    bestCost = currentNode.currentCost;
                    bestPath = currentNode.currentPath;
                }
            }

        }
        else {
            for (int i = 0; i < currentNode.reducedMatrix.getSize(); i++) {
                if (!currentNode.visited[i] && i != currentNode.currentIndex && currentNode.reducedMatrix.get(currentNode.currentIndex, i) != -1) {
                    Node child = createNode(currentNode, matrix, i);
                    if (child.lowerBound < bestCost) {
                        nodes.push(child);
                    }
                }
            }
        }
    }
    bestPath.push_back(start);
    AlgResults result = AlgResults(bestCost, bestPath);
    return result;
}
