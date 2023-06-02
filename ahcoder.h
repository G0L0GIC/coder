#pragma once
#ifndef CODER_H
#define CODER_H

#include <fstream>
#include <memory>

namespace huffman_codes {
	class encoder {
		class CoderImpl;
		std::unique_ptr<CoderImpl> m_pImpl;
	public:
		void compress(std::ifstream& ifile, std::ofstream& ofile);
		void operator()(std::ifstream& ifile, std::ofstream& ofile);
		encoder(std::ifstream& ifile, std::ofstream& ofile);
		encoder();
		~encoder();
	};
	class decoder {
		class DecoderImpl;
		std::unique_ptr<DecoderImpl> m_pImpl;
	public:
		void decompress(std::ifstream& ifile, std::ofstream& ofile);
		void operator()(std::ifstream& ifile, std::ofstream& ofile);
		decoder(std::ifstream& ifile, std::ofstream& ofile);
		decoder();
		~decoder();
	};
}

#endif // CODER_H
