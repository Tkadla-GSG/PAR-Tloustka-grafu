/* 
 * File:   main.cpp
 * Author: Tkadla
 *
 * Created on 19. září 2012, 13:36
 */

#include <cstdlib>
#include <iostream> 
#include <fstream>
#include <stack>
#include <string>
#include <math.h>

using namespace std;

#define DEBUG 1

/**
 * Trida fakticky predstavujici uzel vyhledavaciho stromu
 */
class Permutation {
public:

    /**
     * Konstructor
     * @param permutation - int [] pole s permutaci cisel uzlu
     * @param length - int delka pole permutace
     * @param edgeTable - int[][] 2d tabulka prechodu z uzlu do uzlu
     * @param expanable - bool ma-li stav generovat dalsi potomky
     */
    Permutation(int * permutation, int length, int level, int ** edgeTable, bool expandable) {
        this->maxTLG = 0;
        this->expandable = expandable;
        this->length = length;
        this->level = level;
        this->edgeTable = edgeTable;
        this->permutation = permutation;
    }

    /**
     * Destruktor
     */
    ~Permutation() {
        delete [] permutation;
        permutation = NULL;
    }

private:

    int * permutation;
    int ** edgeTable;
    int maxTLG;
    int length;
    int level;
    bool expandable;

public:

    /**
     * Provadi vypocet hodnoty TLG pro tuto permutaci
     * @return int hodnota maximalni TLG
     */
    int getTLG() {
        // pro vsechny diry      
        for (int i = 0; i < (length - 1); i++) {
            // pro vsechny uzly pred touto dirou
            int TLG = 0;
            for (int j = 0; j <= i; j++) {
                // projdi vsechny uzly za dirou
                for (int k = i + 1; k < length; k++) {
                    // a existuje-li mezi nimi hrana spocitej je   
                    TLG += edgeTable[permutation[j]][permutation[k]];
                }
            }

            if (TLG > maxTLG) {
                maxTLG = TLG;
            }
        }
        return maxTLG;
    }

    /**
     * Naplni hlavni stack potomky tohoto stavu
     * @return 
     */
    void getChildren(stack < Permutation * > & mainStack) {

        if (!expandable) {
            return;
        }

        bool expand = true;
        for (int i = level; i < length; i++) {

            // permutuj vsechny cisla po indexu
            for (int j = i + 1; j < length; j++) {

                int * newPermutation = new int[length];

                // okopiruj stavajici permutaci do nove
                for (int k = 0; k < length; k++) {
                    newPermutation[k] = permutation[k];
                }

                /* vytvor novou permutaci
                 * urcita cast permutace je fixni (proto i = level)
                 * pak se prohazuje postupne kazdy prvek za fixni
                 * casti se zbytkem permutace
                 */
                newPermutation[i] = permutation[j];
                newPermutation[j] = permutation[i];

                // stavy v listech jiz neni potreba expandovat
                if ((level + 1) == length) {
                    expand = false;
                }
//              TODO comment a implementovat
//              if(leve+pocetCyklu == length){ expand = false}  

                Permutation * p = new Permutation(newPermutation, length, level + 1, edgeTable, expand);

                mainStack.push(p);

                if (DEBUG) {
                    p->toString();
                }
            }
        }
    }

    //DEBUG

    void toString() {
        cout << " node  | ";

        for (int i = 0; i < length; i++) {
            cout << permutation[i] << " | ";
        }
        cout << "level: " << level << " ";
        cout << "tlg: " << getTLG();
        cout << endl;
    }
};

int main(int argc, char** argv) {

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
    int length = atoi(line.c_str());

    double degree = 0; 

    // Zbytek souboru po radkach prevest do int[][] pole
    int ** edgeTable = new int * [length];
    int edge = 0; 
    for (int j = 0; j < length; j++) {
        
        int nodeDegree = 0; 
        getline(file, line);
        edgeTable[j] = new int [length];
        for (int i = 0; i < length; i++) {
            char ch = line.at(i);
            
            edge = atoi(&ch);
            nodeDegree += edge; 
             // ulozeni hrany do pole
            edgeTable[j][i] = edge;
        }
        
        if( nodeDegree > degree ){
            degree = nodeDegree; 
        }
    }
    
//    Prekontrolovat
    // Triviální spodní mez: polovina maximálního stupně grafu (zaokrouhlená nahoru).
    double threshold = ceil( ( degree/2 ) );

    // Vysledek ( nekonecno, nebo nejblizsi nejvetsi vec )
    int minTLG = numeric_limits<int>::max();
    // Stavovy zasobnik
    stack < Permutation * > mainStack;

    // Inicializace pocatecniho stavu
    int * permutation = new int [length];
    for (int i = 0; i < length; i++) {
        permutation[i] = i;
    }

    Permutation * permutace = new Permutation(permutation, length, 0, edgeTable, true);
    mainStack.push(permutace);

    int tlg;
    Permutation * state;
    while (!mainStack.empty()) {

        if (DEBUG) {
            cout << "stack size " << mainStack.size() << endl;
        }
        state = mainStack.top();
        mainStack.pop();

        tlg = state->getTLG();

        if (tlg < minTLG) {
            minTLG = tlg;
        }

        // Dosazena spodni mez, konec algoritmu. Vysledek je jiz ulozen v minTLG.
        if (minTLG <= threshold) {
            break;
        }

        // expanze do hlavniho zasobniku
        state->getChildren(mainStack);

        //stav uz neni a nebude potreba 
        delete state;
    }

    if (DEBUG) {
        cout << " ============= " << endl;
        cout << " min TLG: " << minTLG << endl;
    }

    // odstran ze zasobniku stavy, ktere tam potencialne zustali
    Permutation * toDelete; 
    while(!mainStack.empty()){
        toDelete = mainStack.top();
        mainStack.pop();
        delete toDelete; 
    }

    // uklid
    for (int i = 0; i < length; i++) {
        delete [] edgeTable[i];
    }
    delete [] edgeTable;
    edgeTable = NULL;

    return 0;
}


