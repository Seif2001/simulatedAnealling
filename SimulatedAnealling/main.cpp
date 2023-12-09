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
    vector<int> nets;
};

struct Placer {
    int numOfComponents, numOfNets, ny, nx;
    double initialTemp;
    double finalTemp;
    int movesPerTemp;
    vector<vector<int>> nets;
    vector<Cell> cellPositions;
    vector<int> maxX;
    vector<int> maxY;
    vector<int> minX;
    vector<int> minY;
};


std::string formatNets(const std::vector<int>& nets) {
    // Format vector of nets as a comma-separated string
    std::string result = " ";
    for (size_t i = 0; i < nets.size(); ++i) {
        result = result +  std::to_string(nets[i]);
        if (i < nets.size() - 1) {
            result += ", ";
        }
    }
    cout << endl << result << endl;

    return result;
}


void printTable(const std::vector<std::vector<Cell*>>& netCellPosition) {
    // Print table header
    printf("%-10s%-10s%-10s%-15s\n", "Index", "Value", "PosX", "Nets");

    // Iterate through the vector of vectors
    for (size_t i = 0; i < netCellPosition.size(); ++i) {
        const auto& cellVector = netCellPosition[i];

        // Print data in a table
        for (const auto& cell : cellVector) {
            printf("%-10zu%-10d%-10d%-15s%\n", i, cell->value, cell->posX, formatNets(cell->nets));
        }
    }
}




int hpwl(Placer& p) {

    int hpwl = 0;
    for (int i = 0; i < p.nets.size(); i++) {
        int maxX = -1;
        int maxY = -1;
        int minX = 9999999;
        int minY = 9999999;
        for (int j = 0; j < p.nets[i].size(); j++) {
            int currentX = (p.cellPositions[p.nets[i][j]]).posX;
            int currentY = (p.cellPositions[p.nets[i][j]]).posY;
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
        p.maxX.push_back(maxX);
        p.minX.push_back(minX);
        p.maxY.push_back(maxY);
        p.minY.push_back(minY);
        hpwl += (maxX - minX);
        hpwl += (maxY - minY);
    }
    return hpwl;
}

int updateHpwl(int i, int j, Placer& p) {
    int hp = 0;
    Cell x = p.cellPositions[i];
    Cell y = p.cellPositions[j];
    for (int net : x.nets) {
        if (p.maxX[net] < x.posX) {
            p.maxX[net] = x.posX;
        }

        if (p.maxY[net] < x.posY) {
            p.maxY[net] = x.posY;
        }

        if (p.minX[net] > x.posX) {
            p.minX[net] = x.posX;
        }

        if (p.minY[net] > x.posY) {
            p.minY[net] = x.posY;
        }
   }

    for (int net : y.nets) {
        if (p.maxX[net] < y.posX) {
            p.maxX[net] = y.posX;
        }

        if (p.maxY[net] < y.posY) {
            p.maxY[net] = y.posY;
        }

        if (p.minX[net] > y.posX) {
            p.minX[net] = y.posX;
        }

        if (p.minY[net] > y.posY) {
            p.minY[net] = y.posY;
        }
    }

    for (int i = 0; i < p.numOfNets; i++) {
        hp += (p.maxX[i] - p.minX[i]);
        hp += (p.maxY[i] - p.minY[i]);
    }
    cout << "HP: " << hp << "HPWL: " << hpwl(p) << endl;
    return hp;
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
                p.cellPositions[i].posX = row;
                p.cellPositions[i].posY = col;
                p.cellPositions[i].value = i;
                inserted.push_back({ row, col });
                
                placed = true;
            }
            row = rand() % p.nx;
            col = rand() % p.ny;

        }


    }

    for (int i = 0; i < p.nets.size(); i++) {
        for (int j = 0; j < p.nets[i].size(); j++) {
            p.cellPositions[p.nets[i][j]].nets.push_back(i);
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
            arr2d[(p.cellPositions[i]).posX][(p.cellPositions[i]).posY] = i;
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

void swap(int x, int y,  Placer& p) {
    int tempx = p.cellPositions[x].posX;
    int tempy = p.cellPositions[x].posY;
    p.cellPositions[x].posX = p.cellPositions[y].posX;
    p.cellPositions[x].posY = p.cellPositions[y].posY;
    p.cellPositions[y].posX = tempx;
    p.cellPositions[y].posY = tempy;
}



void simulatedAnealing(Placer& p) {
    int initialCost = hpwl(p);
    p.initialTemp = INIT_TEMP * initialCost;
    p.finalTemp = FINAL_TEMP * (static_cast<double>(initialCost) / p.numOfNets);
    double temp = p.initialTemp;
    const int cols = p.ny;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> disy(0, (p.ny) - 1);
    uniform_int_distribution<int> disx(0, (p.nx) - 1);
    uniform_int_distribution<int> disC(0, (p.nx*p.ny) - 1);
    uniform_real_distribution<double> dis2(0.0, 1.0);
    vector<Cell> oldVec(p.nx * p.ny);
    int swapx1, swapx2, swapy1, swapy2, costI, costF, deltaCost;

    int cell1, cell2;

    costI = initialCost;
    while (temp > p.finalTemp) {
        for (int i = 0; i < p.movesPerTemp; i++) {
            oldVec = p.cellPositions;
            

            cell1 = disC(gen);
            cell2 = disC(gen);

            swap(cell1, cell2, p);
            
            costF = updateHpwl(cell1, cell2, p);
            deltaCost = costF - costI;
            //cout << "DELTA: " << deltaCost << endl;
            if (deltaCost > 0 && (dis2(gen)) < (1 - exp(static_cast<double>(-deltaCost) / temp))) {
                p.cellPositions = oldVec;
            }
            else {
                costI = costF;
            }
        }

        printToConsole(p);

        temp = 0.95 * temp;
    }
}




int main() {
    srand(time(NULL));

    Placer p = makePlacer("C:\\Users\\elsha\\Desktop\\IC\\SimulatedAnealling\\d0.txt");
    makeCore(p);
    placeRandomly(p);
    printToConsole(p);
    simulatedAnealing(p);
    printToConsole(p);

    // todo: temp scheduling, sa algo
    return 0;
}