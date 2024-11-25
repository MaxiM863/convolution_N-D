#pragma once
#include <cstdint>
#include <cstddef>
#include <string>


class eccConvolutionalHash {

public:

	static int* encode(char* data, int dataLength) {

		int* retour = new int[(dataLength + 8) * 13];

		std::size_t str_hash = std::hash<std::string>{}(std::string(data));

		for (int i = 0; i < 13; i++) {

			for (int j = 0; j < dataLength; j++) {

				retour[(i * (dataLength + 8)) + j] = data[j];
			}

			for (int j = 0; j < 8; j++) {

				int tmpC = (str_hash << j * 8) >> 7*8;

				retour[(i * (dataLength + 8)) + dataLength + j] = tmpC;
			}
		}

		return retour;
	}

	static int* decode(int* dataE, int dataLength) {

		int* retour = new int[(dataLength + 8) * 13];

		for (int i = 0; i < 13; i++) {

			char* data = new char[dataLength + 1];
			std::size_t hash = 0;

			for (int j = 0; j < dataLength; j++) {

				data[j] = dataE[(i * (dataLength + 8)) + j];
			}

			for (int j = 0; j < 8; j++) {

				unsigned long long tmpC = dataE[(i * (dataLength + 8)) + j + dataLength];

				hash |= (tmpC << ((8-j-1) * 8));
			}

			data[dataLength] = '\0';
			
			std::size_t str_hash = std::hash<std::string>{}(std::string(data));

			int* dataR = new int[dataLength];

			for (int oo = 0; oo < dataLength; oo++) dataR[oo] = data[oo];

			if (str_hash == hash) {

				return dataR;
			}
		}

		return retour;
	}
};