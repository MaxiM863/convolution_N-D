#include<string>
#include<math.h>
#include<random>

class correcteur {

public:

    static uint64_t* encrypting(uint32_t* inputData, unsigned char* key, int dataLength) {

        std::vector<std::vector<int>> dimensions;
        std::vector<int> keyDataLayer;

        getOperationsData(key, dataLength, dimensions, keyDataLayer);

        uint64_t* dataEncrypted = new uint64_t[dataLength];

        for (int i = 0; i < dataLength; i++) {

            uint64_t a = convolution(dimensions, inputData, key, i, dataLength, keyDataLayer);

            uint64_t tmpD = 0;
            tmpD |= inputData[i];
            uint64_t tmpA = 0;
            tmpA |= (a << 8);

            dataEncrypted[i] = tmpA | tmpD;
        }

        return dataEncrypted;
    }

    static int* decrypting(uint64_t inputData[], int dataLength, unsigned char* key, std::vector<int>& listeErreurs) {

        int nbrHash = key[0];

        std::vector<std::vector<int>> dimensions;
        std::vector<int> keyPos;

        getOperationsData(key, dataLength, dimensions, keyPos);

        int** data = new int* [nbrHash];

        int* dataGlobale = new int[dataLength];

        std::vector<std::vector<int>> erreurs;

        for (int i = 0; i < nbrHash; i++) {

            data[i] = new int[dataLength];

            std::vector<int> tmpV;
            erreurs.push_back(tmpV);

            for (int j = dataLength - 1; j >= 0; j--) {

                data[i][j] = deconvolution(dimensions, inputData, key, j, dataLength, keyPos, erreurs[i], i);
            }
        }

        for (int hashNbr = 0; hashNbr < nbrHash; hashNbr++) {

            applyIterativeConvolution(dataLength, erreurs, hashNbr, dimensions, inputData, key, keyPos);

            applyHashErrorsDiscovery(dataLength, erreurs, hashNbr, dimensions, inputData, key, keyPos, data[hashNbr]);

            applyErrorsCorrection(dataLength, erreurs, hashNbr, dimensions, inputData, key, keyPos, data[hashNbr]);
        }

        std::vector<int> globalErreurs;

        int* dataT = new int[dataLength];

        for (int i = 0; i < dataLength; i++) {

            dataT[i] = data[0][i];
        }

        mergeHashData(erreurs, globalErreurs, dataT, data);

        for (int i = 0; i < dataLength; i++) {

            dataGlobale[i] = dataT[i];
        }

        listeErreurs = globalErreurs;

        return dataGlobale;
    }

    static unsigned char* getKey(std::vector<std::vector<int>> dimensions, int ecryptionAmplitude) {


        if (dimensions.size() > 0 && dimensions.size() <= 6 && ecryptionAmplitude != 0 && ecryptionAmplitude) {

            bool clefValide = true;


            int nbrHash = dimensions.size();
            int nbrElements = 0;
            int grandeurMatrice = 0;

            for (int j = 0; j < nbrHash; j++) {

                int gg = 1;

                for (int i = 0; i < dimensions[j].size(); i++) {

                    nbrElements++;

                    gg *= dimensions[j][i];

                    if (dimensions[j][i] < 1 || dimensions[j][i] > 6) clefValide = false;                    
                }

                grandeurMatrice += gg;
            }

            if (grandeurMatrice * ecryptionAmplitude * 256 > pow(2, 56 / nbrHash)) clefValide = false;

            unsigned char* key = new unsigned char[grandeurMatrice + nbrElements + nbrHash + 1 + 1];

            key[0] = nbrHash;

            int cntr = 1;

            for (int i = 0; i < nbrHash; i++) {

                key[cntr] = dimensions[i].size();

                cntr++;

                int countKey = 1;

                int subDim = dimensions[i].size();

                for (int j = 0; j < subDim; j++) {

                    countKey *= dimensions[i][j];
                    key[cntr] = dimensions[i][j];
                    cntr++;
                }

                int top = cntr;

                for (int i = top; i < countKey + top; i++) {

                    key[cntr] = (rand() % ecryptionAmplitude) + 1;
                    cntr++;
                }
            }

            key[cntr] = '\0';

            return key;
        }

        return nullptr;
    }

private:

    static uint64_t convolution(std::vector<std::vector<int>> dimensions, uint32_t* inputData, 
        unsigned char* key, int positionInData, int dataLength, std::vector<int> keyPos) {

        uint64_t valeurRetour = 0;

        for (int hashNbr = 0; hashNbr < dimensions.size(); hashNbr++) {

            int widthDataRelativeMatrix = getWidthOfDataMatrix(dataLength, dimensions.size());

            int longueurConvolution = getConvolutionLength(dimensions, hashNbr);

            int somme = 0;

            for (int positionInConvolution = longueurConvolution - 1; positionInConvolution >= 0; positionInConvolution--) {

                int relativePositionInData = positionInData;

                applyDimRelative(positionInConvolution, dimensions, hashNbr, relativePositionInData, widthDataRelativeMatrix);

                if (relativePositionInData < dataLength) {

                    somme += inputData[relativePositionInData] * key[keyPos[hashNbr] + positionInConvolution];
                }
                else {

                    recenterPositionInData(relativePositionInData, dataLength);

                    somme += inputData[relativePositionInData] * key[keyPos[hashNbr] + positionInConvolution];
                }
            }

            uint64_t tmp64 = translateDataIn64(dimensions, somme, hashNbr);

            valeurRetour |= tmp64;
        }

        return valeurRetour;
    }

    static int deconvolution(std::vector<std::vector<int>> dimensions, uint64_t inputData[], unsigned char* key, 
        int cntr, int dataLength, std::vector<int> keyPos, std::vector<int>& erreurs, int hashNbr) {

        int nbrHash = dimensions.size();
        int grandeurMatrice = getConvolutionLength(dimensions, hashNbr);

        uint64_t valeurRetour = 0;

        uint64_t sommeHash = extractHash(inputData, cntr, nbrHash, hashNbr);

        int base = (inputData[cntr] << 56) >> 56;

        int widthD = getWidthOfDataMatrix(dataLength, dimensions.size());

        std::vector<int> falsePos;

        for (int positionInConvolution = grandeurMatrice - 1; positionInConvolution >= 0; positionInConvolution--) {

            int relativePositionInData = cntr;

            applyDimRelative(positionInConvolution, dimensions, hashNbr, relativePositionInData, widthD);

            recenterPositionInData(relativePositionInData, dataLength);

            uint64_t valeur = (inputData[relativePositionInData] << 56) >> 56;

            if (positionInConvolution == 0) sommeHash /= key[keyPos[hashNbr] + positionInConvolution];
            else sommeHash -= valeur * key[keyPos[hashNbr] + positionInConvolution];

            falsePos.push_back(relativePositionInData);
        }

        if (base != sommeHash) {

            for (int i = 0; i < falsePos.size(); i++) {

                bool test = false;

                for (int j = 0; j < erreurs.size(); j++) {
                    
                    if (erreurs[j] == falsePos[i]) test = true;
                }

                if(!test) erreurs.push_back(falsePos[i]);
            }
        }

        return base;
    }

    static void corriger(std::vector<std::vector<int>> dimensions, uint64_t* inputData, unsigned char* key, 
        int cntr, int dataLength, std::vector<int> keyPos, std::vector<int>& erreurs, int* data, int hashNbr) {

        // trouver l'oppos� g�om�trique:
        // effectuer les translation pour sortir l'erreur

        int nbrHash = dimensions.size();
        int grandeurMatrice = 1;

        uint64_t valeurRetour = 0;

        for (int i = 0; i < dimensions[hashNbr].size(); i++) {

            grandeurMatrice *= dimensions[hashNbr][i];
        }

        int widthD = floor(pow(exp(1.0), std::log(dataLength) / dimensions[hashNbr].size()));

        std::vector<int> sommes;

        for (int hhh = 0; hhh < dimensions.size(); hhh++) {

            for (int w = 1; w <= dimensions[hhh].size(); w++) {

                for (int y = -1; y <= -1; y++) {

                    for (int u = 1; u < dimensions[hhh][w - 1]; u++) {

                        if (y != 0) {

                            int hh = w - 1;

                            int indiceOppose = erreurs[cntr];


                            int indice = 0;

                            int i = y * u;

                            int aa = i / abs(i);

                            i = abs(i);

                            applyDimRelative(i, dimensions, hh, indice, widthD);

                            indiceOppose += aa * indice;

                            recenterPositionInData(indiceOppose, dataLength);

                            int posPossibleErreur = -1;
                            int ii = 0;

                            uint64_t somme = extractHash(inputData, indiceOppose, nbrHash, hh);


                            for (int j = grandeurMatrice - 1; j >= 0; j--) {

                                int indice = 0;
                                int i = j;

                                applyDimRelative(i, dimensions, hh, indice, widthD);

                                indice *= -aa;

                                indice += indiceOppose;

                                recenterPositionInData(indice, dataLength);

                                if (erreurs[cntr] != indice) {

                                    uint8_t valeur = (uint8_t)((inputData[indice] << 56) >> 56);

                                    somme -= valeur * key[keyPos[hh] + j];

                                    bool testR = true;

                                    for (int f = 0; f < erreurs.size(); f++) {

                                        if (indice == erreurs[f])
                                        {
                                            testR = false;
                                        }
                                    }

                                    if (!testR) {

                                        posPossibleErreur = j;
                                        break;
                                    }
                                }
                                else {


                                    ii = j;
                                }
                            }

                            if (posPossibleErreur == -1) sommes.push_back(somme / key[keyPos[hh] + ii]);
                        }
                    }
                }
            }
        }

        bool test = false;
        int goodAnswer = -1;
        int countF = 0;

        for (int llk = 0; llk < sommes.size(); llk++) {

            for (int lkk = 0; lkk < sommes.size(); lkk++) {

                if (llk != lkk) {

                    if (sommes[llk] == sommes[lkk]) {

                        countF++;
                        goodAnswer = sommes[llk];
                        break;
                    }
                    else {

                        int o = 0;
                    }
                }
            }
        }

        if (countF >= sommes.size()-2) {

            data[erreurs[cntr]] = goodAnswer;

            inputData[erreurs[cntr]] >>= 8;

            inputData[erreurs[cntr]] <<= 8;

            inputData[erreurs[cntr]] |= goodAnswer;

            erreurs.erase(erreurs.begin() + cntr);

            return;
        }
    }

    static void separer(std::vector<std::vector<int>> dimensions, uint64_t* inputData, unsigned char* key,
        int cntr, int dataLength, std::vector<int> keyPos, std::vector<int>& erreurs, int data[], int hashNbr) {

        // trouver le point:
        // effectuer les translations pour trouver un seul vrai, ce qui met la faute sur le hash

        int nbrHash = dimensions.size();
        int grandeurMatrice = getConvolutionLength(dimensions, hashNbr);

        int widthD = getWidthOfDataMatrix(dataLength, dimensions.size());

        std::vector<std::vector<int>> sommes;

        int has = hashNbr;

        for (int w = 1; w <= dimensions[has].size(); w++) {

            for (int u = 1; u < dimensions[has][w - 1]; u++) {

                int hh = has;

                int indiceOppose = erreurs[cntr];

                int indice = 0;

                applyDimRelative(u, dimensions, hh, indice, widthD);

                indiceOppose -= indice;

                recenterPositionInData(indiceOppose, dataLength);

                // effectuer les convolutions en fonction des hashs:

                uint64_t somme = extractHash(inputData, indiceOppose, nbrHash, hh);
                int hash = (inputData[indiceOppose] << 56) >> 56;

                int posPossibleErreur = -1;

                int ii = -1;

                std::vector<int> tmpDimension;

                for (int j = grandeurMatrice - 1; j >= 0; j--) {

                    int indice = 0;
                    int i = j;

                    applyDimRelative(i, dimensions, hh, indice, widthD);

                    indice += indiceOppose;

                    if (indice == erreurs[cntr]) ii = j;

                    recenterPositionInData(indice, dataLength);

                    tmpDimension.push_back(indice);

                    int valeur = (inputData[indice] << 56) >> 56;

                    somme -= valeur * key[keyPos[hh] + j];

                    bool testR = true;

                    for (int f = 0; f < erreurs.size(); f++) {

                        if (f != cntr && indice == erreurs[f])
                        {
                            testR = false;
                        }
                    }

                    if (!testR) {

                        posPossibleErreur = j;
                        break;
                    }
                }

                if (somme == 0 && posPossibleErreur == -1) sommes.push_back(tmpDimension);
            }
        }


        if (sommes.size() > 0) {

            for (int i = 0; i < sommes.size(); i++) {

                for (int j = 0; j < sommes[i].size(); j++) {

                    for (int k = 0; k < erreurs.size(); k++) {

                        if (erreurs[k] == sommes[i][j]) {

                            erreurs.erase(erreurs.begin() + k);
                            k--;
                        }
                    }
                }
            }
        }
    }

    static void deleteErreurs(std::vector<std::vector<int>> dimensions, uint64_t* inputData, unsigned char* key, 
        int cntr, int dataLength, std::vector<int> keyPos, std::vector<int>& erreurs, int hashNbr) {

        int nbrHash = dimensions.size();
        int grandeurMatrice = getConvolutionLength(dimensions, hashNbr);

        uint64_t valeurRetour = 0;
        
        uint64_t somme = extractHash(inputData, cntr, nbrHash, hashNbr);
        uint8_t hash = (uint8_t)((inputData[cntr] << 56) >> 56);

        int widthD = getWidthOfDataMatrix(dataLength, dimensions.size());

        std::vector<int> liste;

        for (int j = grandeurMatrice - 1; j >= 0; j--) {

            int indice = cntr;

            applyDimRelative(j, dimensions, hashNbr, indice, widthD);

            recenterPositionInData(indice, dataLength);

            liste.push_back(indice);
        }

        for (int i = 0; i < liste.size(); i++) {

            for (int j = 0; j < erreurs.size(); j++) {

                if (erreurs[j] == liste[i]) {

                    erreurs.erase(erreurs.begin() + j);
                    j--;
                }
            }
        }
    }

    static void applyDimRelative(int subRelativeIndice, std::vector<std::vector<int>> dimensions, 
        int hashNbr, int& outRelativeIndice, int widthOutRelativeMatrix) {

        for (int dd = dimensions[hashNbr].size(); dd > 0; dd--) {

            int dimensionActuelle = dimensions[hashNbr][dd - 1];

            if (subRelativeIndice > 0 && dimensionActuelle > subRelativeIndice) {

                outRelativeIndice += (int)((subRelativeIndice) % dimensionActuelle) * pow(widthOutRelativeMatrix, dd - 1);
                subRelativeIndice -= dimensionActuelle - 1;
            }
        }
    }

    static void applyIterativeConvolution(int dataLength, std::vector<std::vector<int>>& erreurs, int hashNbr,
        std::vector<std::vector<int>> dimensions, uint64_t* inputData, unsigned char* key, std::vector<int> keyPos) {

        for (int i = 0; i < dataLength; i++) {

            if (erreurs[hashNbr].size() > 0) {

                std::vector<int> tmp;

                deconvolution(dimensions, inputData, key, i, dataLength, keyPos, tmp, hashNbr);

                if (tmp.size() == 0) {

                    deleteErreurs(dimensions, inputData, key, i, dataLength, keyPos, erreurs[hashNbr], hashNbr);
                }
            }
        }

        for (int i = dataLength - 1; i >= 0; i--) {

            if (erreurs[hashNbr].size() > 0) {

                std::vector<int> tmp;

                deconvolution(dimensions, inputData, key, i, dataLength, keyPos, tmp, hashNbr);

                if (tmp.size() == 0) {

                    deleteErreurs(dimensions, inputData, key, i, dataLength, keyPos, erreurs[hashNbr], hashNbr);
                }
            }
        }
    }

    static void applyHashErrorsDiscovery(int dataLength, std::vector<std::vector<int>>& erreurs, int hashNbr,
        std::vector<std::vector<int>> dimensions, uint64_t* inputData, unsigned char* key, std::vector<int> keyPos, int data[]) {

        std::vector<int> erreursHash;

        int nbrHash = erreursHash.size();
        int ttt = erreursHash.size();

        for (int i = 0; i < erreurs[hashNbr].size(); i++) {

            separer(dimensions, inputData, key, i, dataLength, keyPos, erreurs[hashNbr], data, hashNbr);

            i -= erreursHash.size() - nbrHash;
            nbrHash = erreursHash.size();
        }

        for (int i = erreurs[hashNbr].size() - 1; i >= 0; i--) {

            separer(dimensions, inputData, key, i, dataLength, keyPos, erreurs[hashNbr], data, hashNbr);

            i -= erreursHash.size() - nbrHash;
            nbrHash = erreursHash.size();
        }
    }

    static void applyErrorsCorrection(int dataLength, std::vector<std::vector<int>>& erreurs, int hashNbr, 
        std::vector<std::vector<int>> dimensions, uint64_t* inputData, unsigned char* key, std::vector<int> keyPos, 
        int* dataOutCorrected) {

        bool testErreurs = true;

        while (testErreurs) {

            int size = erreurs[hashNbr].size();
            std::vector<int> tmpE = erreurs[hashNbr];

            for (int i = 0; i < tmpE.size(); i++) {

                corriger(dimensions, inputData, key, i, dataLength, keyPos, tmpE, dataOutCorrected, hashNbr);

                i -= size - tmpE.size();
                size = tmpE.size();
            }

            if (erreurs[hashNbr].size() == size) testErreurs = false;

            erreurs[hashNbr] = tmpE;
        }
    }

    static void mergeHashData(std::vector<std::vector<int>> erreurs, std::vector<int>& globalErreurs, int* dataT, int** data) {

        int countErreurs = erreurs[0].size();

        for (int j = 0; j < erreurs[0].size(); j++) {

            std::vector<int> whereIsIncorrect;

            for (int k = 1; k < erreurs.size(); k++) {

                bool test = false;

                for (int l = 0; l < erreurs[k].size(); l++) {

                    if (erreurs[0][j] == erreurs[k][l]) {

                        test = true;
                        break;
                    }
                }

                if (!test) {

                    dataT[erreurs[0][j]] = data[k][erreurs[0][j]];
                    countErreurs--;
                }
            }
        }

        globalErreurs.push_back(countErreurs);
    }

    static uint64_t extractHash(uint64_t inputData[], int cntr, int nbrHash, int hashNbr) {

        uint64_t deb = (int)(56 / nbrHash);

        return (inputData[cntr] << ((56 - nbrHash * deb) + (nbrHash - hashNbr - 1) * deb) >> (8 + (56 - nbrHash * deb) + (nbrHash - 1) * deb));
    }

    static void getOperationsData(unsigned char* key, int dataLength,
        std::vector<std::vector<int>>& dimensionsArray, std::vector<int>& positionInKey) {

        int numberOfHash = key[0];

        int counterPositionKey = 1;

        for (int i = 0; i < numberOfHash; i++) {

            std::vector<int> discretesDimensions;

            int numberOfElementsPerDimenion = key[counterPositionKey++];

            int keyDataLength = 1;

            for (int j = 0; j < numberOfElementsPerDimenion; j++) {

                discretesDimensions.push_back(key[counterPositionKey++]);
                keyDataLength *= discretesDimensions[discretesDimensions.size() - 1];
            }

            positionInKey.push_back(counterPositionKey);

            counterPositionKey += keyDataLength;

            dimensionsArray.push_back(discretesDimensions);
        }
    }

    static int getWidthOfDataMatrix(int dataLength, int dimensionsSize) {

        return floor(pow(exp(1.0), std::log(dataLength) / dimensionsSize));
    }

    static int getConvolutionLength(std::vector<std::vector<int>> dimensions, int hashNbr) {

        int length = 1;

        for (int d = 0; d < dimensions.size(); d++) {

            length *= dimensions[hashNbr][d];
        }

        return length;
    }

    static void recenterPositionInData(int& positionInData, int dataLength) {

        if (positionInData >= dataLength) {

            int count = dataLength;
            int ccount = 1;

            while (positionInData - ccount * count >= 0) {

                ccount++;
            }

            positionInData -= (ccount - 1) * count;
        }
        else if (positionInData < 0) {

            int count = dataLength;
            int ccount = 1;

            while (positionInData + ccount * count < count) {

                ccount++;
            }

            positionInData += (ccount - 1) * count;
        }
    }

    static uint64_t translateDataIn64(std::vector<std::vector<int>> dimensions, int somme, int hashNbr) {

        int lB = (int)(56 / dimensions.size());

        uint64_t tmp64 = 0;
        
        tmp64 |= (uint64_t)somme;

        tmp64 <<= (hashNbr * lB);

        return tmp64;
    }
};