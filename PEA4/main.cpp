#include <iostream>
#include "Matrix.h"
#include "AlgResults.h"
#include "FileReader.h"
#include "GeneticAlgorithm.h"
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

string mutationTypeToString(MutationType type) {
    if (type == SWAP) {
        return "SWAP";
    }
    return "INVERSION";
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

void printParams(const GeneticParameters &params) {
    cout << "Aktualne parametry:" << endl;
    cout << "Wielkosc populacji: " << params.population_size << endl;
    cout << "Wspolczynnik mutacji: " << params.mutation_rate << endl;
    cout << "Wspolczynnik krzyzowania: " << params.crossover_rate << endl;
    cout << "Metoda mutacji: " << mutationTypeToString(params.mutation_type) << endl;
    cout << "Rozmiar turnieju: " << params.tournament_size << endl;
    cout << "Limit czasu: " << params.time_limit << " ms" << endl;
    cout << "Limit generacji bez poprawy: ";
    if (params.no_improvement_limit > 0) {
        cout << params.no_improvement_limit << endl;
    }
    else {
        cout << "wylaczone" << endl;
    }
}

AlgResults runGA(const Matrix &matrix, GeneticParameters params) {
    return GeneticAlgorithm::GA(matrix, 0, params);
}

void printResult(const AlgResults &result, int opt) {
    cout << "Najlepszy koszt: " << result.total_cost << endl;
    cout << "Sredni koszt ostatniej populacji: " << fixed << setprecision(2) << result.avg_cost << endl;
    cout << "Czas: " << result.time_to_best_ms << " ms" << endl;

    if (opt > 0) {
        cout << "Blad wzgl. (najlepszy): " << fixed << setprecision(4) << calculateError(result.total_cost, opt) << " %" << endl;
        double errorAvg = 100.0 * (result.avg_cost - opt) / opt;
        cout << "Blad wzgl. (sredni): " << fixed << setprecision(4) << errorAvg << " %" << endl;
    }

    cout << "Sciezka: ";
    printPath(result.path);
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
                   GeneticParameters params,
                   const AlgResults &result) {

    double errorBest = calculateError(result.total_cost, opt);
    double errorAvg = (opt > 0) ? 100.0 * (result.avg_cost - opt) / opt : 0.0;

    file << filename << ";"
         << size << ";"
         << opt << ";"
         << testName << ";"
         << configName << ";"
         << runNumber << ";"
         << params.population_size << ";"
         << fixed << setprecision(4) << params.mutation_rate << ";"
         << params.crossover_rate << ";"
         << mutationTypeToString(params.mutation_type) << ";"
         << params.tournament_size << ";"
         << params.time_limit << ";"
         << result.total_cost << ";"
         << fixed << setprecision(2) << result.avg_cost << ";"
         << fixed << setprecision(4) << errorBest << ";"
         << fixed << setprecision(4) << errorAvg << ";"
         << result.time_to_best_ms << ";";

    savePathToFile(file, result.path);
    file << "\n";
}

void runConfigForFile(ofstream &file, string path, map<string, int> &solutions,
                      string testName, string configName, int repeats,
                      GeneticParameters params) {

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
        GeneticParameters runParams = params;
        runParams.optimal_cost = opt;
        AlgResults result = runGA(matrix, runParams);

        saveOneResult(file, name, matrix.getSize(), opt, testName, configName, i + 1,
                      runParams, result);

        cout << name << ";" << testName << ";" << configName << ";"
             << (i + 1) << ";koszt=" << result.total_cost;

        if (opt > 0) {
            cout << ";blad=" << fixed << setprecision(2) << calculateError(result.total_cost, opt) << "%";
        }

        cout << ";czas=" << result.time_to_best_ms << " ms" << endl;
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

void geneticAlgorithmTests(GeneticParameters baseParams) {
    string solutionsFile;
    int repeats;

    vector<string> files = readTestFiles();

    cout << "Plik z wynikami optymalnymi: ";
    cin >> solutionsFile;

    cout << "Liczba powtorzen kazdej konfiguracji: ";
    cin >> repeats;

    long long compSek;
    cout << "Limit czasu testow populacja/mutacja w sekundach: ";
    cin >> compSek;
    long long compTimeMs = compSek * 1000;

    map<string, int> solutions = loadSolutions(solutionsFile);

    if (files.empty()) {
        cout << "Nie podano zadnych plikow" << endl;
        return;
    }

    ofstream file("ga_results.csv");
    if (!file.is_open()) {
        cout << "Blad otwarcia ga_results.csv" << endl;
        return;
    }

    file << "Plik;N;OPT;Test;Konfiguracja;Powtorzenie;Populacja;WspMutacji;WspKrzyzowania;Mutacja;Turniej;LimitMs;NajlepszyKoszt;SredniKoszt;BladNajlepszy%;BladSredni%;CzasMs;Sciezka\n";
    file << fixed << setprecision(4);

    cout << fixed << setprecision(2);
    cout << "Start testow GA" << endl;

    for (int f = 0; f < static_cast<int>(files.size()); f++) {
        string path = files[f];

        try {
            GeneticParameters paramsRozmiar = baseParams;
            paramsRozmiar.population_size = 200;
            runConfigForFile(file, path, solutions, "rozmiar", "pop200", repeats, paramsRozmiar);

            GeneticParameters paramsPop50 = baseParams;
            paramsPop50.population_size = 50;
            paramsPop50.time_limit = compTimeMs;
            runConfigForFile(file, path, solutions, "populacja", "pop50", repeats, paramsPop50);

            GeneticParameters paramsPop100 = baseParams;
            paramsPop100.population_size = 100;
            paramsPop100.time_limit = compTimeMs;
            runConfigForFile(file, path, solutions, "populacja", "pop100", repeats, paramsPop100);

            GeneticParameters paramsPop200 = baseParams;
            paramsPop200.population_size = 200;
            paramsPop200.time_limit = compTimeMs;
            runConfigForFile(file, path, solutions, "populacja", "pop200", repeats, paramsPop200);

            GeneticParameters paramsSwap = baseParams;
            paramsSwap.population_size = 200;
            paramsSwap.mutation_type = SWAP;
            paramsSwap.time_limit = compTimeMs;
            runConfigForFile(file, path, solutions, "mutacja", "swap", repeats, paramsSwap);

            GeneticParameters paramsInv = baseParams;
            paramsInv.population_size = 200;
            paramsInv.mutation_type = INVERSION;
            paramsInv.time_limit = compTimeMs;
            runConfigForFile(file, path, solutions, "mutacja", "inversion", repeats, paramsInv);

            file.flush();
        }
        catch (const exception &e) {
            cout << "Blad dla pliku " << path << ": " << e.what() << endl;
        }
    }

    file.close();
    cout << "Zapisano wyniki do ga_results.csv" << endl;
}

void changeParameters(GeneticParameters &params) {
    int choice;

    do {
        cout << "\nModyfikacja parametrow" << endl;
        printParams(params);
        cout << "1. Wielkosc populacji" << endl;
        cout << "2. Wspolczynnik mutacji" << endl;
        cout << "3. Wspolczynnik krzyzowania" << endl;
        cout << "4. Metoda mutacji" << endl;
        cout << "5. Metoda krzyzowania" << endl;
        cout << "6. Metoda selekcji (rozmiar turnieju)" << endl;
        cout << "7. Limit generacji bez poprawy (0 = bez limitu)" << endl;
        cout << "8. Powrot" << endl;
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Podaj wielkosc populacji: ";
                cin >> params.population_size;
                break;

            case 2:
                cout << "Podaj wspolczynnik mutacji (0.0 - 1.0): ";
                cin >> params.mutation_rate;
                break;

            case 3:
                cout << "Podaj wspolczynnik krzyzowania (0.0 - 1.0): ";
                cin >> params.crossover_rate;
                break;

            case 4: {
                int type;
                cout << "0 - SWAP, 1 - INVERSION: ";
                cin >> type;

                if (type == 0) {
                    params.mutation_type = SWAP;
                }
                else {
                    params.mutation_type = INVERSION;
                }
                break;
            }

            case 5: {
                int type;
                cout << "0 - OX (Order Crossover): ";
                cin >> type;
                cout << "Wybrano: OX" << endl;
                break;
            }

            case 6:
                cout << "Podaj rozmiar turnieju: ";
                cin >> params.tournament_size;
                break;

            case 7:
                cout << "Podaj limit generacji bez poprawy (0 = bez limitu): ";
                cin >> params.no_improvement_limit;
                break;

            case 8:
                break;

            default:
                cout << "Niepoprawna opcja" << endl;
        }
    } while (choice != 8);
}

int main() {
    Matrix matrix;
    string filename;
    int choice;
    int opt = 0;

    GeneticParameters params;
    params.population_size = 100;
    params.mutation_rate = 0.1;
    params.crossover_rate = 0.8;
    params.time_limit = 900000;
    params.max_generations = 0;
    params.no_improvement_limit = 0;
    params.mutation_type = SWAP;
    params.tournament_size = 3;

    do {
        cout << "\n1. Wczytywanie danych z pliku" << endl;
        cout << "2. Wprowadzenie kryterium stopu" << endl;
        cout << "3. Podaj optymalne rozwiazanie (OPT)" << endl;
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

                case 2: {
                    long long sek;
                    cout << "Podaj limit czasu w sekundach: ";
                    cin >> sek;
                    params.time_limit = sek * 1000;
                    break;
                }

                case 3:
                    cout << "Podaj optymalne rozwiazanie (OPT): ";
                    cin >> opt;
                    break;

                case 4:
                    changeParameters(params);
                    break;

                case 5: {
                    if (matrix.getSize() == 0) {
                        cout << "Najpierw wczytaj macierz" << endl;
                        break;
                    }

                    cout << "Algorytm genetyczny:" << endl;
                    GeneticParameters runParams = params;
                    runParams.optimal_cost = opt;
                    AlgResults result = runGA(matrix, runParams);
                    printResult(result, opt);
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
                    geneticAlgorithmTests(params);
                    break;

                case 8:
                    printParams(params);
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
