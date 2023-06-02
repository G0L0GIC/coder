/**
 * ahcoder.h
 *
 * 动态哈夫曼编码（FGK算法）
 * by {[()]}
 * 05.2023
 */

#pragma once
#ifndef AHCODER_H
#define AHCODER_H

#include <fstream> // ifstream, ofstream
#include <memory> // unique_ptr

namespace adaptivecodes {
	// 压缩类
	class ahcoder {
			class CoderImpl; // 编码器
			std::unique_ptr<CoderImpl> m_pImpl; // 编码器的智能指针
		public:
			void compress(std::ifstream &ifile, std::ofstream &ofile); // 压缩
			void operator()(std::ifstream &ifile, std::ofstream &ofile); // 重载（）运算符
			ahcoder(std::ifstream &ifile, std::ofstream &ofile);
			ahcoder();
			~ahcoder();
	};

	// 解压类
	class ahdecoder {
			class DecoderImpl; // 解码器
			std::unique_ptr<DecoderImpl> m_pImpl; // 解码器的智能指针
		public:
			void decompress(std::ifstream &ifile, std::ofstream &ofile); // 解压
			void operator()(std::ifstream &ifile, std::ofstream &ofile); // 重载（）运算符
			ahdecoder(std::ifstream &ifile, std::ofstream &ofile);
			ahdecoder();
			~ahdecoder();
	};
}

#endif // AHCODER_H
