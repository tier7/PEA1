//
// Created by wiktor on 17.03.2026.
//

#include "RepetativeNearestNeighbour.h"
#include "Matrix.h"
#include "AlgResults.h"
#include <vector>
#include <limits>

using namespace std;

AlgResults RepetativeNearestNeighbour::RNN(const Matrix &matrix, int start) {
    int size = matrix.getSize();
    AlgResults result(numeric_limits<int>::max());
    if (size <= 1) {
        result.total_cost = 0;
        result.path = {start};
        return result;
    }

    vector<bool> visited(size, false);
    vector<int> current_path;

    visited[start] = true;
    current_path.push_back(start);
    RNNRec(matrix, start, start, visited, current_path, 0, 1, result);
    return result;
}

void RepetativeNearestNeighbour::RNNRec(const Matrix &matrix, int start, int current, vector<bool> &visited, vector<int> &current_path,
int current_cost, int visited_count, AlgResults &result) {


    int size = matrix.getSize();

    if (visited_count == size) {
        int total_cost = current_cost + matrix.get(current, start);
        if (total_cost < result.total_cost) {
            result.total_cost = total_cost;
            result.path = current_path;
            result.path.push_back(start);
        }
        return;
    }

    int min_cost = numeric_limits<int>::max();

    for (int i = 0; i < size; i++) {
        if (visited[i] == false && matrix.get(current, i) < min_cost) {
            min_cost = matrix.get(current, i);
        }
    }

    vector<int> next_vertices;

    for (int i = 0; i < size; i++) {
        if (visited[i] == false && matrix.get(current, i) == min_cost) {
            next_vertices.push_back(i);
        }
    }

    int next_count = next_vertices.size();
    for (int i = 0; i < next_count; i++) {

        int next = next_vertices[i];

        visited[next] = true;
        current_path.push_back(next);
        RNNRec(matrix, start, next, visited, current_path,
                 current_cost + matrix.get(current, next),
                 visited_count + 1, result);

        current_path.pop_back();
        visited[next] = false;
    }
}