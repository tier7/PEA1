#include <iostream>
#include "Matrix.h"
#include "Generator.h"
#include "NearestNeighbour.h"
#include "AlgResults.h"
#include "Random.h"
#include "BruteForce.h"
#include "RepetativeNearestNeighbour.h"
#include "FileReader.h"
#include <string>
#include <chrono>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;


using namespace std;

void printResult(const AlgResults& result, long long time) {
    cout << "Koszt: " << result.total_cost << endl;
    cout << "Sciezka: ";
    for (int i = 0; i < result.path.size(); i++) {
        cout << result.path[i];
        cout<<" ";
    }
    cout << endl;
    cout << "Czas: " << time << " us" << endl;
}

double calculateError(int algCost, int bestCost) {
    if (bestCost == 0) return 0.0;
    return 100.00*(algCost - bestCost) / bestCost;
}

void bruteForceTest() {
    cout << "Test czasowy BF" << endl;
    vector<int> sizes = {8, 9, 10, 11, 12, 13, 14};
    Generator generator;

    ofstream file("bruteForce_results.csv");
    if (!file.is_open()) {
        cout << "Blad otwarcia bruteForce_results.csv" << endl;
        return;
    }

    cout << fixed << setprecision(3);
    file << fixed << setprecision(3);

    cout << "N;Koszt;Czas(ms)" << endl;
    file << "N;Koszt;Czas(ms)\n";

    for (int n : sizes) {
        Matrix matrix;
        matrix.resize(n);
        generator.generateRandom(matrix, 1, 100);

        auto start = chrono::high_resolution_clock::now();
        AlgResults result = BruteForce::BF(matrix, 0);
        auto end = chrono::high_resolution_clock::now();

        long long time = chrono::duration_cast<chrono::microseconds>(end - start).count();
        double time_ms = time / 1000.0;

        cout << n << ";" << result.total_cost << ";" << time_ms << endl;
        file << n << ";" << result.total_cost << ";" << time_ms << "\n";

        file.flush();
    }

    file.close();
    cout << "Zapisano do bruteForce_results.csv" << endl;
}

void compareAlgorithms() {
    cout << "Porownanie algorytmow" << endl;
    Generator generator;
    int tests;
    cout << "Liczba powtorzen dla kazdego N: ";
    cin >> tests;

    ofstream file("compareAlg_results.csv");
    if (!file.is_open()) {
        cout << "Blad otwarcia compareAlg_results.csv" << endl;
        return;
    }

    file << "N;TestNr;BFCost;NNCost;NNError;RNNCost;RNNError;RNGCost;RNGError\n";
    file << fixed << setprecision(2);

    cout <<"N;AvgNNError;AvgRNNError;AvgRNGError" << endl;
    cout << fixed << setprecision(2);

    for (int size = 10; size < 15; size++) {
        double sumNN = 0.0;
        double sumRNN = 0.0;
        double sumRNG = 0.0;

        for (int i = 0; i < tests; i++) {
            Matrix matrix;
            matrix.resize(size);
            generator.generateRandom(matrix, 1, 100);

            AlgResults BF = BruteForce::BF(matrix, 0);
            AlgResults NN = NearestNeighbour::NN(matrix, 0);
            AlgResults RNN = RepetativeNearestNeighbour::RNN(matrix, 0);
            AlgResults RNG = Random::RandomAlg(matrix, 0, 10 * size);
            double errNN = calculateError(NN.total_cost, BF.total_cost);
            double errRNN = calculateError(RNN.total_cost, BF.total_cost);
            double errRNG = calculateError(RNG.total_cost, BF.total_cost);

            sumNN += errNN;
            sumRNN += errRNN;
            sumRNG += errRNG;

            file << size << ";" << (i + 1) << ";" << BF.total_cost << ";" << NN.total_cost << ";" << errNN << ";"
                 << RNN.total_cost << ";" << errRNN << ";" << RNG.total_cost << ";" << errRNG << "\n";

            file.flush();
        }

        cout << size << ";"
             << (sumNN / tests) << ";"
             << (sumRNN / tests) << ";"
             << (sumRNG / tests) << endl;
    }
    file.close();
    cout << "Zapisano do compareAlg_results.csv" << endl;
}


int main() {
    Matrix matrix;
    string filename;

    int choice, minval,maxval,n,p;
    do {
        cout << "\n1. Wczytywanie danych z pliku\n";
        cout << "2. Generowanie losowej macierzy\n";
        cout << "3. Wyswietlenie macierzy\n";
        cout << "4. Algorytm brute force \n";
        cout << "5. Algorytm NN\n";
        cout << "6. Algorytm losowy\n";
        cout<< "7. Algorytm RNN\n";
        cout << "8. Test brute force\n";
        cout << "9. Porownanie algorytmow\n ";
        cout <<"10. Zakoncz\n";
        cin >> choice;
        switch (choice) {
            case 1:
                cout<<"Nazwa pliku: "<<endl;
            cin>>filename;
            FileReader::LoadFromFile(filename, matrix);
            break;
            case 2:
                cout<<"Rozmiaar macierzy N: ";
            cin>>n;
            cout<<"Minimalna wartosc: ";
            cin>>minval;
            cout<<"Maksymalna wartosc: ";
            cin>>maxval;
            matrix.resize(n);
            Generator generator;
            generator.generateRandom(matrix, minval, maxval);
            break;
            case 3:
                matrix.print();
            break;
            case 4: {
                cout<<"Algorytm Brute Force: \n";
                auto start = chrono::high_resolution_clock::now();
                AlgResults BF = BruteForce::BF(matrix, 0);
                auto end = chrono::high_resolution_clock::now();
                long long executionTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
                printResult(BF, executionTime);
                break;
            }
            case 5: {
                cout<<"Algorytm NN: \n";
                auto start = chrono::high_resolution_clock::now();
                AlgResults NN = NearestNeighbour::NN (matrix, 0);
                auto end = chrono::high_resolution_clock::now();
                long long executionTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
                printResult(NN, executionTime);
                break;
            }
            case 6: {
                cout<<"Podaj ilosc permutacji: ";
                cin>>p;
                cout<<"Algorytm losowy: \n";
                auto start = chrono::high_resolution_clock::now();
                AlgResults RNG = Random::RandomAlg(matrix, 0, p);
                auto end = chrono::high_resolution_clock::now();
                long long executionTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
                printResult(RNG, executionTime);
                break;
            }
            case 7: {
                cout<<"Algorytm RNN: \n";
                auto start = chrono::high_resolution_clock::now();
                AlgResults RNN = RepetativeNearestNeighbour::RNN(matrix, 0);
                auto end = chrono::high_resolution_clock::now();
                long long executionTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
                printResult(RNN, executionTime);
                break;
            }
            case 8: {
                bruteForceTest();
                break;
            }
            case 9: {
                compareAlgorithms();
                break;
            }

            default:
                std::cout << "Niepoprawna opcja\n";
        }
    } while (choice != 10);
    return 0;
}