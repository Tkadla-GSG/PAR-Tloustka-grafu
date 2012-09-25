/* 
 * File:   main.cpp
 * Author: Tkadla
 *
 * Created on 19. září 2012, 13:36
 */

#include <cstdlib>
#include <iostream> 
#include <fstream>
#include <string>

using namespace std;

/**
 * Trida fakticky predstavujici uzel vyhledavaciho stromu
 */
class Permutace {
public:

    /**
     * Konstructor
     * @param permutation - int [] pole s permutaci cisel uzlu
     * @param nodeCount - int pocet uzlu 
     * @param edgeTable - int[][] 2d tabulka prechodu z uzlu do uzlu
     */
    Permutace(int * permutation, int nodeCount, int ** edgeTable) {
        this->maxTLG = 0;

        this->nodeCount = nodeCount;
        this->edgeTable = edgeTable;
        this->permutation = permutation;
    }

    /**
     * Destruktor
     */
    ~Permutace() {
        delete [] this->permutation;
        this->permutation = NULL;

        for (int i = 0; i < nodeCount; i++) {
            delete [] edgeTable[i];
        }
        delete [] edgeTable;
        this->edgeTable = NULL;
    }

private:

    int * permutation;
    int ** edgeTable;
    int nodeCount;

    // maximalni TLG nalezene v permutaci
    int maxTLG;

    /**
     * Vraci bool hodnotu zda existuje hrana (prechod) mezi uzly
     * @param node1 - int cislo uzlu 1
     * @param node2 - int cislo uzlu 2
     * @return bool 
     */
    bool hasEdge(int node1, int node2) {
        return edgeTable[node1][node2];
    }

public:

    /**
     * Provadi vypocet hodnoty TLG pro tuto permutaci
     * @return int hodnota maximalni TLG
     */
    int getTLG() {

        // pro vsechny diry
        for (int i = 0; i < (nodeCount - 1); i++) {
            // pro vsechny uzly pred touto dirou
            int TLG = 0;
            for (int j = 0; j <= i; j++) {
                // projdi vsechny uzly za dirou
                for (int k = i + 1; k < nodeCount; k++) {
                    // a existuje-li mezi nimi hrana spocitej je
                    if (hasEdge(j, k)) {
                        TLG++;
                    };
                }

            }

            if (TLG > maxTLG) {
                maxTLG = TLG;
            }

        }

        return maxTLG;
    }
};

int main(int argc, char** argv) {

    // DEBUG nahrani tabulky ze souboru
    // Nahrani souboru 
    ifstream file;
    string fileName = "vstup.txt";
    file.open(fileName.c_str());
    if (!file) {
        cout << "Error in openening file";
        return EXIT_FAILURE;
    }

    // Prvni radek je pocet uzlu 
    string line;
    getline(file, line);
    int nodeCount = atoi(line.c_str());

    // Zbytek souboru po radkach prevest do int[][] pole
    int ** edgeTable = new int * [nodeCount];
    for (int j = 0; j < nodeCount; j++){
        getline(file, line); 
        edgeTable[j] = new int [nodeCount];
        for (int i = 0; i < nodeCount; i++) {
            char ch = line.at(i);
            // ulozeni hrany do pole
            edgeTable[j][i] = atoi( &ch ); 
        }
    }
    
    for( int i = 0; i < nodeCount; i++){
        
        for (int j = 0; j < nodeCount; j++) {
            cout << edgeTable[i][j] ;
        }
        
        cout << endl;
    }

    // DEBUG bude nahrazeno generovanim prohledavaciho stromu
    //int nodeCount = 4;
    int * permutation = new int [nodeCount];
    permutation[0] = 1;
    permutation[1] = 0;
    permutation[2] = 3;
    permutation[3] = 4;

    Permutace * permutace = new Permutace(permutation, nodeCount, edgeTable);

    cout << "TLG: " << permutace->getTLG() << endl;

    return 0;
}


