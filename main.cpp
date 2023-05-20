#include <fstream> // ifstream, ofstream
#include "coder.h"

int main() {
	std::ifstream ifile1;
	std::ofstream ofile1;

	ifile1.open("test.txt", std::ios::in);
	ofile1.open("test_encode.txt", std::ios::binary);

	huffman_codes::encoder en(ifile1, ofile1);

	ifile1.close();
	ofile1.close();

	std::ifstream ifile2;
	std::ofstream ofile2;

	ifile2.open("test_encode.txt", std::ios::binary);
	ofile2.open("test_decode.txt", std::ios::out);

	huffman_codes::decoder de(ifile2, ofile2);

	ifile2.close();
	ofile2.close();

	return 0;
}
