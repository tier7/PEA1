//
// Created by wiktor on 17.03.2026.
//

#ifndef FILEREADER_H
#define FILEREADER_H
#include "Matrix.h"


class FileReader {
public:
    static void LoadFromFile(const string &filename, Matrix &matrix);
};



#endif //FILEREADER_H
