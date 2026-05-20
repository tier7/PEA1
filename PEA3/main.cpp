#include <iostream>
#include "Matrix.h"
#include "AlgResults.h"
#include "FileReader.h"
#include "SimulatedAnnealing.h"
#include <string>
#include <chrono>
#include <vector>
#include <fstream>
#include <iomanip>
#include <map>
#include <limits>
#include <filesystem>
#include <algorithm>

using namespace std;

void printPath(const vector<int> &path) {
    for (int i = 0; i < static_cast<int>(path.size()); i++) {
        cout << path[i] << " ";
    }
    cout << endl;
}

void printResult(const AlgResults &result, long long time, const Matrix &matrix) {
    cout << "Koszt: " << result.total_cost << endl;
    cout << "Sciezka: ";
    printPath(result.path);
    cout << "Czas: " << time << " us" << endl;

    if (result.interrupted) {
        cout << "Przerwano po przekroczeniu limitu czasu" << endl;
    }
}

string initTypeToString(InitialSolutionType type) {
    if (type == RANDOM) {
        return "RANDOM";
    }
    return "NEAREST_NEIGHBOUR";
}

string coolingSchemeToString(CoolingScheme scheme) {
    if (scheme == LINEAR) {
        return "LINEAR";
    }
    return "EXPONENTIAL";
}

double calculateError(int algCost, int bestCost) {
    if (bestCost <= 0) {
        return 0.0;
    }

    if (algCost == numeric_limits<int>::max()) {
        return 1000000.0;
    }

    return 100.0 * (algCost - bestCost) / bestCost;
}

string removeSpaces(string text) {
    string result = "";

    for (int i = 0; i < static_cast<int>(text.size()); i++) {
        if (text[i] != ' ' && text[i] != '\t' && text[i] != '\r' && text[i] != '\n') {
            result += text[i];
        }
    }

    return result;
}

string getFileName(string path) {
    int lastSlash = -1;

    for (int i = 0; i < static_cast<int>(path.size()); i++) {
        if (path[i] == '/' || path[i] == '\\') {
            lastSlash = i;
        }
    }

    string name = path.substr(lastSlash + 1);

    int lastDot = -1;
    for (int i = 0; i < static_cast<int>(name.size()); i++) {
        if (name[i] == '.') {
            lastDot = i;
        }
    }

    if (lastDot != -1) {
        name = name.substr(0, lastDot);
    }

    return name;
}

map<string, int> loadSolutions(const string &filename) {
    map<string, int> solutions;
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Nie mozna otworzyc pliku z rozwiazaniami" << endl;
        return solutions;
    }

    string line;
    while (getline(file, line)) {
        size_t colon = line.find(':');

        if (colon == string::npos) {
            continue;
        }

        string name = line.substr(0, colon);
        string value = line.substr(colon + 1);

        name = removeSpaces(name);
        value = removeSpaces(value);

        if (name != "" && value != "") {
            solutions[name] = stoi(value);
        }
    }

    file.close();
    return solutions;
}

void printParams(const Parameters &params, InitialSolutionType initType) {
    cout << "Aktualne parametry:" << endl;
    cout << "Rozwiazanie poczatkowe: " << initTypeToString(initType) << endl;
    cout << "Temperatura poczatkowa: " << params.start_temperature << endl;
    cout << "Temperatura koncowa: " << params.end_temperature << endl;
    cout << "Chlodzenie: " << params.cooling << endl;
    cout << "Dlugosc epoki: " << params.epochs << endl;
    cout << "Limit czasu: " << params.time_limit << " ms" << endl;
    cout << "Schemat chlodzenia: " << coolingSchemeToString(params.cooling_scheme) << endl;
}

AlgResults runSA(const Matrix &matrix, Parameters params, InitialSolutionType initType, long long &time) {
    auto start = chrono::high_resolution_clock::now();
    AlgResults result = SimulatedAnnealing::SA(matrix, 0, initType, params);
    auto end = chrono::high_resolution_clock::now();

    time = chrono::duration_cast<chrono::microseconds>(end - start).count();
    return result;
}

void calculateInitialSolution(const Matrix &matrix, InitialSolutionType initType) {
    cout << "Rozwiazanie poczatkowe: " << initTypeToString(initType) << endl;

    auto start = chrono::high_resolution_clock::now();
    vector<int> path = SimulatedAnnealing::initialSolution(initType, 0, matrix);
    auto end = chrono::high_resolution_clock::now();

    long long time = chrono::duration_cast<chrono::microseconds>(end - start).count();
    int cost = SimulatedAnnealing::calculateCost(matrix, path);

    cout << "Koszt: " << cost << endl;
    cout << "Sciezka: ";
    printPath(path);
    cout << "Czas: " << time << " us" << endl;
}

void savePathToFile(ofstream &file, const vector<int> &path) {
    for (int i = 0; i < static_cast<int>(path.size()); i++) {
        file << path[i];

        if (i + 1 < static_cast<int>(path.size())) {
            file << " ";
        }
    }
}

void saveOneResult(ofstream &file, string filename, int size, int opt,
                   string testName, string configName, int runNumber,
                   Parameters params, InitialSolutionType initType,
                   const AlgResults &result, long long time, const Matrix &matrix) {

    int checkedCost = SimulatedAnnealing::calculateCost(matrix, result.path);
    double error = calculateError(result.total_cost, opt);

    file << filename << ";"
         << size << ";"
         << opt << ";"
         << testName << ";"
         << configName << ";"
         << runNumber << ";"
         << initTypeToString(initType) << ";"
         << coolingSchemeToString(params.cooling_scheme) << ";"
         << params.start_temperature << ";"
         << params.end_temperature << ";"
         << params.cooling << ";"
         << params.epochs << ";"
         << params.time_limit << ";"
         << result.total_cost << ";"
         << checkedCost << ";"
         << fixed << setprecision(4) << error << ";"
         << time << ";"
         << result.interrupted << ";";

    savePathToFile(file, result.path);
    file << "\n";
}

void runConfigForFile(ofstream &file, string path, map<string, int> &solutions,
                      string testName, string configName, int repeats,
                      Parameters params, InitialSolutionType initType) {

    Matrix matrix;
    FileReader::LoadFromFile(path, matrix);

    string name = getFileName(path);
    int opt = 0;

    if (solutions.find(name) != solutions.end()) {
        opt = solutions[name];
    }
    else {
        cout << "Brak OPT dla pliku: " << name << endl;
    }

    for (int i = 0; i < repeats; i++) {
        long long time = 0;
        AlgResults result = runSA(matrix, params, initType, time);

        saveOneResult(file, name, matrix.getSize(), opt, testName, configName, i + 1,
                      params, initType, result, time, matrix);

        cout << name << ";" << testName << ";" << configName << ";"
             << (i + 1) << ";koszt=" << result.total_cost;

        if (opt > 0) {
            cout << ";blad=" << fixed << setprecision(2) << calculateError(result.total_cost, opt) << "%";
        }

        cout << ";czas=" << time / 1000.0 << " ms" << endl;
    }
}

vector<string> readTestFiles() {
    vector<string> files;
    string folder;

    cout << "Folder z plikami .atsp: ";
    cin >> folder;

    try {
        for (const auto &entry : std::filesystem::directory_iterator(folder)) {
            if (entry.is_regular_file()) {
                string extension = entry.path().extension().string();

                if (extension == ".atsp" || extension == ".ATSP") {
                    files.push_back(entry.path().string());
                }
            }
        }
    }
    catch (const exception &e) {
        cout << "Blad odczytu folderu: " << e.what() << endl;
        return files;
    }

    sort(files.begin(), files.end());

    cout << "Znaleziono plikow .atsp: " << files.size() << endl;
    for (int i = 0; i < static_cast<int>(files.size()); i++) {
        cout << (i + 1) << ". " << files[i] << endl;
    }

    return files;
}

void simulatedAnnealingTests(Parameters baseParams) {
    string solutionsFile;
    int repeats;

    vector<string> files = readTestFiles();

    cout << "Plik z wynikami optymalnymi: ";
    cin >> solutionsFile;

    cout << "Liczba powtorzen kazdej konfiguracji: ";
    cin >> repeats;

    map<string, int> solutions = loadSolutions(solutionsFile);

    if (files.empty()) {
        cout << "Nie podano zadnych plikow" << endl;
        return;
    }

    ofstream file("sa_results.csv");
    if (!file.is_open()) {
        cout << "Blad otwarcia sa_results.csv" << endl;
        return;
    }

    file << "Plik;N;OPT;Test;Konfiguracja;Powtorzenie;Start;Schemat;TempStart;TempEnd;Chlodzenie;Epoka;LimitMs;Koszt;KosztSprawdzony;BladProc;CzasUs;Przerwany;Sciezka\n";
    file << fixed << setprecision(4);

    cout << fixed << setprecision(2);
    cout << "Start testow SA" << endl;

    for (int f = 0; f < static_cast<int>(files.size()); f++) {
        string path = files[f];

        try {
            // Zaleznosc czasu i bledu od rozmiaru instancji
            Parameters paramsSize = baseParams;
            runConfigForFile(file, path, solutions, "rozmiar", "domyslne", repeats, paramsSize, NEARESTNEIGHBOUR);

            // Wplyw rozwiazania poczatkowego
            Parameters paramsRandom = baseParams;
            runConfigForFile(file, path, solutions, "start", "random", repeats, paramsRandom, RANDOM);

            Parameters paramsNN = baseParams;
            runConfigForFile(file, path, solutions, "start", "nearest_neighbour", repeats, paramsNN, NEARESTNEIGHBOUR);

            // Wplyw schematu chlodzenia
            Parameters paramsExp = baseParams;
            paramsExp.cooling_scheme = EXPONENTIAL;
            paramsExp.cooling = 0.999;
            runConfigForFile(file, path, solutions, "chlodzenie", "exponential_0_999", repeats, paramsExp, NEARESTNEIGHBOUR);

            Parameters paramsLin = baseParams;
            paramsLin.cooling_scheme = LINEAR;
            paramsLin.cooling = 28.0;
            runConfigForFile(file, path, solutions, "chlodzenie", "linear_28", repeats, paramsLin, NEARESTNEIGHBOUR);

            file.flush();
        }
        catch (const exception &e) {
            cout << "Blad dla pliku " << path << ": " << e.what() << endl;
        }
    }

    file.close();
    cout << "Zapisano wyniki do sa_results.csv" << endl;
}

void changeParameters(Parameters &params, InitialSolutionType &initType) {
    int choice;

    do {
        cout << "\nModyfikacja parametrow" << endl;
        printParams(params, initType);
        cout << "1. Temperatura poczatkowa" << endl;
        cout << "2. Temperatura koncowa" << endl;
        cout << "3. Chlodzenie" << endl;
        cout << "4. Dlugosc epoki" << endl;
        cout << "5. Schemat chlodzenia" << endl;
        cout << "6. Rozwiazanie poczatkowe" << endl;
        cout << "7. Powrot" << endl;
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Podaj temperature poczatkowa: ";
                cin >> params.start_temperature;
                break;

            case 2:
                cout << "Podaj temperature koncowa: ";
                cin >> params.end_temperature;
                break;

            case 3:
                cout << "Podaj wartosc chlodzenia: ";
                cin >> params.cooling;
                break;

            case 4:
                cout << "Podaj dlugosc epoki: ";
                cin >> params.epochs;
                break;

            case 5: {
                int type;
                cout << "0 - liniowe, 1 - wykladnicze: ";
                cin >> type;

                if (type == 0) {
                    params.cooling_scheme = LINEAR;
                }
                else {
                    params.cooling_scheme = EXPONENTIAL;
                }
                break;
            }

            case 6: {
                int type;
                cout << "0 - losowe, 1 - NN: ";
                cin >> type;

                if (type == 0) {
                    initType = RANDOM;
                    params.initial_solution_type = RANDOM;
                }
                else {
                    initType = NEARESTNEIGHBOUR;
                    params.initial_solution_type = NEARESTNEIGHBOUR;
                }
                break;
            }

            case 7:
                break;

            default:
                cout << "Niepoprawna opcja" << endl;
        }
    } while (choice != 7);
}

int main() {
    Matrix matrix;
    string filename;
    int choice;

    Parameters params;
    params.start_temperature = 500000.0;
    params.end_temperature = 0.01;
    params.cooling = 0.999;
    params.epochs = 20000;
    params.time_limit = 900000;
    params.cooling_scheme = EXPONENTIAL;
    params.initial_solution_type = NEARESTNEIGHBOUR;


    InitialSolutionType initType = NEARESTNEIGHBOUR;

    do {
        cout << "\n1. Wczytywanie danych z pliku" << endl;
        cout << "2. Wprowadzenie kryterium stopu" << endl;
        cout << "3. Obliczanie rozwiazania poczatkowego" << endl;
        cout << "4. Modyfikacja ustawien algorytmu" << endl;
        cout << "5. Uruchom algorytm" << endl;
        cout << "6. Wyswietlenie macierzy" << endl;
        cout << "7. Testy do CSV" << endl;
        cout << "8. Wyswietlenie parametrow" << endl;
        cout << "9. Zakoncz" << endl;
        cin >> choice;

        try {
            switch (choice) {
                case 1:
                    cout << "Nazwa pliku: " << endl;
                    cin >> filename;
                    FileReader::LoadFromFile(filename, matrix);
                    cout << "Wczytano plik" << endl;
                    cout << "Rozmiar macierzy: " << matrix.getSize() << endl;
                    break;

                case 2:
                    cout << "Podaj limit czasu w ms: ";
                    cin >> params.time_limit;
                    break;

                case 3:
                    if (matrix.getSize() == 0) {
                        cout << "Najpierw wczytaj macierz" << endl;
                        break;
                    }
                    calculateInitialSolution(matrix, initType);
                    break;

                case 4:
                    changeParameters(params, initType);
                    break;

                case 5: {
                    if (matrix.getSize() == 0) {
                        cout << "Najpierw wczytaj macierz" << endl;
                        break;
                    }

                    cout << "Algorytm symulowanego wyzarzania:" << endl;
                    long long executionTime = 0;
                    AlgResults result = runSA(matrix, params, initType, executionTime);
                    printResult(result, executionTime, matrix);
                    break;
                }

                case 6:
                    if (matrix.getSize() == 0) {
                        cout << "Najpierw wczytaj macierz" << endl;
                        break;
                    }
                    matrix.print();
                    break;

                case 7:
                    simulatedAnnealingTests(params);
                    break;

                case 8:
                    printParams(params, initType);
                    break;

                case 9:
                    break;

                default:
                    cout << "Niepoprawna opcja" << endl;
            }
        }
        catch (const exception &e) {
            cout << "Blad: " << e.what() << endl;
        }
    } while (choice != 9);

    return 0;
}