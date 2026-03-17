//
// Created by wiktor on 13.03.2026.
//

#include "NearestNeighbour.h"
#include "Matrix.h"
#include <limits>
#include "AlgResults.h"

#include <vector>
using namespace std;

AlgResults NearestNeighbour::NN(const Matrix &matrix, int start){
    int size = matrix.getSize();
    AlgResults result(0);
    if (size <= 1) {
        result.total_cost = 0;
        result.path = {start};
        return result;
    }
    vector<bool> visited(size, false);
    int unvisited_count = size-1;
    int current = start;
    int next = 0;
    int path_cost = 0;
    visited[start] = true;
    result.path.push_back(start);
    while (unvisited_count > 0) {
        int min_cost = std::numeric_limits<int>::max();
        for (int i = 0; i < size; i++) {
            if (matrix.get(current, i) < min_cost && visited[i] == false) {
                min_cost = matrix.get(current, i);
                next = i;
            }
        }
        result.total_cost += min_cost;
        current = next;
        visited[current] = true;
        result.path.push_back(current);
        unvisited_count--;
    }
    result.total_cost += matrix.get(current, 0);
    result.path.push_back(start);
    return result;
}
