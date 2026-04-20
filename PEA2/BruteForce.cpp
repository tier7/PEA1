//
// Created by wiktor on 15.03.2026.
//

#include "BruteForce.h"
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

AlgResults BruteForce::BF(const Matrix &matrix, int start) {
    AlgResults result(numeric_limits<int>::max());
    int size = matrix.getSize();

    if (size <= 0 || start < 0 || start >= size) {
        return result;
    }

    if (size == 1) {
        result.total_cost = 0;
        result.path = {start};
        return result;
    }

    vector<int> vertex_list;
    for (int i = 0; i < size; i++) {
        if (i != start) {
            vertex_list.push_back(i);
        }
    }

    sort(vertex_list.begin(), vertex_list.end());

    do {
        bool validPath = true;
        int total_cost = 0;

        int firstCost = matrix.get(start, vertex_list[0]);
        if (firstCost == -1) {
            validPath = false;
        } else {
            total_cost += firstCost;
        }

        for (int j = 0; validPath && j < size - 2; j++) {
            int edgeCost = matrix.get(vertex_list[j], vertex_list[j + 1]);
            if (edgeCost == -1) {
                validPath = false;
            } else {
                total_cost += edgeCost;
            }
        }

        if (validPath) {
            int returnCost = matrix.get(vertex_list[size - 2], start);
            if (returnCost == -1) {
                validPath = false;
            } else {
                total_cost += returnCost;
            }
        }

        if (validPath && total_cost < result.total_cost) {
            result.total_cost = total_cost;
            result.path = {start};
            for (int v : vertex_list) {
                result.path.push_back(v);
            }
            result.path.push_back(start);
        }
    } while (next_permutation(vertex_list.begin(), vertex_list.end()));

    return result;
}
