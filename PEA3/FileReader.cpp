//
// Created by wiktor on 17.03.2026.
//

#include "FileReader.h"

#include <iostream>
#include <fstream>
#include <string>
#include "Matrix.h"

using namespace std;

void FileReader::LoadFromFile(const string &filename, Matrix &matrix) {
    string text;
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Nie mozna otworzyc pliku " + filename);
    }
    int size;
    file >> size;
    if (!file || size <= 0) {
        throw runtime_error("Niepoprawny rozmiar macierzy");
    }

    matrix.resize(size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int val;
            file >> val;

            if (!file) {
                throw runtime_error("Niepoprawny rozmiar macierzy");
            }
            matrix.set(i, j, val);
        }
    }
    file.close();
}