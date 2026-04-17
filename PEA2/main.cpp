#include <iostream>
#include "FileReader.h"
#include "Matrix.h"
#include "Generator.h"
#include "AlgResults.h"
#include "Matrix.h"
#include "Generator.h"
#include "Node.h"
#include "BranchAndBound.h"

int main() {
    Matrix matrix = Matrix();
    Generator generator = Generator();
    matrix.resize(3);
    generator.generateRandom(matrix, 1, 100);
    matrix.print();
    cout<<BranchAndBound::calculateLowerBound(matrix);

    return 0;
}
