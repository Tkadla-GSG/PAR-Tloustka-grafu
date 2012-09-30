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

using namespace std;

/**
 * Trida fakticky predstavujici uzel vyhledavaciho stromu
 */
class Permutation {
public:

    /**
     * Konstructor
     * @param parent - Permutation * ukazatel na rodice uzlu
     * @param permutation - int [] pole s permutaci cisel uzlu
     * @param nodeCount - int pocet uzlu 
     * @param edgeTable - int[][] 2d tabulka prechodu z uzlu do uzlu
     * @param expanable - bool ma-li stav generovat dalsi potomky
     */
    Permutation(Permutation * parent, int * permutation, int nodeCount, int level, int ** edgeTable, bool expandable) {
        this->maxTLG = 0;
        this->expandable = expandable;
        this->parent = parent;
        this->nodeCount = nodeCount;
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

        for (int i = 0; i < nodeCount; i++) {
            delete [] edgeTable[i];
        }
        delete [] edgeTable;
        edgeTable = NULL;
    }

    Permutation * parent;

private:

    int * permutation;
    int ** edgeTable;
    int maxTLG;
    int nodeCount;
    int level;
    bool expandable;

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
                    }
                }
            }

            if (TLG > maxTLG) {
                maxTLG = TLG;
            }
        }
        return maxTLG;
    }

    /**
     * Provede srovnani tohoto a dalsiho uzlu. 
     * Vraci true, pokud jsou uzly shodne. Vraci false, pokud nejsou stejne.
     * @param that Permutation * - ukazatel na srovnavany uzel
     * @return 
     */
    bool equals(Permutation * that) {
        if (that == NULL) return false;

        if (nodeCount != that->getNodeCount()) return false;

        int * thatPermutation = that->getPermutation();
        for (int i = 0; i < nodeCount; i++) {
            if (permutation[i] != thatPermutation[i]) return false;
        }

        return true;
    }

    /**
     * Generuje a vraci pole pointeru na deti teto permutace
     * @return 
     */
    stack< Permutation * > getChildren() {

        stack< Permutation * > stack;

        if (!expandable) {
            stack;
        }

        for (int i = level; i < nodeCount; i++) {

            int * newPermutation = new int[nodeCount];

            // okopiruj vsechny cisla pred indexem
            for (int j = 0; j < i; j++) {
                newPermutation[j] = permutation[j];
            }

            // permutuj vsechny cisla po indexu
            for (int j = i; i < nodeCount; j++) {
                newPermutation[i] = permutation[j];
                newPermutation[j] = permutation[i];

                Permutation * p = new Permutation(this, newPermutation, nodeCount, level + 1, edgeTable, false);

                stack.push(p);

                p->toString();
            }
        }

        return stack;
    }

    // Gettery

    bool isExpandable() const {
        return expandable;
    }

    int getNodeCount() const {
        return nodeCount;
    }

    int * getPermutation() const {
        return permutation;
    }

    int getLevel() const {
        return level;
    }

    //DEBUG

    void toString() {
        cout << " node  | ";

        for (int i = 0; i < this->nodeCount; i++) {
            cout << this->permutation[i] << " | ";
        }
        cout << "level: " << this->level;
        cout << "ltg: " << this->getTLG();
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
    int nodeCount = atoi(line.c_str());

    // Zbytek souboru po radkach prevest do int[][] pole
    int ** edgeTable = new int * [nodeCount];
    for (int j = 0; j < nodeCount; j++) {
        getline(file, line);
        edgeTable[j] = new int [nodeCount];
        for (int i = 0; i < nodeCount; i++) {
            char ch = line.at(i);
            // ulozeni hrany do pole
            edgeTable[j][i] = atoi(&ch);
        }
    }

    // Vysledek ( nekonecno, nebo nejblizsi nejvetsi vec )
    int minTLG = numeric_limits<int>::max();
    // Triviální spodní mez: polovina maximálního stupně grafu (zaokrouhlená nahoru).
    // TODO CHYBNY INIT PRO TESTOVANI
    int threshold = numeric_limits<int>::max();
    // Stavovy zasobnik
    stack < Permutation * > stack;

    // Inicializace pocatecniho stavu
    int * permutation = new int [nodeCount];
    for (int i = 0; i < nodeCount; i++) {
        permutation[i] = i + 1;
    }

    Permutation * permutace = new Permutation(NULL, permutation, nodeCount, 0, edgeTable, true);
    stack.push(permutace);

    //Main Loop
    stack < Permutation * > children;
    int tlg;
    int length;
    while (!stack.empty()) {

        Permutation * state = stack.top();
        stack.pop();

        tlg = state->getTLG();

        if (tlg < minTLG) {
            minTLG = tlg;
        }

        // Dosazena spodni mez, konec algoritmu. Vysledek je jiz ulozen v minTLG.
        if (minTLG <= threshold) {
            break;
        }

        /**
         * EXPANZE STAVU
         */
        children = state->getChildren();

        // Nejsou-li potomci, neexpanduj
        if (children.empty()) continue;

        // ulozeni vsech stavu do zasobniku
        length = sizeof ( children) / sizeof ( children[0]);
        for (int i = 0; i < length; i++) {
            stack.push(children[i]);
        }
    }

    cout << " ============= " << endl;
    cout << " min TLG: " << minTLG << endl;

    return 0;
}


