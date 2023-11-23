#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <fstream>


using namespace std;

const int EMPTY_CELL = -1;

struct Placer {
    int numOfComponents, numOfNets, ny, nx;
    vector<vector<int>> nets;
};

int** makeCore(int nx, int ny) {
    int** core = new int* [nx];
    for (int i = 0; i < nx; i++) {
        core[i] = new int[ny];
        for (int j = 0; j < ny; j++) {
            core[i][j] = EMPTY_CELL;
        }
    }
    return core;
}

void placeRandomly(int** core, int numOfComponents, int nx, int ny) {
    for (int i = 0; i < numOfComponents; i++) {
        int row = rand() % nx;
        int col = rand() %  ny;
        bool placed = false;
        while (!placed) {
            if (core[row][col] == EMPTY_CELL) {
                core[row][col] = i;
                placed = true;
            }
            row = rand() % nx;
            col = rand() % ny;
            
        }
    }
}

void printToConsole(int** core, int nx, int ny) {
    const int cellWidth = 5;

    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            if (core[i][j] == EMPTY_CELL) {
                cout << setw(cellWidth) << left << "--";
            }
            else {
                cout << setw(cellWidth) << left << core[i][j];
            }
        }
        cout << endl;
    }
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
    }
    else {
        cout << "NO OPEN FILE";
    }

    return p;
}

int main() {
    Placer p = makePlacer("C:\\Users\\elsha\\Desktop\\IC\\SimulatedAnealling\\t1.txt");
    cout << "EMPTY CELL: " << EMPTY_CELL<<endl;
    int** core = makeCore(p.nx, p.ny);
    placeRandomly(core, p.numOfComponents, p.nx, p.ny);
    printToConsole(core, p.nx, p.ny);
    // todo: hpwl function, temp scheduling, sa algo
    return 0;
}
