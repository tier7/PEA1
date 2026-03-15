//
// Created by wiktor on 13.03.2026.
//

#ifndef NEARESTNEIGHBOUR_H
#define NEARESTNEIGHBOUR_H
#include <vector>

#include "Matrix.h"
#include "AlgResults.h"
using namespace std;


class NearestNeighbour {
public:
    static AlgResults NN(const Matrix &matrix, int start);
};



#endif //NEARESTNEIGHBOUR_H
