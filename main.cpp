/**
 * main.cpp
 *
 * 动态哈夫曼编码（FGK算法）
 * by {[()]}
 * 05.2023
 */

/**
 * 关于动态哈夫曼编码的原理，参见 http://ben-tanen.com/adaptive-huffman/
 * 关于 C++ STL 的使用，参见 https://learn.microsoft.com/zh-cn/cpp/standard-library/cpp-standard-library-reference?view=msvc-170
 * 除此之外代码还使用了一些大家不太熟悉的C++语言特性，具体包括：
 * 头文件、伪函数、namespace、constexpr、enum、using、nullptr的语法，缓冲区的设置，类的封装规范……
 * 这些都不是必须的，这样写只是为了让大家了解更多。
 * 主函数写的比较随意，因为它只是一个测试代码。严格的文件操作应当考虑文件是否存在、流是否异常等。
 */

#include <fstream> // ifstream, ofstream
#include "ahcoder.h"

int main() {
	// 压缩
	std::ifstream ifile1;
	std::ofstream ofile1;

	ifile1.open("test.txt", std::ios::in);
	ofile1.open("test_encode.txt", std::ios::binary);

	adaptivecodes::ahcoder ah(ifile1, ofile1);

	ifile1.close();
	ofile1.close();

	// 解压
	std::ifstream ifile2;
	std::ofstream ofile2;

	ifile2.open("test_encode.txt", std::ios::binary);
	ofile2.open("test_decode.txt", std::ios::out);

	adaptivecodes::ahdecoder ahde(ifile2, ofile2);

	ifile2.close();
	ofile2.close();

	return 0;
}
