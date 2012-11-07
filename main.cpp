/* 
 * File:   main.cpp
 * Author: Tkadla
 *
 * Created on 19. září 2012, 13:36
 */
#include "mpi.h"
#include <windows.h>
#include <cstdlib>
#include <cstring> 
#include <iostream> 
#include <fstream>
#include <stack>
#include <string>
#include <math.h>
#include <limits>
#include <stdio.h>
#include <sstream>
#include <ctime>

using namespace std;

#define DEBUG 1

#define JOB_REQUEST     1000
#define HAS_JOB         1001
#define NO_JOB          1002
#define TOKEN           1003
#define FINISH          1004

#define CHECK_MSG       100
#define SLEEP           100

#define JOB_THRESHOLD   3

double threshold = 0;
ofstream logFile;

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

    int * getPermutation() {
        return permutation;
    }

    int getLevel() {
        return level;
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
        stringstream elements;
        //        cout << " node  | ";
        elements << " node  | ";

        for (int i = 0; i < length; i++) {
            //            cout << permutation[i] << " | ";
            elements << permutation[i] << " | ";
        }

        logString(elements.str());
        //        cout << "level: " << level << " ";
        //        cout << "tlg: " << getTLG();
        //        cout << endl;
        stringstream summary;
        summary << "level: " << level << " " << "tlg: " << getTLG();
        logString(summary.str());
    }

    /* Kopie metody z predka. Bohuzel se nemuzu na predka odkazat
     * jako je tomu mozne v Jave
     */
    void logString(string stringToLog) {
        // Current date/time based on current system
        time_t now = time(0);
        // Convert now to tm struct for local timezone
        tm* localtm = localtime(&now);
        /*    get actual time and save it to the
         * variable (returns string with \n at the end)
         */
        string timestamp = asctime(localtm);
        //    remove last "new line" char
        timestamp.erase(timestamp.length() - 1);
        //    Log to the file first timestamp (plus some space) and then string
        logFile << timestamp << "\t" << stringToLog << endl;
    };
};

/**
 * Using global variable logFile, enables to log any string
 * into the file, additionaly enriched by timestamp (for easier search)
 * 
 * AVAILABLE MODIFY:    Want flexible method to work for your output? Simple!
 *                      Just exchange variable logFile. For example for cout
 * 
 * @param stringToLog   represents string, that will be logged into the file
 */
void logString(string stringToLog) {
    // Current date/time based on current system
    time_t now = time(0);
    // Convert now to tm struct for local timezone
    tm* localtm = localtime(&now);
    /*    get actual time and save it to the
     * variable (returns string with \n at the end)
     */
    string timestamp = asctime(localtm);
    //    remove last "new line" char
    timestamp.erase(timestamp.length() - 1);
    //    Log to the file first timestamp (plus some space) and then string
    logFile << timestamp << "\t" << stringToLog << endl;
};

/**
 * Modifikuje (zkracuje,prodluzuje pole)
 * @param source - zdrojove pole
 * @param s_length - pocet prvku, ktere maji byt precteny ze zdrojoveho pole
 * @param d_length - delka finalniho pole
 * @return 
 */
int * modifyArray(int * source, int s_length, int d_length) {
    int * toReturn = new int[d_length];

    for (int i = 0; i < s_length; i++) {
        toReturn[i] = source[i];
    }

    return toReturn;
}

/**
 * Helper pro odesilani prace
 * @param toSend - pointer na odesilanou permutaci
 * @param length - delka pole permutace
 * @param target - cislo ciloveho procesu 
 */
void sendJob(Permutation * toSend, int length, int target) {
    //    cout << "sending job to " << target << endl;
    stringstream temp;
    temp << "sending job to " << target;
    logString(temp.str());

    int * data = modifyArray(toSend->getPermutation(), length, length + 1);
    data[length] = toSend->getLevel();
    MPI_Send(data, length + 1, MPI_INT, target, HAS_JOB, MPI_COMM_WORLD);

    delete toSend;
}

/**
 * Zpracovava obsah zasobniku
 * Vypocitava a expanduje stavy 
 * @param p
 * @param minTLG
 * @param edgeTable
 * @param mainStack
 * @return 
 */
int doWork(int rank, int p, int length, int minTLG, int ** edgeTable, stack < Permutation * > & mainStack) {

    int * data = new int[length + 1];
    MPI_Status status;

    int tlg;
    int flag;
    int citac = 0;
    bool suspended = false;
    int process_to_ask = 2; // cislo procesu, ktereho se budu ptat na potencialni praci
    int asked_for_job = 0; // kolikrat uz jsem pozadal o praci v jednom kuse
    Permutation * state;
    while (true) {
        citac++;

        if (!suspended) {
            if (mainStack.empty()) {
                // zeptej se na praci
                //                cout << "procesu " << rank << " dosla prace " << endl;

                stringstream temp;
                temp << "procesu " << rank << " dosla prace ";
                logString(temp.str());

                do {
                    process_to_ask = (process_to_ask + 1) % p;
                } while (process_to_ask == rank);

                //                cout << "proces si vyzadal novou praci od " << process_to_ask << endl;

                stringstream temp2;
                temp2 << "proces si vyzadal novou praci od " << process_to_ask;
                logString(temp2.str());

                MPI_Send(new int[1], 1, MPI_INT, process_to_ask, JOB_REQUEST, MPI_COMM_WORLD);
                suspended = true;

            } else {

                //ve stacku opravdu neco je a muzu provadet vypocty
                if (DEBUG) {
                    //                    cout << "stack size procesu " << rank << " | " << mainStack.size() << endl;

                    stringstream temp;
                    temp << "stack size procesu " << rank << " | " << mainStack.size();
                    logString(temp.str());
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
        } else {
            //stack empty, sleep thread for a while to conserve system resources
            //            cout << "suspending process " << rank << " for " << SLEEP << endl;

            stringstream temp;
            temp << "suspending process " << rank << " for " << SLEEP;
            logString(temp.str());

            Sleep(SLEEP);

            //check messages after suspention
            citac = CHECK_MSG;
        }

        if ((citac % CHECK_MSG) == 0) {
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
            if (flag) {

//                has to be initialised before switch. Used in logging.
                stringstream temp6;
                stringstream temp7;
                
                switch (status.MPI_TAG) {
                        //                    TENTO STAV UZ BY NASTAT NEMEL
                    case HAS_JOB:
                        // prisel rozdeleny zasobnik, prijmout
                        // deserializovat a spustit vypocet
                        //                        cout << "process prijal novou praci" << endl;
                        logString("process prijal novou praci");
                        asked_for_job = 0; // reset poctu dotazovani na novou praci, protoze jsem ji nakonec obdrzel

                        MPI_Recv(data, length + 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

                        state = new Permutation(modifyArray(data, length + 1, length), length, data[length], edgeTable, true);
                        mainStack.push(state);

                        suspended = false;

                        break;
                        //                        TAKY BYCH NEMEL DOSTAT
                    case NO_JOB:
                        // odmitnuti zadosti o praci
                        // zkusit jiny proces
                        // a nebo se prepnout do pasivniho stavu a cekat na token
                        //                        cout << "process " << rank << " obdrzel NO_JOB" << endl;

                        temp6 << "process " << rank << " obdrzel NO_JOB";
                        logString(temp6.str());

                        do {
                            process_to_ask = (process_to_ask + 1) % p;
                        } while (process_to_ask == rank);

                        MPI_Send(new int[1], 1, MPI_INT, process_to_ask, JOB_REQUEST, MPI_COMM_WORLD);
                        asked_for_job++;

                        if (asked_for_job == p) {
                            //uz jsem se zeptal vsech procesu a praci jsem nedostal
                            suspended = true;
                        }

                        break;
                        //                        ZDE BY BYLO DOBRE UMET ODHADNOUT KOLIK JESTE MAM STAVOVEHO PROSTORU
                        //                        ABYCHOM MOHLI ODMITAT
                    case JOB_REQUEST:
                        // zadost o praci, prijmout a dopovedet
                        // zaslat rozdeleny zasobnik a nebo odmitnuti MSG_WORK_NOWORK
                        //                        cout << "process " << rank << " prijal JOB_REQUEST" << endl;

                        temp7 << "process " << rank << " prijal JOB_REQUEST";
                        logString(temp7.str());

                        if (mainStack.size() > 1) { //mam dost prace pro dokonceni cyklu a zaroven odeslani nejake casti dat 

                            Permutation * toSend = mainStack.top();

                            //TODO vyladit pomer mezi 10 a JOB_THRESHOLD
                            if (mainStack.size() < 10) {
                                //prace v zasobniku je malo a nevyplati se ji preposilat

                                if (length - toSend->getLevel() > JOB_THRESHOLD) {
                                    // praci je jeste mozne expandovat
                                    toSend->getChildren(mainStack);

                                    //v zasobniku je nyni dost prace na jeji prerozdeleni
                                    mainStack.pop();
                                    //                                    cout << "process " << rank << " rozdelil zasobnik " << endl;

                                    stringstream temp3;
                                    temp3 << "process " << rank << " rozdelil zasobnik ";
                                    logString(temp3.str());

                                    sendJob(toSend, length, status.MPI_SOURCE);

                                } else {
                                    // prace v zasobniku se uz nevyplati expandovat
                                    cout << "process " << rank << " nema uz skoro praci a vraci NO_JOB procesu " << status.MPI_SOURCE << endl;

                                    stringstream temp4;
                                    temp4 << "process " << rank << " nema uz skoro praci a vraci NO_JOB procesu " << status.MPI_SOURCE;
                                    logString(temp4.str());

                                    MPI_Send(new int[1], 1, MPI_INT, status.MPI_SOURCE, NO_JOB, MPI_COMM_WORLD);
                                }

                            } else {
                                //v zasobniku je dost prace na jeji prerozdeleni 
                                mainStack.pop();
                                sendJob(toSend, length, status.MPI_SOURCE);

                            }

                        } else {
                            cout << "process " << rank << " nema uz zadnou praci a vraci NO_JOB procesu " << status.MPI_SOURCE << endl;

                            stringstream temp5;
                            temp5 << "process " << rank << " nema uz zadnou praci a vraci NO_JOB procesu " << status.MPI_SOURCE;
                            logString(temp5.str());

                            MPI_Send(new int[1], 1, MPI_INT, status.MPI_SOURCE, NO_JOB, MPI_COMM_WORLD);
                        }

                        break;
                        //                        JAKMILE JDE PRACE PRO PROCESOR PROTI CYKLU PROCESORU
                        //                        MUSIM SPUSTIT NOVOU TOKENOVOU SEKVENCI
                    case TOKEN:
                        //ukoncovaci token, prijmout a nasledne preposlat
                        // - bily nebo cerny v zavislosti na stavu procesu
                        break;
                        //MUSIM JAKO SLAVE POSLAT MASTRU VYSLEDEK HLEDALNI MINIMALNI TLOUSTKY
                        //POKUD SE UKONCUJE
                    case FINISH:
                        //konec vypoctu - proces 0 pomoci tokenu zjistil, ze jiz nikdo nema praci
                        //a rozeslal zpravu ukoncujici vypocet
                        //mam-li reseni, odeslu procesu 0
                        //nasledne ukoncim spoji cinnost
                        //jestlize se meri cas, nezapomen zavolat koncovou barieru MPI_Barrier (MPI_COMM_WORLD)
                        break;
                }
            }
        }
    }

    return minTLG;
}

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

        if (nodeDegree > degree) {
            degree = nodeDegree;
        }
    }

    // Triviální spodní mez: polovina maximálního stupně grafu (zaokrouhlená nahoru).
    threshold = ceil((degree / 2));

    // Vysledek ( nekonecno, nebo nejblizsi nejvetsi vec )
    double minTLG = numeric_limits<int>::max();

    // Stavovy zasobnik
    stack < Permutation * > mainStack;

    //init MPI
    MPI_Init(&argc, &argv);
    MPI_Status status;
    int rank = 0;
    int p = 0;

    //rank beziciho procesu
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //pocet procesoru 
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    /*---------------INICIALIZACE LOGOVANI-----------------*/
    //    Sestaveni nazvu LOG souboru pro jednotlive procesory
    stringstream logName;
    logName << ".\\logs\\procesor" << rank << ".log";

    /* Nechapu plne "jemne" finesy C++ ale logfile.open(logName.str().c_str())
     * proste nefunguje. Zrejme to bude z duvodu pointeru.
     */
    string logFileName = logName.str();

    /* PRETYPOVANI: Pro otevreni souboru je nutny format const char *
     * ios_base::app je zde proto, aby se novy log pridal na konec souboru
     * a neprepsal stavajici logy
     */
    logFile.open(logFileName.c_str(), ios_base::app);
    logFile << "\n\n------------NEW SESSION------------\n\n";

    /*---------------INICIALIZACE LOGOVANI-----------------*/


    //    -------------------WILL BE REMOVED-------------
//    logFile.close();
//    //MPI end
//    MPI_Finalize();
//
//    // uklid
//    for (int i = 0; i < length; i++) {
//        delete [] edgeTable[i];
//    }
//    delete [] edgeTable;
//    edgeTable = NULL;
//
//    return 0;
    //    -------------------WILL BE REMOVED-------------

    if (rank == 0) {
        //MASTER

        // Inicializace pocatecniho stavu
        int * permutation = new int [length];
        for (int i = 0; i < length; i++) {
            permutation[i] = i;
        }

        Permutation * permutace = new Permutation(permutation, length, 0, edgeTable, true);
        mainStack.push(permutace);

        //nageneruj potomky, ktere budou rozeslany slave procesum
        Permutation * parent = NULL;
        while (mainStack.size() < p) {
            int size = mainStack.size();
            parent = mainStack.top();
            parent->getChildren(mainStack);

            //            Mezitim si nageneruju childy?
            if (size == mainStack.size()) {
                //TODO mizerne misto, ve stacku mohou byt jinne expandovatelne stavy
                //stav nenageneroval zadne potomky, pravdepodobne doslo k chybe, nebo je zadany problem smesne maly (tj. nepodarilo se nagenerovat ani tolik stavu jako procesoru)
                break;
            }
        }

        //odesli slave procesum jejich uvodni stavy
        Permutation * toSend = NULL;
        for (int target = 1; target < p; target++) {
            toSend = mainStack.top();
            mainStack.pop();

            sendJob(toSend, length, target);

        }

        minTLG = doWork(rank, p, length, minTLG, edgeTable, mainStack);

        //TODO prijmout vysledky vsech ostatnich procesu

        //        cout << "waiting for other process to send me their results" << endl;
        logString("waiting for other process to send me their results");

        /* int result = 0;
         for (int i = 0; i < p; i++) {
             MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
         }*/

        if (DEBUG) {
            //            cout << " ============= " << endl;
            //            cout << " min TLG: " << minTLG << endl;

            logString(" ============= ");
            logString(" min TLG: ");
        }

    } else {
        //SLAVE

        //prvni zacatecni dotaz na job musi by blokujici, jinak by process nemel na cem pracovat 
        int * data = new int[length + 1];

        MPI_Recv(data, length + 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        //        cout << " process " << rank << " obdrzel praci " << endl;
        stringstream temp;
        temp << " process " << rank << " obdrzel praci ";
        logString(temp.str());

        Permutation * state = new Permutation(modifyArray(data, length, length), length, data[length], edgeTable, true);
        mainStack.push(state);

        minTLG = doWork(rank, p, length, minTLG, edgeTable, mainStack);

        // TODO odesli vysledek
        //        cout << " process " << rank << " computed " << minTLG << endl;
        stringstream temp2;
        temp2 << " process " << rank << " computed " << minTLG;
        logString(temp2.str());
    }

    //MPI end
    MPI_Finalize();

    // odstran ze zasobniku stavy, ktere tam potencialne zustali
    Permutation * toDelete;
    while (!mainStack.empty()) {
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

