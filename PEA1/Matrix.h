//
// Created by ja on 11.03.2026.
//

#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <iostream>

using namespace std;

class Matrix {
private:
    int size;
    vector<vector<int>> data;

public:
    Matrix();
    Matrix(int n);

    void resize(int n);

    int getSize() const;

    void clear();

    int get(int row, int col) const;
    void set(int row, int col, int val);
    void print() const;

    int calculateTotalCost(vector<int> &path) const;
};



#endif //MATRIX_H
