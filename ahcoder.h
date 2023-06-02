/**
 * ahcoder.h
 *
 * ��̬���������루FGK�㷨��
 * by {[()]}
 * 05.2023
 */

#pragma once
#ifndef AHCODER_H
#define AHCODER_H

#include <fstream> // ifstream, ofstream
#include <memory> // unique_ptr

namespace adaptivecodes {
	// ѹ����
	class ahcoder {
			class CoderImpl; // ������
			std::unique_ptr<CoderImpl> m_pImpl; // ������������ָ��
		public:
			void compress(std::ifstream &ifile, std::ofstream &ofile); // ѹ��
			void operator()(std::ifstream &ifile, std::ofstream &ofile); // ���أ��������
			ahcoder(std::ifstream &ifile, std::ofstream &ofile);
			ahcoder();
			~ahcoder();
	};

	// ��ѹ��
	class ahdecoder {
			class DecoderImpl; // ������
			std::unique_ptr<DecoderImpl> m_pImpl; // ������������ָ��
		public:
			void decompress(std::ifstream &ifile, std::ofstream &ofile); // ��ѹ
			void operator()(std::ifstream &ifile, std::ofstream &ofile); // ���أ��������
			ahdecoder(std::ifstream &ifile, std::ofstream &ofile);
			ahdecoder();
			~ahdecoder();
	};
}

#endif // AHCODER_H
