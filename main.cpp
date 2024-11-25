#include<iostream>
#include<fstream>
#include<filesystem>
#include<random>
#include<string.h>
#include<algorithm>

#include"correcteur.h"
#include"eccConvolutionalHash.h"

using namespace std;

uint32_t* getRandomData(int size);
char* getCmdOption(char** begin, char** end, const std::string& option);
bool cmdOptionExists(char** begin, char** end, const std::string& option);

bool bench();
vector<vector<int>> permuterDimensions(vector<int> dims);
bool ecrire(char* data, char* filename, size_t length);
bool afficherOptions();
bool creerClef(int argc, char** argv);
bool insererErreurs(uint64_t* data, int pourcentageBitsErreurs);

int main(int argc, char** argv) {

    if (cmdOptionExists(argv, argv + argc, "-h"))
    {
        return afficherOptions();
    }

    if (cmdOptionExists(argv, argv + argc, "-bench"))
    {
        return bench();
    }

    if (cmdOptionExists(argv, argv + argc, "-cc"))
    {
        return creerClef(argc, argv);
    }

    return 0;
}

bool creerClef(int argc, char** argv) {

    char* filenameClef = getCmdOption(argv, argv + argc, "-cc");

    cout << endl << "Cr�ation d'une clef dans le fichier: " << filenameClef << " !";

    cout << endl << "Entrer le nombres de dimensions (1-6, inclusivement): ";

    int nbrHash = 0;

    while (nbrHash < 1 || nbrHash > 6) {

        cin >> nbrHash;

        if (nbrHash < 1 || nbrHash > 6) cout << endl << "Le nombre de dimensions doit �tre entre 1-6, inclusivement ! Entrer: ";
    }

    cout << endl << "Merci ! Le nombre de dimensions et de hashs est de: " << nbrHash << " !" << endl << endl;

    vector<int> dimensions;

    for (int i = 0; i < nbrHash; i++) {

        cout << "Entrer la largeur de la dimension #" << i + 1 << " (1-10, inclusivement): ";

        int largeur = 0;

        while (largeur < 1 || largeur > 10) {

            cin >> largeur;

            if (largeur < 1 || largeur > 10) cout << endl << "La largeur de la dimension doit �tre entre 1-10, inclusivement ! Entrer: ";
        }

        dimensions.push_back(largeur);
    }

    cout << endl << endl << "Vos valeures seront permutt�s diff�rament pour chacun des hashs !";

    vector<vector<int>> hashs = permuterDimensions(dimensions);

    cout << endl << endl << "Entrer une valeure d'encryption maximale (1=non-encrypt� - 255=encrypt�(diminue le nombre de dimensions maximale)) suggestion(1): ";

    int encryption = 0;

    while (encryption < 1 || encryption > 255) {

        cin >> encryption;

        if (encryption < 1 || encryption > 255) cout << endl << "Le nombre d'encryptions doit �tre entre 1-255, inclusivement ! Entrer: ";
    }

    correcteur C;

    unsigned char* clef = C.getKey(hashs, encryption);

    bool success = clef != nullptr;

    if (success) {

        char* data;
        char tmpC = clef[0];
        int cntr = 0;

        while (tmpC != '\0') {

            cntr++;
            tmpC = clef[cntr];
        }

        data = new char[cntr + 1];

        for (int i = 0; i < cntr; i++) {

            data[i] = clef[i];
        }

        data[cntr + 1] = '\0';

        ecrire(data, filenameClef, cntr);

        cout << endl << "Clef cr��e avec succ�s !";
    }
    else cout << endl << "Clef impossible � cr�er !";

    cout << endl;

    return 0;
}

bool afficherOptions() {

    cout << endl << "Options:";
    cout << endl << "\t" << "-c: Appliquer une convolution N-D !";
    cout << endl << "\t" << "-d: De-appliquer une convolution N-D !";
    cout << endl << "\t" << "-clef: Utiliser ce fichier pour la clef !";
    cout << endl << "\t" << "-e: Fichier d'entr� !";
    cout << endl << "\t" << "-s: Fichier de sortie !";
    cout << endl << "\t" << "-bench: Ne pas tenir compte des autres args et executer le bench-test !";
    cout << endl << "\t" << "-cc: Ne pas tenir compte des autres args et cr�er une clef dans ce fichier !";
    cout << endl << "\t" << "-h: Afficher l'aide !";
    cout << endl;

    return 0;
}

bool insererErreurs(uint64_t* data, int pourcentageBitsErreurs, int dataLength) {

    for(int posData; posData < dataLength; posData++) {

        for (int pos64 = 0; pos64 < 64; pos64++) {

            int pos2 = pos64 % 2;

            int pos = pos64 % 32;

            uint32_t tmpL = 0L;

            tmpL = UINT32_MAX;

            tmpL >>= 31;
            tmpL <<= 31;
            tmpL >>= (32 - pos);

            if (pos2 == 0) {

                uint32_t tmpLL = (uint32_t)((data[posData] << 32) >> 32);

                if ((tmpLL << pos) >> 31 == 0) {

                    tmpLL |= tmpL;
                }
                else {

                    tmpLL &= (tmpL ^ UINT32_MAX);
                }

                data[posData] >>= 32;
                data[posData] <<= 32;

                data[posData] |= tmpLL;
            }
            else {

                uint32_t tmpLL = (uint32_t)(data[posData] >> 32);

                if ((tmpLL << pos) >> 31 == 0) {

                    tmpLL |= tmpL;
                }
                else {

                    tmpLL &= (tmpL ^ UINT32_MAX);
                }

                data[posData] <<= 32;
                data[posData] >>= 32;

                uint64_t tmpKJH = tmpLL;

                data[posData] |= (tmpKJH << 32);
            }
        }
    }
    
    return true;
}

bool bench() {

    cout << endl << "bench-test pas fonctionel!";

    cout << endl;

    /*
    vector<int> dims;

    dims.push_back(2);
    dims.push_back(1);
    dims.push_back(1);
    dims.push_back(1);

    vector<vector<int>> hashs = permuterDimensions(dims);

    int dataSize = 1000000;

    uint32_t* data = getRandomData(dataSize);

    correcteur C;

    unsigned char* clef = C.getKey(hashs, 1);

    for (int i = 0; i < 100; i++) {

        auto start2 = std::chrono::high_resolution_clock::now();

        for (int j = 0; j < dataSize / 16; j++) {

            uint32_t* chunkData = new uint32_t[16];

            //int* finaleData = C.encrypting(chunckData, clef, 16);
        }

        auto end2 = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> duration2 = end2 - start2;

        //insererErreurs(dataC)
    }

    //if (success) cout << endl << "Bench-test execut� avec succ�s !";
    //else cout << endl << "Bench-test a �chou� !";

    cout << endl;*/

    return 0;
}

bool ecrire(char* data, char* filename, size_t length) {

    ofstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Unable to open file";
        return false;
    }

    file.write(data, length);

    file.close();

    return true;
}

vector<vector<int>> permuterDimensions(vector<int> dims) {

    vector<vector<int>> dimensions;

    int hauteDim = 0;

    for (int i = 0; i < dims.size(); i++) {

        if (dims[i] > hauteDim) hauteDim = dims[i];
    }

    for (int i = 0; i < dims.size(); i++) {

        vector<int> tmp;
        dimensions.push_back(tmp);
    }

    for (int i = 0; i < dims.size(); i++) {

        vector<int> tmpV = dims;

        for (int j = 0; j < tmpV.size(); j++) {

            if (tmpV[j] == hauteDim) {

                tmpV.erase(tmpV.begin() + j);
                j--;
            }
        }

        for (int j = 0; j < dims.size(); j++) {

            if (i == j) dimensions[i].push_back(hauteDim);
            else {

                int pos = rand() % tmpV.size();
                dimensions[i].push_back(tmpV[pos]);
                tmpV.erase(tmpV.begin() + pos);
            }
        }
    }

    return dimensions;
}

char* getCmdOption(char** begin, char** end, const string& option)
{
    char** itr = find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const string& option)
{
    return find(begin, end, option) != end;
}

uint32_t* getRandomData(int size) {

    uint32_t* data = new uint32_t[size];

    for (int i = 0; i < size; i++) {

        data[i] = i % 256;
    }

    return data;
}