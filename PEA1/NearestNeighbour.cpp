//
// Created by wiktor on 13.03.2026.
//

#include "NearestNeighbour.h"
#include "Matrix.h"
#include <limits>
#include "AlgResults.h"

#include <vector>
using namespace std;

AlgResults NearestNeighbour::NN(const Matrix &matrix){
    int size = matrix.getSize();
    AlgResults result(0);
    if (size <= 1) {
        return result;
    }
    vector<bool> visited(size, false);
    int unvisited_count = size-1;
    int current = 0;
    int next = 0;
    int path_cost = 0;
    visited[0] = true;
    while (unvisited_count > 0) {
        int min_cost = std::numeric_limits<int>::max();
        for (int i = 0; i < size; i++) {
            if (matrix.get(current, i) < min_cost && visited[i] == false) {
                min_cost = matrix.get(current, i);
                next = i;
            }
        }
        result.total_cost += min_cost;
        result.path.push_back(next);
        current = next;
        visited[current] = true;
        unvisited_count--;
    }
    path_cost += matrix.get(current, 0);
    return result;
}
