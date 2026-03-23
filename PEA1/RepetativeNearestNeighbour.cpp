//
// Created by wiktor on 17.03.2026.
//

#include "RepetativeNearestNeighbour.h"
#include "Matrix.h"
#include "AlgResults.h"
#include <vector>
#include <limits>
#include "NearestNeighbour.h"

using namespace std;

AlgResults RepetativeNearestNeighbour::RNN(const Matrix &matrix, int start) {
    int size = matrix.getSize();

    if (size <= 1) {
        AlgResults result(0);
        result.total_cost = 0;
        result.path = {start};
        return result;
    }

    AlgResults best_result = NearestNeighbour::NN(matrix, start);

    for (int i = 0; i < size; i++) {
        if (i == start) {
            continue;
        }

        AlgResults current_result = NearestNeighbour::NN(matrix, i);

        if (current_result.total_cost < best_result.total_cost) {
            best_result = current_result;
        }
    }

    return best_result;
}
