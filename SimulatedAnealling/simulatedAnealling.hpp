#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include<tuple> // for tuple
#include <time.h>
#include<math.h>
#include<algorithm>
#include <random>




using namespace std;

const int EMPTY_CELL = -1;
const int INIT_TEMP = 500;
const double FINAL_TEMP = 5e-6;
const float COOLING_RATE = 0.95;
const int MOVES = 10;

struct Cell {
    int value;
    int posX;
    int posY;
    int net;
};

struct Placer {
    int numOfComponents, numOfNets, ny, nx;
    double initialTemp;
    double finalTemp;
    int movesPerTemp;
    vector<vector<int>> nets;
    vector<tuple<int, int>> cellPositions;
    vector<Cell> cells;
    int initCost;
    vector<int> MaxX;
    vector<int> MinX;
    vector<int> MaxY;
    vector<int> MinY;
};

int hpwl(Placer& p) {

    int hpwl = 0;
    p.MaxX.clear();
    p.MinX.clear();
    p.MaxY.clear();
    p.MinY.clear();
    for (int i = 0; i < p.nets.size(); i++) {
        vector<tuple<int, int>> net;
        int maxX = -1;
        int maxY = -1;
        int minX = 9999999;
        int minY = 9999999;
        for (int j = 0; j < p.nets[i].size(); j++) {
            int currentX = get<0>(p.cellPositions[p.nets[i][j]]);
            int currentY = get<1>(p.cellPositions[p.nets[i][j]]);
            if (currentX < minX) {
                minX = currentX;
            }
            if (currentX > maxX) {
                maxX = currentX;
            }
            if (currentY < minY) {
                minY = currentY;
            }
            if (currentY > maxY) {
                maxY = currentY;
            }
        }
        p.MaxX.push_back(maxX);
        p.MinX.push_back(minX);
        p.MaxY.push_back(maxY);
        p.MinY.push_back(minY);

        hpwl += (maxX - minX);
        hpwl += (maxY - minY);
    }
    return hpwl;
}

void makeCore(Placer& p) {
    p.cellPositions.resize(p.nx * p.ny);
    for (int i = 0; i < p.nx * p.ny; i++) {
        p.cellPositions[i] = { 0,0 };
    }

}

void placeRandomly(Placer& p) {
    vector<tuple<int, int>> inserted;
    for (int i = 0; i < p.cellPositions.size(); i++) {
        int row = rand() % p.nx;
        int col = rand() % p.ny;
        bool placed = false;

        while (!placed) {

            if (find(inserted.begin(), inserted.end(), make_tuple(row, col)) == inserted.end()) {
                p.cellPositions[i] = { row, col };
                inserted.push_back({ row, col });
                placed = true;
            }
            row = rand() % p.nx;
            col = rand() % p.ny;

        }
    }

}

int** getGrid(Placer& p) {
    int** arr2d = new int* [p.nx];
    for (int i = 0; i < p.nx; i++) {
        arr2d[i] = new int[p.ny];
        for (int j = 0; j < p.ny; j++) {
            arr2d[i][j] = EMPTY_CELL;
        }
    }
    for (int i = 0; i < p.cellPositions.size(); i++) {
        if (i < p.numOfComponents) {
            arr2d[get<0>(p.cellPositions[i])][get<1>(p.cellPositions[i])] = i;
        }
    }

    return arr2d;
}

Placer makePlacer(string inputFile) {
    ifstream file;
    Placer p;

    file.open(inputFile);
    if (file.is_open()) { // always check whether the file is open
        file >> p.numOfComponents; // pipe file's content into stream
        file >> p.numOfNets;
        file >> p.nx;
        file >> p.ny;

        p.nets.resize(p.numOfNets);

        for (int i = 0; i < p.numOfNets; i++) {
            int netComponents;

            file >> netComponents;

            for (int k = 0; k < netComponents; k++) {
                int comp;

                file >> comp;

                p.nets[i].push_back(comp);
            }
        }
        p.movesPerTemp = MOVES * p.numOfComponents;
    }
    else {
        cout << "NO OPEN FILE";
    }

    return p;
}

void swap(int x, int y, Placer& p) {
    iter_swap(p.cellPositions.begin() + x, p.cellPositions.begin() + y);
}

void printToConsole(Placer& p) {
    int** arr2d = new int* [p.nx];
    for (int i = 0; i < p.nx; i++) {
        arr2d[i] = new int[p.ny];
        for (int j = 0; j < p.ny; j++) {
            arr2d[i][j] = EMPTY_CELL;
        }
    }
    for (int i = 0; i < p.cellPositions.size(); i++) {
        if (i < p.numOfComponents) {
            arr2d[get<0>(p.cellPositions[i])][get<1>(p.cellPositions[i])] = i;
        }
    }

    const int cellWidth = 5;

    for (int i = 0; i < p.nx; i++) {
        for (int j = 0; j < p.ny; j++) {
            if (arr2d[i][j] == EMPTY_CELL) {
                cout << setw(cellWidth) << left << "--";
            }
            else {
                cout << setw(cellWidth) << left << arr2d[i][j];
            }
        }
        cout << endl;
    }
    cout << "HPWL: " << hpwl(p) << endl;
}

int costI = 9999999999999;

vector<tuple<int, int>> oldVec;
uniform_real_distribution<double> dis2;

void initializeTemp(Placer& p) {
    p.initCost = hpwl(p);
    p.initialTemp = INIT_TEMP * p.initCost;
    p.finalTemp = FINAL_TEMP * (static_cast<double>(p.initCost) / p.numOfNets);
    costI = p.initCost;
    oldVec.resize(p.nx * p.ny);
}

void simulatedAnealing(Placer& p, double temp) {

    const int cols = p.ny;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, (p.nx * p.ny) - 1);
    uniform_real_distribution<double> distribution(0.0, 1.0);

    int swap1, swap2, costF, deltaCost;


    for (int i = 0; i < p.movesPerTemp; i++) {
        oldVec = p.cellPositions;
        swap1 = dis(gen);
        swap2 = dis(gen);

        swap(swap1, swap2, p);
        costF = hpwl(p);
        deltaCost = costF - costI;

        if (deltaCost > 0 && (distribution(gen)) < (1 - exp(static_cast<double>(-deltaCost) / temp))) {
            p.cellPositions = oldVec;
        }
        else {
            costI = costF;
            //cout << "DELTA" << distribution(gen) <<"    "<< (1 - exp(-(double)deltaCost / temp)) << endl;
        }
    }

    //printToConsole(p);

}




void runSim(string inputFile) {
    srand(time(NULL));
    //"C:\\Users\\elsha\\Desktop\\IC\\SimulatedAnealling\\t3.txt"
    Placer p = makePlacer(inputFile);
    makeCore(p);
    placeRandomly(p);
    //printToConsole(p);

    //simulatedAnealing(p);
    //printToConsole(p);

    // todo: temp scheduling, sa algo
}
