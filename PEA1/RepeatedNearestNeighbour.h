//
// Created by wiktor on 17.03.2026.
//

#ifndef REPETATIVENEARESTNEIGHBOUR_H
#define REPETATIVENEARESTNEIGHBOUR_H
#include "AlgResults.h"
#include "Matrix.h"


class RepeatedNearestNeighbour {
public:
    static AlgResults RNN(const Matrix &matrix, int start);
private:
    static void RNNRec(const Matrix &matrix, int start, int current, vector<bool> &visited, vector<int> &current_path, int current_cost, int visited_count, AlgResults &result);
};



#endif //REPETATIVENEARESTNEIGHBOUR_H
