//
// Created by wiktor on 13.03.2026.
//

#ifndef ALGRESULTS_H
#define ALGRESULTS_H

#include <vector>
#include <limits>

struct AlgResults {
    int total_cost;
    std::vector<int> path;
    bool interrupted;

    AlgResults(int cost) : total_cost(cost), interrupted(false) {}

    AlgResults()
        : total_cost(std::numeric_limits<int>::max()), interrupted(false) {}

    AlgResults(int cost, const std::vector<int> &p, bool wasInterrupted = false)
        : total_cost(cost), path(p), interrupted(wasInterrupted) {}
};

#endif //ALGRESULTS_H
