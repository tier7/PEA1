#include <iostream>
#include "FileReader.h"
#include "Matrix.h"
#include "Generator.h"
#include "AlgResults.h"
#include "Matrix.h"
#include "Generator.h"
#include "Node.h"
#include "BranchAndBound.h"
#include "AlgResults.h"

int main() {
    Matrix matrix = Matrix();
    Generator generator = Generator();
    matrix.resize(3);
    generator.generateRandom(matrix, 1, 100);
    matrix.print();
    AlgResults results = BranchAndBound::dfs(matrix, 0);
    cout<<results.total_cost<<endl;
    for (int i = 0; i < matrix.getSize(); i++) {
        cout<<results.path[i];
    }
    return 0;
}
