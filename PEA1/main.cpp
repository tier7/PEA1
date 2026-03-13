#include <iostream>
#include"Matrix.h"
#include"Generator.h"

int main() {
    Matrix matrix(6);
    matrix.set(1,1,5);
    Generator generator;
    generator.generateRandom(matrix, 1, 100);
    matrix.print();

    return 0;
}
