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



using namespace std;

const int EMPTY_CELL = -1;
const int INIT_TEMP = 500;
const double FINAL_TEMP = 5e-6;
const float COOLING_RATE = 0.95;
const int MOVES = 10;


struct Placer {
    int numOfComponents, numOfNets, ny, nx;
    double initialTemp;
    double finalTemp;
    int movesPerTemp;
    vector<vector<int>> nets;
    vector<tuple<int, int>> cellPositions;
};

int hpwl(Placer& p) {
    
    int hpwl = 0;
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
            
            if (find(inserted.begin(), inserted.end(), make_tuple(row,col))==inserted.end()) {
                p.cellPositions[i] = { row, col };
                inserted.push_back({ row, col });
                placed = true;
            }
            row = rand() % p.nx;
            col = rand() % p.ny;

        }
    }

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

void swap(int x, int y, Placer &p) {
    //tuple<int, int> temp = p.cellPositions[x];
    //p.cellPositions[x] = p.cellPositions[y];
    //p.cellPositions[y] = temp;
    iter_swap(p.cellPositions.begin() + x, p.cellPositions.begin() + y);

    //cout << "CELL A " << cellA << endl;
    //cout << "CELL B " << cellB<<endl;
}



void simulatedAnealing(Placer& p) {
    int initialCost = hpwl( p);
    p.initialTemp = INIT_TEMP * initialCost;
    p.finalTemp = FINAL_TEMP * ((double)initialCost / p.numOfNets);
    double temp = p.initialTemp;
    const int cols = p.ny;
    vector<tuple<int, int>> oldVec(p.nx * p.ny);
    int totalCells = p.nx * p.ny;
    while (temp > p.finalTemp) {
        for (int i = 0; i < p.movesPerTemp; i++) {
            int costI = hpwl(p);
            oldVec = p.cellPositions;
            int swap1 = rand() % totalCells;
            int swap2 = rand() % totalCells;

            swap(swap1, swap2, p);
            int costF = hpwl( p);
            int deltaCost = costF - costI;
            if (deltaCost > 0 && (rand()/RAND_MAX) < (1-exp((double(-deltaCost) / temp)))) {

                p.cellPositions = oldVec;

            }
            
        }

        printToConsole( p);

       
        temp = 0.95 * temp;
    }
}



int main() {
    srand(time(NULL));

    Placer p = makePlacer("C:\\Users\\elsha\\Desktop\\IC\\SimulatedAnealling\\d0.txt");
    makeCore(p);
    placeRandomly( p);
    printToConsole(p);

    simulatedAnealing(p);

    // todo: temp scheduling, sa algo
    return 0;
}
