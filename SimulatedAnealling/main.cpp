#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <random>
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
        cout <<"\n";
    }
    
    cout <<"\n";

    for (int i = 0; i < p.nx; i++) {
        for (int j = 0; j < p.ny; j++) {
            if (core[i][j] == EMPTY_CELL) {
                cout << 1;
            }
            else {
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
    //cout << "CELL A " << cellA << "\n";
    //cout << "CELL B " << cellB< "\n";
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

    //////////////////
    vector<pair<int, float>> temp_VS_len;
    ///////////////////////////
    int initialCost = hpwl(core, p);
    p.initialTemp = INIT_TEMP * initialCost;
    p.finalTemp = FINAL_TEMP * ((double)initialCost / p.numOfNets);
    double temp = p.initialTemp;
    const int cols = p.ny;
    ///////////////////////////
    int costF=hpwl(core, p);
            temp_VS_len.push_back(make_pair(costF, temp));
//////////////////////////////////
    while (temp > p.finalTemp) {
        //////////////////////////////////////
        if(temp==p.initialTemp)
        {
            printToConsole(core, p);        
            cout<<"Temp: "<<temp<<"\n";
            cout << "HPWL: " << costF<< "\n\n";
        }
        //////////////////////////
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
            costF = hpwl(core, p);
            int deltaCost = costF - costI;  
            if (deltaCost > 0) {

                core = oldCore;

            }
            //cout << i << "\n";
            //&& (rand())< (1 - exp((double(-deltaCost)/temp)))
            //cout << "DELTAAAA: " << deltaCost << "\n";
        }
        

        if (hpwl(core, p) == 0) {
            break;
        }
        /////////
        temp = 0.95 * temp;

          if( temp<(p.finalTemp/0.95) && temp>p.finalTemp)
        {
            printToConsole(core, p);        
            cout<<"Temp: "<<temp<<"\n";
            cout << "HPWL: " << costF<< "\n";
        }
        
        temp_VS_len.push_back(make_pair(costF, temp));
////////////////////////        
    }
    ofstream outputFile1("Temp.txt");
    ofstream outputFile2("coolingRate.txt");

    /* For Printing Temp vs TWL */
    /* Note: Calc Temp then swap or swap then calc Temp? */
    outputFile1<<"Temp"<<"\t"<<"TWL"<<"\n";
    for (const auto& pair : temp_VS_len) {
        outputFile1 << pair.first << "\t\t" << pair.second  << "\n";
    }
    /* For Printing Temp vs COOLING_RATE */
    outputFile2<<"TWL"<<"\t\t"<<"coolRate"<<"\n";
    
      for (const auto& pair : temp_VS_len) {
        outputFile2 << pair.first <<"\t\t" << COOLING_RATE  << "\n";
    }
}



int main() {
    
    srand(42); 
    Placer p = makePlacer("d0.txt");
    int** core = makeCore(p);
    placeRandomly(core, p);
   // printToConsole(core, p);

    simulatedAnealing(p, core);

    // todo: temp scheduling, sa algo
    return 0;
}
