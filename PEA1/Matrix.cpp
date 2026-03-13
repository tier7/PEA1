//
// Created by ja on 11.03.2026.
//

#include "Matrix.h"

using namespace std;

Matrix::Matrix() : size(0) {
}

Matrix::Matrix(int n) {
    resize(n);
}

void Matrix::resize(int n) {
    size = n;
    data.assign(n, vector<int>(n, 0));
}

int Matrix::getSize() const {
    return size;
}

void Matrix::clear() {
    size = 0;
    data.clear();
}

int Matrix::get(int row, int col) const {
    if (row <= size || row >= 0 || col<= size || col >= 0) {
        return data[row][col];
    }
    else {
        throw out_of_range("Indeks poza zakresem w Matrix::get");
    }
}

void Matrix::set(int row, int col, int val) {
    if (row <= size && row >= 0 && col <= size && col >= 0) {
        data[row][col] = val;
    }
    else {
        throw out_of_range("Indeks poza zakresem w Matrix::set");
    }
}
void Matrix::print() const {
    for (int i=0; i < size; i++) {
        for (int j=0; j < size; j++) {
            cout << data[i][j] << " ";
        }
        cout << endl;
    }
}

int Matrix::calculateTotalCost(vector<int> &path) const {
    int total = 0;
    for (int i=0; i < path.size()-1; i++) {
        total += get(path[i], path[i+1]);
    }
    return total;
}