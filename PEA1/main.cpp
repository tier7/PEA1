#include <iostream>
#include"Matrix.h"
#include"Generator.h"
#include "NearestNeighbour.h"
#include "AlgResults.h"
#include "Random.h"
#include "BruteForce.h"

int main() {
    Matrix matrix(6);
    matrix.set(1,1,5);
    Generator generator;
    generator.generateRandom(matrix, 1, 100);
    matrix.print();
    vector<int> cost = {1,2,3,1};
    int sum = matrix.calculateTotalCost(cost);
    cout<<sum<<endl;
    AlgResults NN = NearestNeighbour::NN(matrix, 0);
    cout << "Koszt NN: " << NN.total_cost << endl;
    cout << "Sciezka NN: ";
    for (int v : NN.path) {
        cout << v << " ";
    }
    AlgResults RNG = Random::RandomAlg(matrix, 0, 10);
    cout << "\nKoszt Random: " << RNG.total_cost << endl;
    cout << "Sciezka Random: ";
    for (int v : RNG.path) {
        cout << v << " ";
    }
    AlgResults BF = BruteForce::BF(matrix, 0);
    cout << "\nKoszt BF: " << BF.total_cost << endl;
    cout << "Sciezka BF: ";
    for (int v : BF.path) {
        cout << v << " ";
    }
    return 0;

}
