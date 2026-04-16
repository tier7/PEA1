//
// Created by ja on 16.04.2026.
//

#ifndef NODE_H
#define NODE_H

#include <vector>

struct Node {
    int currentIndex, currentCost, lowerBound, level;
    std::vector<int> currentPath;
    std::vector<bool> visited;
};




#endif //NODE_H
