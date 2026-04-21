#include <iostream>
#include "Matrix.h"
#include "Generator.h"
#include "AlgResults.h"
#include "BruteForce.h"
#include "FileReader.h"
#include "BranchAndBound.h"
#include <string>
#include <chrono>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>

using namespace std;

const int TIME_LIMIT_MS = 300000;

void printResult(const AlgResults& result, long long time) {
    if (result.total_cost == numeric_limits<int>::max()) {
        cout << "Koszt: brak rozwiazania" << endl;
        cout << "Sciezka: brak" << endl;
    } else {
        cout << "Koszt: " << result.total_cost << endl;
        cout << "Sciezka: ";
        for (int i = 0; i < result.path.size(); i++) {
            cout << result.path[i] << " ";
        }
        cout << endl;
    }

    if (result.interrupted) {
        cout << "Przerwanie - limit czasu" << endl;
    }

    cout << "Czas: " << time << " us" << endl;
}

void testAlgorithms() {
    cout << "Test BreadthFirstSearch i BestFirstSearch" << endl;

    Generator generator;
    vector<int> sizes = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    int tests = 100;
    int maxTimeoutsInRow = 10;

    ofstream details("wyniki.csv");
    ofstream summary("podsumowanie.csv");

    if (!details.is_open() || !summary.is_open()) {
        cout << "Blad otwarcia plikow CSV" << endl;
        return;
    }

    details << "N;TestNr;Alg;UseInitial;Cost;TimeUs;Interrupted\n";
    summary << "Alg;UseInitial;N;TestsDone;SolvedCount;InterruptedCount;InterruptedPercent;AvgTimeUsSolved;EarlyStop\n";

    cout << fixed << setprecision(2);
    summary << fixed << setprecision(2);

    vector<pair<string, bool>> variants = {
            {"BFS", false},
            {"BFS", true},
            {"BestFS", false},
            {"BestFS", true}
    };

    for (int n : sizes) {
        vector<int> testsDone(variants.size(), 0);
        vector<int> solvedCount(variants.size(), 0);
        vector<int> interruptedCount(variants.size(), 0);
        vector<int> timeoutsInRow(variants.size(), 0);
        vector<int> earlyStop(variants.size(), 0);
        vector<long long> sumTimeSolved(variants.size(), 0);

        for (int t = 0; t < tests; t++) {
            Matrix matrix;
            matrix.resize(n);
            generator.generateRandom(matrix, 10, 99);

            for (int v = 0; v < variants.size(); v++) {
                if (earlyStop[v] == 1) {
                    continue;
                }

                string alg = variants[v].first;
                bool useInitial = variants[v].second;

                if (alg == "BFS" && !useInitial && n > 12) {
                    continue;
                }

                auto start = chrono::high_resolution_clock::now();

                AlgResults result;
                if (alg == "BFS") {
                    result = BranchAndBound::breadthFirstSearch(matrix, 0, useInitial, TIME_LIMIT_MS);
                } else {
                    result = BranchAndBound::bestFirstSearch(matrix, 0, useInitial, TIME_LIMIT_MS);
                }

                auto end = chrono::high_resolution_clock::now();
                long long time = chrono::duration_cast<chrono::microseconds>(end - start).count();

                testsDone[v]++;

                if (result.interrupted) {
                    interruptedCount[v]++;
                    timeoutsInRow[v]++;
                } else {
                    solvedCount[v]++;
                    sumTimeSolved[v] += time;
                    timeoutsInRow[v] = 0;
                }

                details << n << ";"
                        << (t + 1) << ";"
                        << alg << ";"
                        << (useInitial ? 1 : 0) << ";";

                if (result.total_cost == numeric_limits<int>::max()) {
                    details << -1;
                } else {
                    details << result.total_cost;
                }

                details << ";"
                        << time << ";"
                        << (result.interrupted ? 1 : 0) << "\n";

                if (timeoutsInRow[v] >= maxTimeoutsInRow) {
                    earlyStop[v] = 1;
                }
            }

            bool stopAll = true;
            for (int v = 0; v < variants.size(); v++) {
                string alg = variants[v].first;
                bool useInitial = variants[v].second;

                if (alg == "BFS" && !useInitial && n > 12) {
                    continue;
                }

                if (earlyStop[v] == 0) {
                    stopAll = false;
                    break;
                }
            }

            if (stopAll) {
                break;
            }
        }

        cout << "\nN = " << n << endl;

        for (int v = 0; v < variants.size(); v++) {
            string alg = variants[v].first;
            bool useInitial = variants[v].second;

            if (alg == "BFS" && !useInitial && n > 12) {
                cout << "BFS bez NN: pominięto dla tego N" << endl;
                continue;
            }

            double interruptedPercent = 0.0;
            if (testsDone[v] > 0) {
                interruptedPercent = 100.0 * interruptedCount[v] / testsDone[v];
            }

            double avgTimeSolved = 0.0;
            if (solvedCount[v] > 0) {
                avgTimeSolved = (double)sumTimeSolved[v] / solvedCount[v];
            }

            summary << alg << ";"
                    << (useInitial ? 1 : 0) << ";"
                    << n << ";"
                    << testsDone[v] << ";"
                    << solvedCount[v] << ";"
                    << interruptedCount[v] << ";"
                    << interruptedPercent << ";"
                    << avgTimeSolved << ";"
                    << earlyStop[v] << "\n";

            if (alg == "BFS" && !useInitial) cout << "BFS bez NN: ";
            if (alg == "BFS" && useInitial) cout << "BFS + NN: ";
            if (alg == "BestFS" && !useInitial) cout << "BestFS bez NN: ";
            if (alg == "BestFS" && useInitial) cout << "BestFS + NN: ";

            cout << "testsDone=" << testsDone[v]
                 << " solved=" << solvedCount[v]
                 << " interrupted=" << interruptedCount[v]
                 << " interrupted%=" << interruptedPercent
                 << " avgTimeSolved(us)=" << avgTimeSolved;

            if (earlyStop[v] == 1) {
                cout << " earlyStop=1";
            }

            cout << endl;
        }
    }

    details.close();
    summary.close();

    cout << "\nZapisano pliki wyniki.csv i podsumowanie.csv" << endl;
}

void testBestFSLarge() {
    cout << "Test duzych wartosci dla BestFirstSearch" << endl;

    Generator generator;

    int startN, endN, step, tests;
    int maxTimeoutsInRow = 10;

    cout << "N start: ";
    cin >> startN;
    cout << "N koniec: ";
    cin >> endN;
    cout << "Krok: ";
    cin >> step;
    cout << "Liczba powtorzen: ";
    cin >> tests;

    ofstream details("bestfs_duze_wyniki.csv");
    ofstream summary("bestfs_duze_podsumowanie.csv");

    if (!details.is_open() || !summary.is_open()) {
        cout << "Blad otwarcia plikow CSV" << endl;
        return;
    }

    details << "N;TestNr;Alg;UseInitial;Cost;TimeUs;Interrupted\n";
    summary << "Alg;UseInitial;N;TestsDone;SolvedCount;InterruptedCount;InterruptedPercent;AvgTimeUsSolved;EarlyStop\n";

    cout << fixed << setprecision(2);
    summary << fixed << setprecision(2);

    vector<pair<string, bool>> variants = {
            {"BestFS", false},
            {"BestFS", true}
    };

    for (int n = startN; n <= endN; n += step) {
        vector<int> testsDone(variants.size(), 0);
        vector<int> solvedCount(variants.size(), 0);
        vector<int> interruptedCount(variants.size(), 0);
        vector<int> timeoutsInRow(variants.size(), 0);
        vector<int> earlyStop(variants.size(), 0);
        vector<long long> sumTimeSolved(variants.size(), 0);

        for (int t = 0; t < tests; t++) {
            Matrix matrix;
            matrix.resize(n);
            generator.generateRandom(matrix, 10, 99);

            for (int v = 0; v < variants.size(); v++) {
                if (earlyStop[v] == 1) {
                    continue;
                }

                bool useInitial = variants[v].second;

                auto start = chrono::high_resolution_clock::now();
                AlgResults result = BranchAndBound::bestFirstSearch(matrix, 0, useInitial, TIME_LIMIT_MS);
                auto end = chrono::high_resolution_clock::now();
                long long time = chrono::duration_cast<chrono::microseconds>(end - start).count();

                testsDone[v]++;

                if (result.interrupted) {
                    interruptedCount[v]++;
                    timeoutsInRow[v]++;
                } else {
                    solvedCount[v]++;
                    sumTimeSolved[v] += time;
                    timeoutsInRow[v] = 0;
                }

                details << n << ";"
                        << (t + 1) << ";"
                        << "BestFS" << ";"
                        << (useInitial ? 1 : 0) << ";";

                if (result.total_cost == numeric_limits<int>::max()) {
                    details << -1;
                } else {
                    details << result.total_cost;
                }

                details << ";"
                        << time << ";"
                        << (result.interrupted ? 1 : 0) << "\n";

                if (timeoutsInRow[v] >= maxTimeoutsInRow) {
                    earlyStop[v] = 1;
                }
            }

            bool stopAll = true;
            for (int v = 0; v < variants.size(); v++) {
                if (earlyStop[v] == 0) {
                    stopAll = false;
                    break;
                }
            }

            if (stopAll) {
                break;
            }
        }

        cout << "\nN = " << n << endl;

        for (int v = 0; v < variants.size(); v++) {
            bool useInitial = variants[v].second;

            double interruptedPercent = 0.0;
            if (testsDone[v] > 0) {
                interruptedPercent = 100.0 * interruptedCount[v] / testsDone[v];
            }

            double avgTimeSolved = 0.0;
            if (solvedCount[v] > 0) {
                avgTimeSolved = (double)sumTimeSolved[v] / solvedCount[v];
            }

            summary << "BestFS" << ";"
                    << (useInitial ? 1 : 0) << ";"
                    << n << ";"
                    << testsDone[v] << ";"
                    << solvedCount[v] << ";"
                    << interruptedCount[v] << ";"
                    << interruptedPercent << ";"
                    << avgTimeSolved << ";"
                    << earlyStop[v] << "\n";

            if (!useInitial) cout << "BestFS bez NN: ";
            else cout << "BestFS + NN: ";

            cout << "testsDone=" << testsDone[v]
                 << " solved=" << solvedCount[v]
                 << " interrupted=" << interruptedCount[v]
                 << " interrupted%=" << interruptedPercent
                 << " avgTimeSolved(us)=" << avgTimeSolved;

            if (earlyStop[v] == 1) {
                cout << " earlyStop=1";
            }

            cout << endl;
        }
    }

    details.close();
    summary.close();

    cout << "\nZapisano pliki bestfs_duze_wyniki.csv i bestfs_duze_podsumowanie.csv" << endl;
}

int main() {
    Matrix matrix;
    string filename;

    int choice, minval, maxval, n;

    do {
        cout << "\n1. Wczytanie danych z pliku\n";
        cout << "2. Generowanie losowej macierzy\n";
        cout << "3. Wyswietlenie macierzy\n";
        cout << "4. Brute Force\n";
        cout << "5. B&B BreadthFirstSearch\n";
        cout << "6. B&B BreadthFirstSearch + NN\n";
        cout << "7. B&B BestFirstSearch\n";
        cout << "8. B&B BestFirstSearch + NN\n";
        cout << "9. Eksperyment porownanie algorytmow\n";
        cout << "10. Eksperyment samo BestFS\n";
        cout << "11. Zakoncz\n";
        cout << "Wybor:";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Nazwa pliku: ";
                cin >> filename;
                FileReader::LoadFromFile(filename, matrix);
                break;

            case 2: {
                cout << "Rozmiar macierzy N:";
                cin >> n;
                cout << "Minimalna wartosc: ";
                cin >> minval;
                cout << "Maksymalna wartosc: ";
                cin >> maxval;

                matrix.resize(n);
                Generator generator;
                generator.generateRandom(matrix, minval, maxval);
                cout << "Wygenerowano macierz o rozmiarze N=" << n << endl;
                break;
            }

            case 3:
                matrix.print();
                break;

            case 4: {
                cout << "Brute Force" << endl;
                auto start = chrono::high_resolution_clock::now();
                AlgResults result = BruteForce::BF(matrix, 0);
                auto end = chrono::high_resolution_clock::now();
                long long time = chrono::duration_cast<chrono::microseconds>(end - start).count();
                printResult(result, time);
                break;
            }

            case 5: {
                cout << "B&B - BreadthFirstSearch" << endl;
                auto start = chrono::high_resolution_clock::now();
                AlgResults result = BranchAndBound::breadthFirstSearch(matrix, 0, false, TIME_LIMIT_MS);
                auto end = chrono::high_resolution_clock::now();
                long long time = chrono::duration_cast<chrono::microseconds>(end - start).count();
                printResult(result, time);
                break;
            }

            case 6: {
                cout << "B&B - BreadthFirstSearch (+ NN)" << endl;
                auto start = chrono::high_resolution_clock::now();
                AlgResults result = BranchAndBound::breadthFirstSearch(matrix, 0, true, TIME_LIMIT_MS);
                auto end = chrono::high_resolution_clock::now();
                long long time = chrono::duration_cast<chrono::microseconds>(end - start).count();
                printResult(result, time);
                break;
            }

            case 7: {
                cout << "B&B BestFirstSearch" << endl;
                auto start = chrono::high_resolution_clock::now();
                AlgResults result = BranchAndBound::bestFirstSearch(matrix, 0, false, TIME_LIMIT_MS);
                auto end = chrono::high_resolution_clock::now();
                long long time = chrono::duration_cast<chrono::microseconds>(end - start).count();
                printResult(result, time);
                break;
            }

            case 8: {
                cout << "B&B BestFirstSearch (+ NN)" << endl;
                auto start = chrono::high_resolution_clock::now();
                AlgResults result = BranchAndBound::bestFirstSearch(matrix, 0, true, TIME_LIMIT_MS);
                auto end = chrono::high_resolution_clock::now();
                long long time = chrono::duration_cast<chrono::microseconds>(end - start).count();
                printResult(result, time);
                break;
            }

            case 9:
                testAlgorithms();
                break;

            case 10:
                testBestFSLarge();
            break;

            case 11:
                break;

            default:
                cout << "Niepoprawna opcja" << endl;
        }

    } while (choice != 11);

    return 0;
}