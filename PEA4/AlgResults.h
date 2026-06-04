//
// Created by wiktor on 13.03.2026.
//

#ifndef ALGRESULTS_H
#define ALGRESULTS_H

#include <vector>
#include <limits>

struct AlgResults {
    int total_cost;
    double avg_cost;
    long long time_to_best_ms;  // czas od startu do znalezienia najlepszego rozwiazania
    std::vector<int> path;
    bool interrupted;

    AlgResults()
        : total_cost(std::numeric_limits<int>::max()),
          avg_cost(std::numeric_limits<int>::max()),
          time_to_best_ms(0),
          interrupted(false) {}

    AlgResults(int cost)
        : total_cost(cost), avg_cost(cost), time_to_best_ms(0), interrupted(false) {}

    AlgResults(int cost, double avg, long long timeToBest, const std::vector<int> &p, bool wasInterrupted = false)
        : total_cost(cost), avg_cost(avg), time_to_best_ms(timeToBest), path(p), interrupted(wasInterrupted) {}
};

#endif //ALGRESULTS_H
