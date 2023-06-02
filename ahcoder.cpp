/**
 * ahcoder.cpp
 *
 * 动态哈夫曼编码（FGK算法）
 * by {[()]}
 * 05.2023
 */

#include <fstream> // ifstream, ofstream
#include <cstdlib> // size_t
#include <cstdint> // uint8_t, uint64_t
#include <vector> // vector
#include <algorithm> // reserve(), swap(), 后者在C++11以后改定义到<utility>中了
#include <climits> // CHAR_BIT
#include "ahcoder.h"

namespace adaptivecodes {

	using bitseq_t  = std::vector<bool>;
	using symbseq_t = std::vector<uint8_t>;
	constexpr size_t MAX_LEAF_NUM = 256;
	constexpr size_t MAX_NODE_NUM = 512;

	// NYT = Not Yet Transmitted
	enum node_type {NYT_NODE = -1, INTERNAL_NODE = -2}; // 特殊节点类型标记

	// 节点类
	struct Node {
		int      symbol; // 字符或标记
		int      order; // 编号
		uint64_t weight; // 权重

		Node *parent;
		Node *left;
		Node *right;

		Node(
		    int symbol_,
		    int order_,
		    uint64_t weight_ = 0,
		    Node *parent_ = nullptr,
		    Node *left_   = nullptr,
		    Node *right_  = nullptr
		)
			: symbol(symbol_), order(order_), weight(weight_), parent(parent_), left(left_), right(right_)
		{ }
	};

	// 哈夫曼树

	class fgk {
			Node    *m_nyt; // 溢出码，解码时用来标记新符号的出现
			Node    *m_root;
			Node    *m_leaves[MAX_LEAF_NUM];
			Node    *m_nodes[MAX_NODE_NUM + 1];
			Node    *m_dcurr; // 解码时的当前节点
			bitseq_t m_buf;

			// 从被修改的节点开始向上调整，恢复兄弟性质
			void update_tree(Node *node) {
				while (node) {
					Node *highest = highest_in_class(node);
					swap(highest, node); // 将节点与相同权重的编号最高的结点交换，再更新权重值
					++node->weight;
					node = node->parent;
				}
			}

			// 获得与该节点相同权重的编号最高的节点
			Node *highest_in_class(Node *node) const {
				Node *highest = node;
				int num = node->order;

				for (size_t i = num + 1; i <= MAX_NODE_NUM && m_nodes[i]->weight == node->weight; ++i)
					highest = m_nodes[i];

				return highest;
			}

			// 交换两个节点
			void swap(Node *a, Node *b) {
				if (a == m_root || b == m_root || a == b || a->parent == b || b->parent == a)
					return;

				if (a->parent->left == a && b->parent->left == b)
					std::swap(a->parent->left, b->parent->left);
				else if (a->parent->left == a && b->parent->right == b)
					std::swap(a->parent->left, b->parent->right);
				else if (a->parent->right == a && b->parent->left == b)
					std::swap(a->parent->right, b->parent->left);
				else if (a->parent->right == a && b->parent->right == b)
					std::swap(a->parent->right, b->parent->right);

				std::swap(m_nodes[a->order], m_nodes[b->order]);
				std::swap(a->order, b->order);
				std::swap(a->parent, b->parent);
			}

			// 判断节点是否实际存在
			bool is_in_tree(uint8_t byte) const {
				if (m_leaves[byte])
					return 1;
				return 0;
			}

			// 从节点获得编码
			bitseq_t get_symbol_code(const Node *symbolNode) const {
				bitseq_t code;
				code.reserve(16);

				const Node *currNode = symbolNode;
				while (currNode != m_root) {
					/* if (currNode->parent->left == currNode)
						code.push_back(0);
					else
						code.push_back(1); */
					code.push_back(currNode->parent->right == currNode);
					currNode = currNode->parent;
				}

				std::reverse(code.begin(), code.end());
				return code;
			}

			void encode_existing_byte(uint8_t byte) {
				update_tree(m_leaves[byte]);
			}

			// 本函数负责插入节点
			void encode_new_byte(uint8_t byte) {
				m_nyt->symbol = INTERNAL_NODE;
				m_nyt->left   = new Node(NYT_NODE, m_nyt->order - 2, 0, m_nyt);
				m_nyt->right  = new Node(byte, m_nyt->order - 1, 0, m_nyt);

				m_leaves[byte] = m_nyt->right;
				m_nodes[m_nyt->left->order] = m_nyt->left;
				m_nodes[m_nyt->right->order] = m_nyt->right;

				m_nyt = m_nyt->left;

				update_tree(m_leaves[byte]);
			}

			void delete_tree() {
				delete_tree(m_root);
			}

			void delete_tree(const Node *m_root) {
				if (!m_root)
					return;

				delete_tree(m_root->left);
				delete_tree(m_root->right);
				delete m_root;
			}

		public:
			fgk() : m_nyt(new Node(NYT_NODE, MAX_NODE_NUM)), m_root(m_nyt), m_dcurr(m_root) {
				for (auto &&leaf : m_leaves)
					leaf = nullptr;
				for (auto &&node : m_nodes)
					node = nullptr;

				m_nodes[MAX_NODE_NUM] = m_nyt;
			}

			~fgk() {
				delete_tree();
			}

			// 求字符的编码
			bitseq_t encode(uint8_t byte) {
				if (is_in_tree(byte)) {
					bitseq_t code = get_symbol_code(m_leaves[byte]);

					encode_existing_byte(byte);

					return code;
				} else {
					bitseq_t code = get_symbol_code(m_nyt);

					for (size_t i = 0; i < CHAR_BIT; ++i)
						code.push_back(byte & (1 << (7 - i)));

					encode_new_byte(byte);

					return code;
				}
			}

			// 求编码列的字符列
			symbseq_t decode(const bitseq_t &codeseq) {
				symbseq_t symbseq;

				std::copy(std::begin(codeseq), std::end(codeseq), std::back_inserter(m_buf));

				size_t i = 0;
				while (i < m_buf.size() || (!m_dcurr->left && !m_dcurr->right))
					if (m_dcurr->symbol == NYT_NODE) {
						if (m_buf.size() - i >= CHAR_BIT) { // 新码
							uint8_t ascii_char = 0;

							for (size_t j = 0; j < CHAR_BIT; ++j)
								ascii_char |= m_buf[i + j] << (7 - j);

							symbseq.push_back(ascii_char);

							encode_new_byte(ascii_char);

							i += CHAR_BIT;
							m_dcurr = m_root;
						} else
							break; // end of file
					} else if (!m_dcurr->left && !m_dcurr->right) { // 旧码结束
						symbseq.push_back(m_dcurr->symbol);

						encode_existing_byte(m_dcurr->symbol);

						m_dcurr = m_root;
					} else { // 寻码过程中
						if (m_buf[i])
							m_dcurr = m_dcurr->right;
						else
							m_dcurr = m_dcurr->left;
						++i;
					}

				m_buf.erase(std::begin(m_buf), std::begin(m_buf) + i);

				return symbseq;
			}

			bitseq_t get_nyt_code() const {
				return get_symbol_code(m_nyt);
			}
	};

	// -------------------------------------------------------
	// ---------------------- CODERIMPL ----------------------
	// -------------------------------------------------------

	class ahcoder::CoderImpl : private fgk {
			void flush_output_buffer(std::ostream &ofile, bitseq_t &outbuf) {
				symbseq_t out_bytes;
				out_bytes.reserve((MAX_NODE_NUM * CHAR_BIT) / 2);

				size_t pos = 0;
				while (pos + CHAR_BIT <= outbuf.size()) {
					uint8_t byte = 0;

					for (size_t i = 0; i < CHAR_BIT; ++i)
						if (outbuf[pos + i])
							byte |= 1 << (7 - i);

					out_bytes.push_back(byte);

					pos += CHAR_BIT;
				}

				outbuf.erase(std::begin(outbuf), std::begin(outbuf) + pos);
				ofile.write(reinterpret_cast<const char *>(&out_bytes.front()), out_bytes.size());
			}

		public:
			void compress(std::ifstream &ifile, std::ofstream &ofile) {
				uint8_t inbuf[MAX_NODE_NUM]; // 输入区
				bitseq_t outbuf; // 输出区
				outbuf.reserve(MAX_NODE_NUM * CHAR_BIT + 64); // 申请内存

				// 按 MAX_NODE_NUM 依次处理
				while (ifile.good()) {
					ifile.read(reinterpret_cast<char *>(inbuf), MAX_NODE_NUM); // 读入
					size_t bytes_read = ifile.gcount(); // 字符数统计

					for (size_t i = 0; i < bytes_read ; ++i) {
						bitseq_t code = encode(inbuf[i]); // 依次取字符编码
						std::copy(std::begin(code), std::end(code), std::back_inserter(outbuf)); // 写入输出区
					}

					// 及时刷新输出区
					if (outbuf.size() >= MAX_NODE_NUM * CHAR_BIT)
						flush_output_buffer(ofile, outbuf);
				}

				// 处理输出区最后部分
				if (!outbuf.empty()) {
					// 处理多余字节
					if (outbuf.size() % CHAR_BIT != 0) {
						bitseq_t nyt_code = get_nyt_code();

						size_t extra_bits = CHAR_BIT - outbuf.size() % CHAR_BIT;
						for (size_t i = 0; i < extra_bits; ++i)
							outbuf.push_back(nyt_code[i % nyt_code.size()]);
					}
					flush_output_buffer(ofile, outbuf);
				}
			}

			void operator()(std::ifstream &ifile, std::ofstream &ofile) {
				compress(ifile, ofile);
			}

			CoderImpl(std::ifstream &ifile, std::ofstream &ofile) {
				compress(ifile, ofile);
			}

			CoderImpl()
			{ }
	};

	void ahcoder::compress(std::ifstream &ifile, std::ofstream &ofile) {
		m_pImpl->compress(ifile, ofile);
	}

	void ahcoder::operator()(std::ifstream &ifile, std::ofstream &ofile) {
		m_pImpl->operator()(ifile, ofile);
	}

	ahcoder::ahcoder(std::ifstream &ifile, std::ofstream &ofile)
		: m_pImpl(new CoderImpl(ifile, ofile))
	{ }

	ahcoder::ahcoder() : m_pImpl(new CoderImpl)
	{ }

	ahcoder::~ahcoder()
	{ }

	// -------------------------------------------------------
	// --------------------- DECODERIMPL ---------------------
	// -------------------------------------------------------

	class ahdecoder::DecoderImpl : private fgk {
		public:
			// 解压
			void decompress(std::ifstream &ifile, std::ofstream &ofile) {
				uint8_t inbuf[MAX_NODE_NUM]; // 输入区

				// 按 MAX_NODE_NUM 依次处理
				while (ifile.good()) {
					ifile.read(reinterpret_cast<char *>(inbuf), MAX_NODE_NUM);
					size_t bytes_read = ifile.gcount();

					bitseq_t seq;
					seq.reserve(MAX_NODE_NUM * CHAR_BIT);

					// 依次解码
					for (size_t i = 0; i < bytes_read; ++i) {
						bitseq_t code(CHAR_BIT);
						for (size_t j = 0; j < CHAR_BIT; ++j)
							code[j] = inbuf[i] & (1 << (7 - j));

						std::copy(std::begin(code), std::end(code), std::back_inserter(seq));
					}

					symbseq_t outbuf = decode(seq); // 将解码结果写入输出区
					ofile.write(reinterpret_cast<const char *>(&outbuf.front()), outbuf.size()); // 刷新输出区
				}
			}

			void operator()(std::ifstream &ifile, std::ofstream &ofile) {
				decompress(ifile, ofile);
			}

			DecoderImpl(std::ifstream &ifile, std::ofstream &ofile) {
				decompress(ifile, ofile);
			}

			DecoderImpl()
			{ }
	};

	void ahdecoder::decompress(std::ifstream &ifile, std::ofstream &ofile) {
		m_pImpl->decompress(ifile, ofile);
	}

	void ahdecoder::operator()(std::ifstream &ifile, std::ofstream &ofile) {
		m_pImpl->operator()(ifile, ofile);
	}

	ahdecoder::ahdecoder(std::ifstream &ifile, std::ofstream &ofile)
		: m_pImpl(new DecoderImpl(ifile, ofile))
	{ }

	ahdecoder::ahdecoder() : m_pImpl(new DecoderImpl)
	{ }

	ahdecoder::~ahdecoder()
	{ }

}
