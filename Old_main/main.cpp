/*#include<iostream>
#include<fstream>
#include<filesystem>
#include<random>

#include"correcteur.h"
#include <string.h>
#include "eccConvolutionalHash.h"

#pragma warning(disable : 4996)

using namespace std;

uint32_t* getRandomData(int size);

int main3(int argc, char** argv) {

    //std::cout << argv[1] << std::endl;

    if (strcmp(argv[1], "encrypt") == 0) {

        std::ifstream file(argv[2], std::ios::binary);

        if (!file) {
            std::cerr << "Unable to open file";
            return 1;
        }
        // Determine the file size
        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        // Allocate memory for the file content
        unsigned char* fff = new unsigned char[size];

        // Read the file content into the buffer
        if (file.read(reinterpret_cast<char*>(fff), size)) {
            // Successfully read the file
            for (std::streamsize i = 0; i < size; ++i) {
                std::cout << static_cast<int>(fff[i]) << " ";
            }
        }
        else {
            std::cerr << "Error reading file";
        }

        // Clean up

        file.close();


        std::ifstream file2(argv[3], std::ios::binary);

        if (!file2) {
            std::cerr << "Unable to open file";
            return 1;
        }
        // Determine the file size
        file2.seekg(0, std::ios::end);
        std::streamsize size2 = file2.tellg();
        file2.seekg(0, std::ios::beg);

        // Allocate memory for the file content
        unsigned char* clef = new unsigned char[size2];

        // Read the file content into the buffer
        if (file2.read(reinterpret_cast<char*>(clef), size2)) {
            // Successfully read the file
            for (std::streamsize i = 0; i < size2; ++i) {

                std::cout << static_cast<int>(clef[i]) << "-";
            }
        }
        else {
            std::cerr << "Error reading file";
        }

        // Clean up

        file2.close();


        correcteur E;

        uint64_t* rep = nullptr;// = E.encrypting(fff, clef, size);


        std::ofstream outputFile(argv[2], std::ios::binary);
        if (!outputFile) {
            std::cerr << "Error opening file for writing." << std::endl;
            return 1;
        }

        for (int i = 0; i < size; i++) {

            outputFile.write(reinterpret_cast<const char*>(&rep[i]), sizeof(rep[i]));
            std::cout << rep[i] << " -- ";
        }

        outputFile.close();

        delete[] fff;
        delete[] rep;
        delete[] clef;
    }
    else if (strcmp(argv[1], "key") == 0) {

        correcteur E;

        std::vector<int> tmpKeyA = { 3, 1, 2, 1 };

        std::vector<std::vector<int>> keyDim;

        keyDim.push_back(tmpKeyA);

        tmpKeyA = { 3, 1, 1, 2 };

        keyDim.push_back(tmpKeyA);

        int keySize = 0;

        unsigned char* key = E.getKey(keyDim, 4, keySize);

        std::ofstream outputFile("clef.txt", std::ios::binary);
        if (!outputFile) {
            std::cerr << "Error opening file for writing." << std::endl;
            return 1;
        }

        for (int i = 0; i < keySize; i++) {

            outputFile.write(reinterpret_cast<const char*>(&key[i]), sizeof(key[i]));
        }

        outputFile.close();
    }
    else if (strcmp(argv[1], "decrypt") == 0) {

        std::filesystem::path filePath2 = argv[2];

        uint64_t ll = std::filesystem::file_size(filePath2);

        uint64_t* ff = new uint64_t[ll / 8];


        std::ifstream file(argv[2], std::ios::binary);

        if (!file) {
            std::cerr << "Unable to open file";
            return 1;
        }
        // Determine the file size
        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        // Allocate memory for the file content
        //unsigned char* fff = new unsigned char[size];

        for (int i = 0; i < ll / 8; i++) {

            file.read(reinterpret_cast<char*>(&ff[i]), sizeof(ff[0]));
            //std::cout << "***" << ff[i];
        }
        // Clean up

        file.close();


        std::filesystem::path filePath = argv[3];

        uint64_t llll = std::filesystem::file_size(filePath);

        //unsigned char* clef = new unsigned char[llll];


        std::ifstream file2(argv[3], std::ios::binary);

        if (!file2) {
            std::cerr << "Unable to open file";
            return 1;
        }
        // Determine the file size
        file2.seekg(0, std::ios::end);
        size = file2.tellg();
        file2.seekg(0, std::ios::beg);

        // Allocate memory for the file content
        unsigned char* clef = new unsigned char[size];

        // Read the file content into the buffer
        if (file2.read(reinterpret_cast<char*>(clef), size)) {
            // Successfully read the file
            for (std::streamsize i = 0; i < size; ++i) {
                std::cout << static_cast<int>(clef[i]) << " ";
            }
        }
        else {
            std::cerr << "Error reading file";
        }

        // Clean up

        file2.close();


        correcteur E;

        //std::cout << (ff[0] << 48 >> 56) << " ? ? ? ";

        std::vector<int> tmptmp;

        int* final = E.decrypting(ff, ll / 8, clef, tmptmp);

        for (int i = 0; i < ll / 8; i++) std::cout << (int)final[i] << ":";

        FILE* MyFileSS = fopen(argv[2], "wb");

        fwrite(&final[0], sizeof(unsigned char), ll / 8, MyFileSS);

        fclose(MyFileSS);
    }
    else if (strcmp(argv[1], "bench") == 0) {

        for (int i = 4; i <= 4; i += 1) {

            int lengthData = pow(2, i);

            srand(time(0));

            uint32_t* data = getRandomData(lengthData);

            for (int oo = 4; oo <= 4; oo++) {

                for (int ttt = 2; ttt <= 2; ttt += 2) {

                    for (int ooo = oo; ooo <= oo; ooo++) {

                        std::vector<std::vector<int>> kkey;

                        for (int jj = 0; jj < oo; jj++) {

                            std::vector<int> tmpyyyy;
                            kkey.push_back(tmpyyyy);

                            for (int k = 0; k < ooo; k++) {

                                if (k == jj) kkey[jj].push_back(2);
                                else kkey[jj].push_back(1);
                            }
                        }

                        std::vector<double> tmpData;

                        correcteur E;
                        eccConvolutionalHash C;

                        int keySize = 1;

                        unsigned char* key = E.getKey(kkey, 4, keySize);

                        auto start = std::chrono::high_resolution_clock::now();

                        uint64_t* rep = E.encrypting(data, key, lengthData);

                        char* dataH = new char[lengthData + 1];

                        for (int hi = 0; hi < lengthData; hi++) {

                            char tmpIII = data[hi] + 1;

                            dataH[hi] = tmpIII;
                        }

                        dataH[lengthData] = '\0';

                        int* repHash = C.encode(dataH, lengthData);

                        //for (int y = 0; y < lengthData; y++) std::cout << rep[y] << " ";

                        auto end = std::chrono::high_resolution_clock::now();

                        std::chrono::duration<double> duration = end - start;

                        tmpData.push_back(duration.count());

                        std::ofstream outputFile("c:/a/results.txt", std::ios::binary);
                        if (!outputFile) {
                            std::cerr << "Error opening file for writing." << std::endl;
                            return 1;
                        }


                        for (int y = 9; y > 5; y -= 1) {

                            int failNbr = 0;
                            int failNbrH = 0;
                            int nbrErreursCreated = 0;
                            double totalTime = 0.0;

                            for (int uuy = 0; uuy < 10000; uuy++) {

                                bool test = true;
                                bool testH = true;
                                int ratio = y;

                                uint64_t* repp = new uint64_t[lengthData];
                                int* reppH = new int[(lengthData + 8)*13];

                                int nbrErreurs = 0;
                                int nbrErreursH = 0;

                                for (int j = 0; j < lengthData; j++) {

                                    repp[j] = rep[j];

                                    //std::cout << rep[j] << ":";

                                    test = true;

                                    int localErrors = 0;

                                    for (int u = 0; u < 64; u++) {

                                        if (rand() % (ratio) == 0)
                                        {
                                            test = false;
                                            nbrErreurs++;
                                            localErrors++;
                                        }
                                    }

                                    for (int uui = 0; uui < localErrors; uui++) {

                                        int pos2 = rand() % 2;

                                        int pos = rand() % 32;

                                        uint32_t tmpL = 0L;

                                        tmpL = UINT32_MAX;

                                        tmpL >>= 31;
                                        tmpL <<= 31;
                                        tmpL >>= (32 - pos);

                                        if (pos2 == 0) {

                                            uint32_t tmpLL = (uint32_t)((repp[j] << 32) >> 32);

                                            if ((tmpLL << pos) >> 31 == 0) {

                                                tmpLL |= tmpL;
                                            }
                                            else {

                                                tmpLL &= (tmpL ^ UINT32_MAX);
                                            }

                                            repp[j] >>= 32;
                                            repp[j] <<= 32;

                                            repp[j] |= tmpLL;

                                            uint64_t vv = repp[j];
                                        }
                                        else {

                                            uint32_t tmpLL = (uint32_t)(repp[j] >> 32);

                                            if ((tmpLL << pos) >> 31 == 0) {

                                                tmpLL |= tmpL;
                                            }
                                            else {

                                                tmpLL &= (tmpL ^ UINT32_MAX);
                                            }

                                            repp[j] <<= 32;
                                            repp[j] >>= 32;

                                            uint64_t tmpKJH = tmpLL;

                                            repp[j] |= (tmpKJH << 32);

                                            uint64_t vv = repp[j];
                                        }

                                        uint64_t uv = rep[j];
                                        uint64_t vv = repp[j];

                                        int hhhh = 0;
                                    }
                                }
                                /////////////////////////////
                                /////////////////////////////
                                for (int j = 0; j < (lengthData + 8) * 13; j++) {

                                    reppH[j] = repHash[j];

                                    //std::cout << repHash[j] << ":";

                                    testH = true;

                                    int localErrorsH = 0;

                                    for (int u = 0; u < 8; u++) {

                                        if (rand() % ratio == 0)
                                        {
                                            testH = false;
                                            nbrErreursH++;
                                            localErrorsH++;
                                        }
                                    }

                                    for (int uui = 0; uui < localErrorsH; uui++) {

                                        int pos = rand() % 8;

                                        char tmpL = 255;

                                        tmpL >>= 7;
                                        tmpL <<= 7;
                                        tmpL >>= (8 - pos);

                                        char tmpLL = reppH[j];

                                        if ((tmpLL << pos) >> 7 == 0) {

                                            tmpLL |= tmpL;
                                        }
                                        else {

                                            tmpLL &= (tmpL ^ INT8_MAX);
                                        }

                                        reppH[j] = tmpLL;
                                    }
                                }

                                std::vector<int> listeErreurs;

                                auto start2 = std::chrono::high_resolution_clock::now();

                                int* finaleData = E.decrypting(repp, lengthData, key, listeErreurs);
                                //int* finaleDataH = C.decode(reppH, lengthData);

                                auto end2 = std::chrono::high_resolution_clock::now();

                                std::chrono::duration<double> duration2 = end2 - start2;

                                totalTime += duration2.count();

                                bool test3 = false;
                                bool test4 = false;

                                for (int p = 0; p < lengthData; p++) {

                                    //std::cout << finaleData[p];

                                    if (finaleData[p] != data[p]) {

                                        test3 = true;
                                        break;
                                    }

                                    //int a = finaleDataH[p];
                                    int b = dataH[p];

                                    /*if (finaleDataH[p] != dataH[p]) {

                                        test4 = true;
                                        break;
                                    }*/
                                /*}

                                if (test3 && listeErreurs.size() == 0) std::cout << "f ";

                                if (test3) {

                                    failNbr++;
                                }

                                if (test4) failNbrH++;

                                nbrErreursCreated += nbrErreurs;
                            }

                            std::cout << "Lengthdata: " << lengthData << "  NumberofHash: " << oo << "  Ratiooffailure: " << y << " NumberOfFailure:" << failNbr << " / 10000_failtests(corrective)!" << " time:" << totalTime/10000 << std::endl;

                            outputFile << "Lengthdata: " << lengthData << "  NumberofHash: " << oo << "  Ratiooffailure: " << y << " NumberOfFailure:" << failNbr << " / 10000_failtests(corrective)!" << " time:" << totalTime/10000 << std::endl;
                        }

                        outputFile.close();
                    }
                }
            }
        }
    }
}


uint32_t* getRandomData(int size) {

    uint32_t* data = new uint32_t[size];

    for (int i = 0; i < size; i++) {

        data[i] = i%256;

        //std::cout << data[i];
    }

    return data;
}*/