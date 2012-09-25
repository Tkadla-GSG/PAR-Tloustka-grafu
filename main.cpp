/* 
 * File:   main.cpp
 * Author: Tkadla
 *
 * Created on 19. září 2012, 13:36
 */

#include <cstdlib>
#include <iostream> 

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

    // DEBUG bude nahrazeno generovanim prohledavaciho stromu
    int nodeCount = 4;
    int * permutation = new int [nodeCount];
    permutation[0] = 1;
    permutation[1] = 0;
    permutation[2] = 3;
    permutation[3] = 4;
    
    // DEBUG bude nahrazeno rozparsovanou tabulkou z generatoru
    int ** edgeTable = new int * [nodeCount];
    edgeTable[0] = new int [nodeCount];
    edgeTable[1] = new int [nodeCount];
    edgeTable[2] = new int [nodeCount];
    edgeTable[3] = new int [nodeCount];

    edgeTable[0][0] = 0;
    edgeTable[0][1] = 1;
    edgeTable[0][2] = 1;
    edgeTable[0][3] = 0;

    edgeTable[1][0] = 1;
    edgeTable[1][1] = 0;
    edgeTable[1][2] = 0;
    edgeTable[1][3] = 1;

    edgeTable[2][0] = 1;
    edgeTable[2][1] = 0;
    edgeTable[2][2] = 0;
    edgeTable[2][3] = 0;

    edgeTable[3][0] = 0;
    edgeTable[3][1] = 1;
    edgeTable[3][2] = 0;
    edgeTable[3][3] = 0;

    Permutace * permutace = new Permutace(permutation, nodeCount, edgeTable);

    cout << "TLG: " << permutace->getTLG() << endl;

    return 0;
}


