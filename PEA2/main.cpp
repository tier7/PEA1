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
    BranchAndBound bb = BranchAndBound();
    matrix.print();
    cout<<bb.calculateLowerBound(matrix);

    return 0;
}
