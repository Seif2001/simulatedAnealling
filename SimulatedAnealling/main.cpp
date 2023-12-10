#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <random>
#include <string>
#include <time.h>
#include <tuple>
#include <utility>
#include <vector>

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
  vector<vector<int>> netX;
  vector<vector<int>> netY;
};

string formatNets(const vector<int> &nets) {
  // Format vector of nets as a comma-separated string
  string result = " ";
  for (size_t i = 0; i < nets.size(); ++i) {
    result = result + to_string(nets[i]);
    if (i < nets.size() - 1) {
      result += ", ";
    }
  }
  cout << "\n" << result << "\n";

  return result;
}

void printTable(const vector<vector<Cell *>> &netCellPosition) {
  // Print table header
  printf("%-10s%-10s%-10s%-15s\n", "Index", "Value", "PosX", "Nets");

  // Iterate through the vector of vectors
  for (size_t i = 0; i < netCellPosition.size(); ++i) {
    const auto &cellVector = netCellPosition[i];

    // Print data in a table
    for (const auto &cell : cellVector) {
      printf("%-10zu%-10d%-10d%-15s%%\n", i, cell->value, cell->posX,
             formatNets(cell->nets).c_str());
    }
  }
}

int hpwl(Placer &p) {

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
    p.maxX[i] = maxX;
    p.minX[i] = minX;
    p.maxY[i] = maxY;
    p.minY[i] = minY;
    hpwl += (maxX - minX);
    hpwl += (maxY - minY);
  }
  return hpwl;
}
int getHpwl(Placer &p) {
  int hp = 0;
  for (int i = 0; i < p.numOfNets; i++) {
    hp += (p.maxX[i] - p.minX[i]);
    hp += (p.maxY[i] - p.minY[i]);
  }
  return hp;
}

int updateHpwl(int hpwl, int f, int k, Placer &p) {
  Cell x = p.cellPositions[f];
  Cell y = p.cellPositions[k];
  for (int net : x.nets) {
    hpwl -= p.maxX[net];
    hpwl -= p.maxY[net];
    hpwl += p.minY[net];
    hpwl += p.minX[net];

    int maxX = -1;
    int maxY = -1;
    int minX = 9999999;
    int minY = 9999999;
    for (int j = 0; j < p.nets[net].size(); j++) {
      int currentX = (p.cellPositions[p.nets[net][j]]).posX;
      int currentY = (p.cellPositions[p.nets[net][j]]).posY;
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
    p.maxX[net] = maxX;
    p.minX[net] = minX;
    p.maxY[net] = maxY;
    p.minY[net] = minY;

    hpwl += p.maxX[net];
    hpwl += p.maxY[net];
    hpwl -= p.minY[net];
    hpwl -= p.minX[net];
  }

  for (int net : y.nets) {
    int maxX = -1;
    int maxY = -1;
    int minX = 9999999;
    int minY = 9999999;
    hpwl -= p.maxX[net];
    hpwl -= p.maxY[net];
    hpwl += p.minY[net];
    hpwl += p.minX[net];
    for (int j = 0; j < p.nets[net].size(); j++) {
      int currentX = (p.cellPositions[p.nets[net][j]]).posX;
      int currentY = (p.cellPositions[p.nets[net][j]]).posY;
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
    p.maxX[net] = maxX;
    p.minX[net] = minX;
    p.maxY[net] = maxY;
    p.minY[net] = minY;

    hpwl += p.maxX[net];
    hpwl += p.maxY[net];
    hpwl -= p.minY[net];
    hpwl -= p.minX[net];
  }

  return hpwl;
}
void makeCore(Placer &p) {
  p.cellPositions.resize(p.nx * p.ny);
  for (int i = 0; i < p.nx * p.ny; i++) {
    p.cellPositions[i].posX = 0;
    p.cellPositions[i].posY = 0;
  }
}

void placeRandomly(Placer &p) {
  vector<tuple<int, int>> inserted;
  for (int i = 0; i < p.cellPositions.size(); i++) {
    int row = rand() % p.nx;
    int col = rand() % p.ny;
    bool placed = false;

    while (!placed) {

      if (find(inserted.begin(), inserted.end(), make_tuple(row, col)) ==
          inserted.end()) {
        p.cellPositions[i].posX = row;
        p.cellPositions[i].posY = col;
        p.cellPositions[i].value = i;
        inserted.push_back({row, col});

        placed = true;
      }
      row = rand() % p.nx;
      col = rand() % p.ny;
    }
  }

  for (int i = 0; i < p.nets.size(); i++) {
    for (int j = 0; j < p.nets[i].size(); j++) {
      p.cellPositions[p.nets[i][j]].nets.push_back(i);
      // cout << p.cellPositions[p.nets[i][j]].value << " " << i;
    }
    // cout << "\n";
  }
}

void printToConsole(Placer &p) {
  int **arr2d = new int *[p.nx];
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
      } else {
        cout << setw(cellWidth) << left << arr2d[i][j];
      }
    }
    cout << "\n";
  }

  cout << "\n";

  for (int i = 0; i < p.nx; i++) {
    for (int j = 0; j < p.ny; j++) {
      if (arr2d[i][j] == EMPTY_CELL) {
        cout << 1;
      } else {
        cout << 0;
      }
    }
    cout << "\n";
  }

  cout << "\n";
}

Placer makePlacer(string inputFile) {
  ifstream file;
  Placer p;

  file.open(inputFile);
  if (file.is_open()) {        // always check whether the file is open
    file >> p.numOfComponents; // pipe file's content into stream
    file >> p.numOfNets;
    file >> p.nx;
    file >> p.ny;
    p.nets.resize(p.numOfNets);
    p.maxX.resize(p.numOfNets);
    p.maxY.resize(p.numOfNets);
    p.minX.resize(p.numOfNets);
    p.minY.resize(p.numOfNets);

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
  } else {
    cout << "NO OPEN FILE";
  }

  return p;
}

void swap(int x, int y, Placer &p) {
  int tempx = p.cellPositions[x].posX;
  int tempy = p.cellPositions[x].posY;
  p.cellPositions[x].posX = p.cellPositions[y].posX;
  p.cellPositions[x].posY = p.cellPositions[y].posY;
  p.cellPositions[y].posX = tempx;
  p.cellPositions[y].posY = tempy;
}

void simulatedAnealing(Placer &p) {
  int initialCost = getHpwl(p);
  p.initialTemp = INIT_TEMP * initialCost;
  p.finalTemp = FINAL_TEMP * (static_cast<double>(initialCost) / p.numOfNets);
  double temp = p.initialTemp;
  vector<pair<int, float>> temp_VS_len;

  random_device rd;
  mt19937 gen(rd());
  gen.seed(42);

  uniform_int_distribution<int> disy(0, (p.ny) - 1);
  uniform_int_distribution<int> disx(0, (p.nx) - 1);
  uniform_int_distribution<int> disC(0, (p.nx * p.ny) - 1);
  uniform_real_distribution<double> dis2(0.0, 1.0);
  vector<Cell> oldVec(p.nx * p.ny);
  int costI, costF, deltaCost;

  int cell1, cell2;

  costI = initialCost;
  costF = costI;
  vector<int> MaxXtemp(p.numOfNets);
  vector<int> MaxYtemp(p.numOfNets);
  vector<int> MinXtemp(p.numOfNets);
  vector<int> MinYtemp(p.numOfNets);
  
  temp_VS_len.push_back(make_pair(costF, temp));

  while (temp > p.finalTemp) {
    if (temp == p.initialTemp) {
      printToConsole(p);
      cout << "Temp: " << temp << "\n";
      cout << "HPWL: " << costI << "\n\n";
    }

    for (int i = 0; i < p.movesPerTemp; i++) {
      oldVec = p.cellPositions;

      cell1 = disC(gen);
      cell2 = disC(gen);
      MaxXtemp = p.maxX;
      MinXtemp = p.minX;
      MaxYtemp = p.maxY;
      MinYtemp = p.minY;
      swap(cell1, cell2, p);
      // cout << oldVec[cell1].posY << "        " << p.cellPositions[cell1].posY
      // << "\n";
      costF = updateHpwl(costI, cell1, cell2, p);
      deltaCost = costF - costI;
      // cout << "DELTA: " << deltaCost << "\n";
      //&& (dis2(gen)) < (1 - exp(static_cast<double>(-deltaCost) / temp))
      if (deltaCost > 0) {
        p.cellPositions = oldVec;
        p.maxX = MaxXtemp;
        p.minX = MinXtemp;
        p.maxY = MaxYtemp;
        p.minY = MinYtemp;
      } else {
        costI = costF;
      }
    }

    temp = COOLING_RATE * temp;

    if (temp < (p.finalTemp / 0.95) && temp > p.finalTemp) {
      printToConsole(p);
      cout << "Temp: " << temp << " " << p.finalTemp << "\n";
      cout << "HPWL: " << costF << "\n";
    }
    temp_VS_len.push_back(make_pair(costF, temp));
  }

  ofstream outputFile1("Temp.txt");
  ofstream outputFile2("coolingRate.txt");

  /* For Printing Temp vs TWL */
  /* Note: Calc Temp then swap or swap then calc Temp? */
  outputFile1 << "TWL"
              << "\t"
              << "Temp"
              << "\n";
  for (size_t i = 0; i < temp_VS_len.size() - 1; ++i) {
      outputFile1 << temp_VS_len[i].first << "\t" << temp_VS_len[i].second << "\n";
  }
  /* For Printing Temp vs COOLING_RATE */
  outputFile2 << "TWL"
              << "\t"
              << "coolRate"
              << "\n";

  for (const auto &pair : temp_VS_len) {
    outputFile2 << pair.first << "\t" << COOLING_RATE << "\n";
  }
}

int main() {

  auto start_time = chrono::high_resolution_clock::now();

  srand(42);

  Placer p = makePlacer("d0.txt");
  makeCore(p);
  placeRandomly(p);
  hpwl(p);
  simulatedAnealing(p);
  auto end_time = chrono::high_resolution_clock::now();

  // Calculate the duration
  auto duration =
      chrono::duration_cast<chrono::microseconds>(end_time - start_time);

  // Print the duration
  cout << "\nExecution Time: " << duration.count() << " microseconds"
       << "\n";

  return 0;
}
