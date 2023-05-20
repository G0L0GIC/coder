#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cerrno>
#include <vector>
#include <queue>
#include <utility>
#include <climits>
#include "coder.h"

namespace contextcodes {
	using freq_vec_t = typename std::vector<uint32_t>;
	using freq_table_t = typename std::vector<freq_vec_t>;
	using scheme_vec_t = typename std::vector<std::vector<bool> >;
	using scheme_table_t = typename std::vector<scheme_vec_t>;
	using bitseq_t = typename std::vector<bool>;
	constexpr size_t ALPHABET = 256;

	class Statistics {
	protected:
		freq_vec_t   m_freq_vec;
		freq_table_t m_freq_table;
		uint64_t     m_total_chars;
		void create_freq_vector(std::ifstream& ifile) {
			m_freq_vec.clear();
			m_freq_vec.resize(ALPHABET, 0);
			m_freq_table.clear();
			m_freq_table.resize(ALPHABET);
			char c;
			char context;
			ifile.read(&context, sizeof(context));
			++m_freq_vec[static_cast<uint8_t>(context)];
			m_total_chars = 1;
			while (ifile.read(&c, sizeof(c))) {
				if (m_freq_table[static_cast<uint8_t>(context)].empty()) {
					m_freq_table[static_cast<uint8_t>(context)].clear();
					m_freq_table[static_cast<uint8_t>(context)].resize(ALPHABET, 0);
				}
				++m_freq_vec[static_cast<uint8_t>(c)];
				++m_freq_table[static_cast<uint8_t>(context)][static_cast<uint8_t>(c)];
				++m_total_chars;
				context = c;
			}
		}
		Statistics()
		{ }
	};

	struct Node {
		int     left;
		int     right;
		uint8_t symbol;
		Node(int l, int r, uint8_t s) {
			left = l;
			right = r;
			symbol = s;
		}
	};

	using tree_t = typename std::vector<Node>;
	using forest_t = typename std::vector<tree_t>;

	class CodeTree {
	protected:
		void traverse_code_tree(tree_t& m_tree, scheme_vec_t& m_scheme_vec, int index, bitseq_t code) {
			if (index == -1) return;
			if (m_tree[index].left == -1 && m_tree[index].right == -1) {
				m_scheme_vec[m_tree[index].symbol] = code;
				return;
			}
			code.push_back(0);
			traverse_code_tree(m_tree, m_scheme_vec, m_tree[index].left, code);
			code.pop_back();
			code.push_back(1);
			traverse_code_tree(m_tree, m_scheme_vec, m_tree[index].right, code);
			code.pop_back();
		}
		CodeTree()
		{ }
	};

	class huffman : private CodeTree {
		void create_code_tree(freq_vec_t& m_freq_vec) {
			using pair_t = typename std::pair<uint32_t, int>;
			std::priority_queue<pair_t, std::vector<pair_t>, std::greater<pair_t> > queue;
			for (size_t i = 0; i < m_freq_vec.size(); ++i) {
				if (m_freq_vec[i]) {
					m_tree.push_back(Node(-1, -1, i));
					queue.push(pair_t(m_freq_vec[i], m_tree.size() - 1));
				}
			}
			while (queue.size() > 1) {
				pair_t child1;
				pair_t child2;
				uint32_t parent_w;
				child1 = queue.top(); queue.pop();
				child2 = queue.top(); queue.pop();
				parent_w = child1.first + child2.first;
				m_tree.push_back(Node(child1.second, child2.second, 0));
				queue.push(pair_t(parent_w, m_tree.size() - 1));
			}
			if (m_tree.size() == 1) m_tree.push_back(Node(0, -1, 0));
		}
	public:
		tree_t       m_tree;
		scheme_vec_t m_scheme_vec;
		void create_code_scheme(freq_vec_t& m_freq_vec) {
			m_scheme_vec.clear();
			m_scheme_vec.resize(ALPHABET);
			if (m_freq_vec.empty()) return;
			if (m_freq_vec.size() == 1) {
				m_scheme_vec[m_freq_vec.front()] = bitseq_t(1, 0);
				return;
			}
			create_code_tree(m_freq_vec);
			traverse_code_tree(m_tree, m_scheme_vec, m_tree.size() - 1, bitseq_t());
		}
	};