//
// Created by ja on 16.04.2026.
//

#ifndef BRANCHANDBOUND_H
#define BRANCHANDBOUND_H

#include "Matrix.h"
#include "AlgResults.h"

class BranchAndBound {
public:
    static int calculateLowerBound(Matrix &matrix);

    static AlgResults breadthFirstSearch(const Matrix &matrix, int start, bool useInitialSolution = false, long long timeLimitMs = 300000LL);

    static AlgResults bestFirstSearch(const Matrix &matrix, int start, bool useInitialSolution = false, long long timeLimitMs = 300000LL);
};



#endif //BRANCHANDBOUND_H
