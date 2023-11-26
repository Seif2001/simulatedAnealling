#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include<tuple> // for tuple
#include <time.h>
#include<math.h>



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
};

int hpwl(int** core, Placer& p) {
    vector<tuple<int, int>> cellPositions(p.numOfComponents);
    for (int i = 0; i < p.nx; i++) {
        for (int j = 0; j < p.ny; j++) {
            if (core[i][j] != EMPTY_CELL) {
                cellPositions[core[i][j]] = { i, j };
            }
        }
    }
    int hpwl = 0;
    for (int i = 0; i < p.nets.size(); i++) {
        vector<tuple<int, int>> net;
        int maxX = -1;
        int maxY = -1;
        int minX = 9999999;
        int minY = 9999999;
        for (int j = 0; j < p.nets[i].size(); j++) {
            int currentX = get<0>(cellPositions[p.nets[i][j]]);
            int currentY = get<1>(cellPositions[p.nets[i][j]]);
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
        //int maxX = get<0>(*max_element(net.begin(), net.end(),
            //[](auto& l, auto& r) {return get<0>(l) < get<0>(r);}));
        //int maxY = get<1>(*max_element(net.begin(), net.end(),
            //[](auto& l, auto& r) {return get<1>(l) < get<1>(r);}));
        //int minX = get<0>(*min_element(net.begin(), net.end(),
            //[](auto& l, auto& r) {return get<0>(l) < get<0>(r);}));
        //int minY = get<1>(*min_element(net.begin(), net.end(),
            //[](auto& l, auto& r) {return get<1>(l) < get<1>(r);}));
        hpwl += (maxX - minX);
        hpwl += (maxY - minY);
    }
    return hpwl;
}

int** makeCore(Placer& p) {
        int** core = new int* [p.nx];
        for (int i = 0; i < p.nx; i++) {
            core[i] = new int[p.ny];
            for (int j = 0; j < p.ny; j++) {
                core[i][j] = EMPTY_CELL;
            }
        }
        return core;
}

void placeRandomly(int** core, Placer& p) {
    for (int i = 0; i < p.numOfComponents; i++) {
        int row = rand() % p.nx;
        int col = rand() % p.ny;
        bool placed = false;
        while (!placed) {
            if (core[row][col] == EMPTY_CELL) {
                core[row][col] = i;
                placed = true;
            }
            row = rand() % p.nx;
            col = rand() % p.ny;

        }
    }
}

void printToConsole(int** core, Placer& p) {
    const int cellWidth = 5;

    for (int i = 0; i < p.nx; i++) {
        for (int j = 0; j < p.ny; j++) {
            if (core[i][j] == EMPTY_CELL) {
                cout << setw(cellWidth) << left << "--";
            }
            else {
                cout << setw(cellWidth) << left << core[i][j];
            }
        }
        cout << endl;
    }
    cout << "HPWL: " << hpwl(core, p) << endl;
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

void swap(int x1, int y1, int x2, int y2, int **core) {
    int temp = core[x1][y1];
    core[x1][y1] = core[x2][y2];
    core[x2][y2] = temp;
    //cout << "CELL A " << cellA << endl;
    //cout << "CELL B " << cellB<<endl;
}

int** deepCopy(int** core, Placer p) {
    int** oldCore = new int* [p.nx];
    for (int i = 0; i < p.nx; i++) {
        oldCore[i] = new int [p.ny];
    }
    
    for (int x = 0; x < p.nx; x++)
    {
        for (int y = 0; y < p.ny; y++)
        {
                oldCore[x][y] = core[x][y];
        }
    }
    return oldCore;
}

void simulatedAnealing(Placer& p, int** core) {
    int initialCost = hpwl(core, p);
    p.initialTemp = INIT_TEMP * initialCost;
    p.finalTemp = FINAL_TEMP * ((double)initialCost / p.numOfNets);
    double temp = p.initialTemp;
    const int cols = p.ny;
    while (temp > p.finalTemp) {
        for (int i = 0; i < p.movesPerTemp; i++) {
            int costI = hpwl(core, p);
            int** oldCore = deepCopy(core, p);
            int Ax, Ay, Bx, By;
            do {
                Ax = rand() % p.nx;
                Ay = rand() % p.ny;
                Bx = rand() % p.nx;
                By = rand() % p.ny;
            } while (core[Ax][Ay] == core[Bx][By]);

            swap(Ax,Ay, Bx,By, core);
            //printToConsole(core, p);
            int costF = hpwl(core, p);
            int deltaCost = costF - costI;  
            if (deltaCost > 0) {

                core = oldCore;

            }
            //cout << i << endl;
            //&& (rand())< (1 - exp((double(-deltaCost)/temp)))
            //cout << "DELTAAAA: " << deltaCost << endl;
        }

        printToConsole(core, p);

        if (hpwl(core, p) == 0) {
            break;
        }
        temp = 0.95 * temp;
    }
}



int main() {
    srand(time(NULL));

    Placer p = makePlacer("C:\\Users\\elsha\\Desktop\\IC\\SimulatedAnealling\\d0.txt");
    int** core = makeCore(p);
    placeRandomly(core, p);
    printToConsole(core, p);

    simulatedAnealing(p, core);

    // todo: temp scheduling, sa algo
    return 0;
}

/* The above code has some issues that affects its performance as using dynamic array, 
and calling deep copy function in a while loop in the simulatedAnnealing function. This can be a performance bottleneck, besides the nested loops with N^2 time complexity*/ 

/* The below is an updated function that works with vectors and avoid calling deep copy function in a while loop in the simulatedAnnealing function.
    Instead, I made a copy only when the move is accepted, and try to reduce loops complexity to N */

/*

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace chrono;

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
};

vector<vector<int>> getCellPositions(const vector<int>& core, Placer& p) {
    vector<vector<int>> cellPositions(p.numOfComponents, vector<int>(2, 0));

    for (int i = 0; i < p.nx; i++) {
        for (int j = 0; j < p.ny; j++) {
            if (core[i * p.ny + j] != EMPTY_CELL) {
                int component = core[i * p.ny + j];
                cellPositions[component][0] = i;
                cellPositions[component][1] = j;
            }
        }
    }

    return cellPositions;
}

int hpwl(const vector<int>& core, Placer& p) {
    // auto start = high_resolution_clock::now();

    vector<vector<int>> cellPositions = getCellPositions(core, p);
    int hpwl = 0;

    for (const auto& net : p.nets) {
        int maxX = -1, maxY = -1, minX = 9999999, minY = 9999999;

        for (int comp : net) {
            int currentX = cellPositions[comp][0];
            int currentY = cellPositions[comp][1];

            minX = min(minX, currentX);
            maxX = max(maxX, currentX);
            minY = min(minY, currentY);
            maxY = max(maxY, currentY);
        }

        hpwl += (maxX - minX) + (maxY - minY);
    }

    // auto stop = high_resolution_clock::now();
    // auto duration = duration_cast<microseconds>(stop - start);
    // cout << "hpwl Execution Time: " << duration.count() << " microseconds" << endl;

    return hpwl;
}

vector<int> makeCore(Placer& p) {
    auto start = high_resolution_clock::now();

    vector<int> core(p.nx * p.ny, EMPTY_CELL);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "makeCore Execution Time: " << duration.count() << " microseconds" << endl;

    return core;
}

void placeRandomly(vector<int>& core, Placer& p) {
    auto start = high_resolution_clock::now();

    for (int i = 0; i < p.numOfComponents; i++) {
        int pos = rand() % (p.nx * p.ny);
        while (core[pos] != EMPTY_CELL) {
            pos = rand() % (p.nx * p.ny);
        }
        core[pos] = i;
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "placeRandomly Execution Time: " << duration.count() << " microseconds" << endl;
}

void printToConsole(const vector<int>& core, Placer& p) {
    auto start = high_resolution_clock::now();
    const int cellWidth = 5;

    for (int i = 0; i < p.nx * p.ny; i++) {
        if (core[i] == EMPTY_CELL) {
            cout << setw(cellWidth) << left << "--";
        } else {
            cout << setw(cellWidth) << left << core[i];
        }

        // Print a newline after every p.ny elements
        if ((i + 1) % p.ny == 0) {
            cout << endl;
        }
    }

    cout << "HPWL: " << hpwl(core, p) << endl;

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "printToConsole Execution Time: " << duration.count() << " microseconds" << endl;
}

Placer makePlacer(string inputFile) {
    auto start = high_resolution_clock::now();
    ifstream file(inputFile);

    if (!file.is_open()) {
        cerr << "Error: Could not open file " << inputFile << endl;
        exit(EXIT_FAILURE);
    }

    Placer p;
    file >> p.numOfComponents >> p.numOfNets >> p.nx >> p.ny;

    p.nets.resize(p.numOfNets);

    for (int i = 0; i < p.numOfNets; i++) {
        int netComponents;
        file >> netComponents;

        p.nets[i].resize(netComponents);

        for (int k = 0; k < netComponents; k++) {
            file >> p.nets[i][k];
        }
    }

    p.movesPerTemp = MOVES * p.numOfComponents;

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "makePlacer Execution Time: " << duration.count() << " microseconds" << endl;

    return p;
}

void swap(int x1, int y1, int x2, int y2, vector<int>& core, Placer& p) {
    int temp = core[x1 * p.ny + y1];
    core[x1 * p.ny + y1] = core[x2 * p.ny + y2];
    core[x2 * p.ny + y2] = temp;
}

void simulatedAnnealing(Placer& p, vector<int>& core) {
    auto start = high_resolution_clock::now();

    int initialCost = hpwl(core, p);
    p.initialTemp = INIT_TEMP * initialCost;
    p.finalTemp = FINAL_TEMP * ((double)initialCost / p.numOfNets);
    double temp = p.initialTemp;

    while (temp > p.finalTemp) {
        for (int i = 0; i < p.movesPerTemp; i++) {
            int costI = hpwl(core, p);
            int oldPos = rand() % (p.nx * p.ny);
            int newPos = rand() % (p.nx * p.ny);

            swap(oldPos / p.ny, oldPos % p.ny, newPos / p.ny, newPos % p.ny, core, p);

            int costF = hpwl(core, p);
            int deltaCost = costF - costI;

            if (deltaCost > 0 && (rand() / (RAND_MAX + 1.0)) >= exp(-deltaCost / temp)) {
                // Revert the move
                swap(oldPos / p.ny, oldPos % p.ny, newPos / p.ny, newPos % p.ny, core, p);
            }
        }

        printToConsole(core, p);

        if (hpwl(core, p) == 0) {
            break;
        }
        temp = COOLING_RATE * temp;
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "simulatedAnnealing Execution Time: " << duration.count() << " microseconds" << endl;
}

int main() {
    auto start = high_resolution_clock::now();

    srand(time(NULL));

    Placer p = makePlacer("d0.txt");
    vector<int> core = makeCore(p);

    placeRandomly(core, p);
    printToConsole(core, p);

    simulatedAnnealing(p, core);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "main Execution Time: " << duration.count() << " microseconds" << endl;

    return 0;
}
*/
