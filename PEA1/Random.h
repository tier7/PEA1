//
// Created by wiktor on 15.03.2026.
//

#ifndef RANDOM_H
#define RANDOM_H
#include "AlgResults.h"
#include "Matrix.h"


class Random {
public:
    static AlgResults RandomAlg(const Matrix &matrix, int start, int p);
};



#endif //RANDOM_H
