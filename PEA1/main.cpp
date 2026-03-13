#include <iostream>
#include"Matrix.h"
#include"Generator.h"
#include "NearestNeighbour.h"
#include "AlgResults.h"

int main() {
    Matrix matrix(6);
    matrix.set(1,1,5);
    Generator generator;
    generator.generateRandom(matrix, 1, 100);
    matrix.print();
    vector<int> cost = {1,2,3,1};
    int sum = matrix.calculateTotalCost(cost);
    cout<<sum<<endl;
    AlgResults NN = NearestNeighbour::NN(matrix);
    cout << "Koszt: " << NN.total_cost << endl;
    cout << "Sciezka: ";
    for (int v : NN.path) {
        cout << v << " ";
    }
    return 0;
}
