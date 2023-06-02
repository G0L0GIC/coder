/**
 * main.cpp
 *
 * ��̬���������루FGK�㷨��
 * by {[()]}
 * 05.2023
 */

/**
 * ���ڶ�̬�����������ԭ���μ� http://ben-tanen.com/adaptive-huffman/
 * ���� C++ STL ��ʹ�ã��μ� https://learn.microsoft.com/zh-cn/cpp/standard-library/cpp-standard-library-reference?view=msvc-170
 * ����֮����뻹ʹ����һЩ��Ҳ�̫��Ϥ��C++�������ԣ����������
 * ͷ�ļ���α������namespace��constexpr��enum��using��nullptr���﷨�������������ã���ķ�װ�淶����
 * ��Щ�����Ǳ���ģ�����дֻ��Ϊ���ô���˽���ࡣ
 * ������д�ıȽ����⣬��Ϊ��ֻ��һ�����Դ��롣�ϸ���ļ�����Ӧ�������ļ��Ƿ���ڡ����Ƿ��쳣�ȡ�
 */

#include <fstream> // ifstream, ofstream
#include "ahcoder.h"

int main() {
	// ѹ��
	std::ifstream ifile1;
	std::ofstream ofile1;

	ifile1.open("test.txt", std::ios::in);
	ofile1.open("test_encode.txt", std::ios::binary);

	adaptivecodes::ahcoder ah(ifile1, ofile1);

	ifile1.close();
	ofile1.close();

	// ��ѹ
	std::ifstream ifile2;
	std::ofstream ofile2;

	ifile2.open("test_encode.txt", std::ios::binary);
	ofile2.open("test_decode.txt", std::ios::out);

	adaptivecodes::ahdecoder ahde(ifile2, ofile2);

	ifile2.close();
	ofile2.close();

	return 0;
}
