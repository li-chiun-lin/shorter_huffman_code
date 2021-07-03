#pragma once
#include<iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <cassert>
#include <algorithm>

using namespace std;

const unsigned char pixel2t[256] = {
	8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

const unsigned char byte2bits[256][8] = {
	{0, 0, 0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0, 0, 1 }, {0, 0, 0, 0, 0, 0, 1, 0 }, {0, 0, 0, 0, 0, 0, 1, 1 },
	{0, 0, 0, 0, 0, 1, 0, 0 }, {0, 0, 0, 0, 0, 1, 0, 1 }, {0, 0, 0, 0, 0, 1, 1, 0 }, {0, 0, 0, 0, 0, 1, 1, 1 },
	{0, 0, 0, 0, 1, 0, 0, 0 }, {0, 0, 0, 0, 1, 0, 0, 1 }, {0, 0, 0, 0, 1, 0, 1, 0 }, {0, 0, 0, 0, 1, 0, 1, 1 },
	{0, 0, 0, 0, 1, 1, 0, 0 }, {0, 0, 0, 0, 1, 1, 0, 1 }, {0, 0, 0, 0, 1, 1, 1, 0 }, {0, 0, 0, 0, 1, 1, 1, 1 },
	{0, 0, 0, 1, 0, 0, 0, 0 }, {0, 0, 0, 1, 0, 0, 0, 1 }, {0, 0, 0, 1, 0, 0, 1, 0 }, {0, 0, 0, 1, 0, 0, 1, 1 },
	{0, 0, 0, 1, 0, 1, 0, 0 }, {0, 0, 0, 1, 0, 1, 0, 1 }, {0, 0, 0, 1, 0, 1, 1, 0 }, {0, 0, 0, 1, 0, 1, 1, 1 },
	{0, 0, 0, 1, 1, 0, 0, 0 }, {0, 0, 0, 1, 1, 0, 0, 1 }, {0, 0, 0, 1, 1, 0, 1, 0 }, {0, 0, 0, 1, 1, 0, 1, 1 },
	{0, 0, 0, 1, 1, 1, 0, 0 }, {0, 0, 0, 1, 1, 1, 0, 1 }, {0, 0, 0, 1, 1, 1, 1, 0 }, {0, 0, 0, 1, 1, 1, 1, 1 },
	{0, 0, 1, 0, 0, 0, 0, 0 }, {0, 0, 1, 0, 0, 0, 0, 1 }, {0, 0, 1, 0, 0, 0, 1, 0 }, {0, 0, 1, 0, 0, 0, 1, 1 },
	{0, 0, 1, 0, 0, 1, 0, 0 }, {0, 0, 1, 0, 0, 1, 0, 1 }, {0, 0, 1, 0, 0, 1, 1, 0 }, {0, 0, 1, 0, 0, 1, 1, 1 },
	{0, 0, 1, 0, 1, 0, 0, 0 }, {0, 0, 1, 0, 1, 0, 0, 1 }, {0, 0, 1, 0, 1, 0, 1, 0 }, {0, 0, 1, 0, 1, 0, 1, 1 },
	{0, 0, 1, 0, 1, 1, 0, 0 }, {0, 0, 1, 0, 1, 1, 0, 1 }, {0, 0, 1, 0, 1, 1, 1, 0 }, {0, 0, 1, 0, 1, 1, 1, 1 },
	{0, 0, 1, 1, 0, 0, 0, 0 }, {0, 0, 1, 1, 0, 0, 0, 1 }, {0, 0, 1, 1, 0, 0, 1, 0 }, {0, 0, 1, 1, 0, 0, 1, 1 },
	{0, 0, 1, 1, 0, 1, 0, 0 }, {0, 0, 1, 1, 0, 1, 0, 1 }, {0, 0, 1, 1, 0, 1, 1, 0 }, {0, 0, 1, 1, 0, 1, 1, 1 },
	{0, 0, 1, 1, 1, 0, 0, 0 }, {0, 0, 1, 1, 1, 0, 0, 1 }, {0, 0, 1, 1, 1, 0, 1, 0 }, {0, 0, 1, 1, 1, 0, 1, 1 },
	{0, 0, 1, 1, 1, 1, 0, 0 }, {0, 0, 1, 1, 1, 1, 0, 1 }, {0, 0, 1, 1, 1, 1, 1, 0 }, {0, 0, 1, 1, 1, 1, 1, 1 },
	{0, 1, 0, 0, 0, 0, 0, 0 }, {0, 1, 0, 0, 0, 0, 0, 1 }, {0, 1, 0, 0, 0, 0, 1, 0 }, {0, 1, 0, 0, 0, 0, 1, 1 },
	{0, 1, 0, 0, 0, 1, 0, 0 }, {0, 1, 0, 0, 0, 1, 0, 1 }, {0, 1, 0, 0, 0, 1, 1, 0 }, {0, 1, 0, 0, 0, 1, 1, 1 },
	{0, 1, 0, 0, 1, 0, 0, 0 }, {0, 1, 0, 0, 1, 0, 0, 1 }, {0, 1, 0, 0, 1, 0, 1, 0 }, {0, 1, 0, 0, 1, 0, 1, 1 },
	{0, 1, 0, 0, 1, 1, 0, 0 }, {0, 1, 0, 0, 1, 1, 0, 1 }, {0, 1, 0, 0, 1, 1, 1, 0 }, {0, 1, 0, 0, 1, 1, 1, 1 },
	{0, 1, 0, 1, 0, 0, 0, 0 }, {0, 1, 0, 1, 0, 0, 0, 1 }, {0, 1, 0, 1, 0, 0, 1, 0 }, {0, 1, 0, 1, 0, 0, 1, 1 },
	{0, 1, 0, 1, 0, 1, 0, 0 }, {0, 1, 0, 1, 0, 1, 0, 1 }, {0, 1, 0, 1, 0, 1, 1, 0 }, {0, 1, 0, 1, 0, 1, 1, 1 },
	{0, 1, 0, 1, 1, 0, 0, 0 }, {0, 1, 0, 1, 1, 0, 0, 1 }, {0, 1, 0, 1, 1, 0, 1, 0 }, {0, 1, 0, 1, 1, 0, 1, 1 },
	{0, 1, 0, 1, 1, 1, 0, 0 }, {0, 1, 0, 1, 1, 1, 0, 1 }, {0, 1, 0, 1, 1, 1, 1, 0 }, {0, 1, 0, 1, 1, 1, 1, 1 },
	{0, 1, 1, 0, 0, 0, 0, 0 }, {0, 1, 1, 0, 0, 0, 0, 1 }, {0, 1, 1, 0, 0, 0, 1, 0 }, {0, 1, 1, 0, 0, 0, 1, 1 },
	{0, 1, 1, 0, 0, 1, 0, 0 }, {0, 1, 1, 0, 0, 1, 0, 1 }, {0, 1, 1, 0, 0, 1, 1, 0 }, {0, 1, 1, 0, 0, 1, 1, 1 },
	{0, 1, 1, 0, 1, 0, 0, 0 }, {0, 1, 1, 0, 1, 0, 0, 1 }, {0, 1, 1, 0, 1, 0, 1, 0 }, {0, 1, 1, 0, 1, 0, 1, 1 },
	{0, 1, 1, 0, 1, 1, 0, 0 }, {0, 1, 1, 0, 1, 1, 0, 1 }, {0, 1, 1, 0, 1, 1, 1, 0 }, {0, 1, 1, 0, 1, 1, 1, 1 },
	{0, 1, 1, 1, 0, 0, 0, 0 }, {0, 1, 1, 1, 0, 0, 0, 1 }, {0, 1, 1, 1, 0, 0, 1, 0 }, {0, 1, 1, 1, 0, 0, 1, 1 },
	{0, 1, 1, 1, 0, 1, 0, 0 }, {0, 1, 1, 1, 0, 1, 0, 1 }, {0, 1, 1, 1, 0, 1, 1, 0 }, {0, 1, 1, 1, 0, 1, 1, 1 },
	{0, 1, 1, 1, 1, 0, 0, 0 }, {0, 1, 1, 1, 1, 0, 0, 1 }, {0, 1, 1, 1, 1, 0, 1, 0 }, {0, 1, 1, 1, 1, 0, 1, 1 },
	{0, 1, 1, 1, 1, 1, 0, 0 }, {0, 1, 1, 1, 1, 1, 0, 1 }, {0, 1, 1, 1, 1, 1, 1, 0 }, {0, 1, 1, 1, 1, 1, 1, 1 },
	{1, 0, 0, 0, 0, 0, 0, 0 }, {1, 0, 0, 0, 0, 0, 0, 1 }, {1, 0, 0, 0, 0, 0, 1, 0 }, {1, 0, 0, 0, 0, 0, 1, 1 },
	{1, 0, 0, 0, 0, 1, 0, 0 }, {1, 0, 0, 0, 0, 1, 0, 1 }, {1, 0, 0, 0, 0, 1, 1, 0 }, {1, 0, 0, 0, 0, 1, 1, 1 },
	{1, 0, 0, 0, 1, 0, 0, 0 }, {1, 0, 0, 0, 1, 0, 0, 1 }, {1, 0, 0, 0, 1, 0, 1, 0 }, {1, 0, 0, 0, 1, 0, 1, 1 },
	{1, 0, 0, 0, 1, 1, 0, 0 }, {1, 0, 0, 0, 1, 1, 0, 1 }, {1, 0, 0, 0, 1, 1, 1, 0 }, {1, 0, 0, 0, 1, 1, 1, 1 },
	{1, 0, 0, 1, 0, 0, 0, 0 }, {1, 0, 0, 1, 0, 0, 0, 1 }, {1, 0, 0, 1, 0, 0, 1, 0 }, {1, 0, 0, 1, 0, 0, 1, 1 },
	{1, 0, 0, 1, 0, 1, 0, 0 }, {1, 0, 0, 1, 0, 1, 0, 1 }, {1, 0, 0, 1, 0, 1, 1, 0 }, {1, 0, 0, 1, 0, 1, 1, 1 },
	{1, 0, 0, 1, 1, 0, 0, 0 }, {1, 0, 0, 1, 1, 0, 0, 1 }, {1, 0, 0, 1, 1, 0, 1, 0 }, {1, 0, 0, 1, 1, 0, 1, 1 },
	{1, 0, 0, 1, 1, 1, 0, 0 }, {1, 0, 0, 1, 1, 1, 0, 1 }, {1, 0, 0, 1, 1, 1, 1, 0 }, {1, 0, 0, 1, 1, 1, 1, 1 },
	{1, 0, 1, 0, 0, 0, 0, 0 }, {1, 0, 1, 0, 0, 0, 0, 1 }, {1, 0, 1, 0, 0, 0, 1, 0 }, {1, 0, 1, 0, 0, 0, 1, 1 },
	{1, 0, 1, 0, 0, 1, 0, 0 }, {1, 0, 1, 0, 0, 1, 0, 1 }, {1, 0, 1, 0, 0, 1, 1, 0 }, {1, 0, 1, 0, 0, 1, 1, 1 },
	{1, 0, 1, 0, 1, 0, 0, 0 }, {1, 0, 1, 0, 1, 0, 0, 1 }, {1, 0, 1, 0, 1, 0, 1, 0 }, {1, 0, 1, 0, 1, 0, 1, 1 },
	{1, 0, 1, 0, 1, 1, 0, 0 }, {1, 0, 1, 0, 1, 1, 0, 1 }, {1, 0, 1, 0, 1, 1, 1, 0 }, {1, 0, 1, 0, 1, 1, 1, 1 },
	{1, 0, 1, 1, 0, 0, 0, 0 }, {1, 0, 1, 1, 0, 0, 0, 1 }, {1, 0, 1, 1, 0, 0, 1, 0 }, {1, 0, 1, 1, 0, 0, 1, 1 },
	{1, 0, 1, 1, 0, 1, 0, 0 }, {1, 0, 1, 1, 0, 1, 0, 1 }, {1, 0, 1, 1, 0, 1, 1, 0 }, {1, 0, 1, 1, 0, 1, 1, 1 },
	{1, 0, 1, 1, 1, 0, 0, 0 }, {1, 0, 1, 1, 1, 0, 0, 1 }, {1, 0, 1, 1, 1, 0, 1, 0 }, {1, 0, 1, 1, 1, 0, 1, 1 },
	{1, 0, 1, 1, 1, 1, 0, 0 }, {1, 0, 1, 1, 1, 1, 0, 1 }, {1, 0, 1, 1, 1, 1, 1, 0 }, {1, 0, 1, 1, 1, 1, 1, 1 },
	{1, 1, 0, 0, 0, 0, 0, 0 }, {1, 1, 0, 0, 0, 0, 0, 1 }, {1, 1, 0, 0, 0, 0, 1, 0 }, {1, 1, 0, 0, 0, 0, 1, 1 },
	{1, 1, 0, 0, 0, 1, 0, 0 }, {1, 1, 0, 0, 0, 1, 0, 1 }, {1, 1, 0, 0, 0, 1, 1, 0 }, {1, 1, 0, 0, 0, 1, 1, 1 },
	{1, 1, 0, 0, 1, 0, 0, 0 }, {1, 1, 0, 0, 1, 0, 0, 1 }, {1, 1, 0, 0, 1, 0, 1, 0 }, {1, 1, 0, 0, 1, 0, 1, 1 },
	{1, 1, 0, 0, 1, 1, 0, 0 }, {1, 1, 0, 0, 1, 1, 0, 1 }, {1, 1, 0, 0, 1, 1, 1, 0 }, {1, 1, 0, 0, 1, 1, 1, 1 },
	{1, 1, 0, 1, 0, 0, 0, 0 }, {1, 1, 0, 1, 0, 0, 0, 1 }, {1, 1, 0, 1, 0, 0, 1, 0 }, {1, 1, 0, 1, 0, 0, 1, 1 },
	{1, 1, 0, 1, 0, 1, 0, 0 }, {1, 1, 0, 1, 0, 1, 0, 1 }, {1, 1, 0, 1, 0, 1, 1, 0 }, {1, 1, 0, 1, 0, 1, 1, 1 },
	{1, 1, 0, 1, 1, 0, 0, 0 }, {1, 1, 0, 1, 1, 0, 0, 1 }, {1, 1, 0, 1, 1, 0, 1, 0 }, {1, 1, 0, 1, 1, 0, 1, 1 },
	{1, 1, 0, 1, 1, 1, 0, 0 }, {1, 1, 0, 1, 1, 1, 0, 1 }, {1, 1, 0, 1, 1, 1, 1, 0 }, {1, 1, 0, 1, 1, 1, 1, 1 },
	{1, 1, 1, 0, 0, 0, 0, 0 }, {1, 1, 1, 0, 0, 0, 0, 1 }, {1, 1, 1, 0, 0, 0, 1, 0 }, {1, 1, 1, 0, 0, 0, 1, 1 },
	{1, 1, 1, 0, 0, 1, 0, 0 }, {1, 1, 1, 0, 0, 1, 0, 1 }, {1, 1, 1, 0, 0, 1, 1, 0 }, {1, 1, 1, 0, 0, 1, 1, 1 },
	{1, 1, 1, 0, 1, 0, 0, 0 }, {1, 1, 1, 0, 1, 0, 0, 1 }, {1, 1, 1, 0, 1, 0, 1, 0 }, {1, 1, 1, 0, 1, 0, 1, 1 },
	{1, 1, 1, 0, 1, 1, 0, 0 }, {1, 1, 1, 0, 1, 1, 0, 1 }, {1, 1, 1, 0, 1, 1, 1, 0 }, {1, 1, 1, 0, 1, 1, 1, 1 },
	{1, 1, 1, 1, 0, 0, 0, 0 }, {1, 1, 1, 1, 0, 0, 0, 1 }, {1, 1, 1, 1, 0, 0, 1, 0 }, {1, 1, 1, 1, 0, 0, 1, 1 },
	{1, 1, 1, 1, 0, 1, 0, 0 }, {1, 1, 1, 1, 0, 1, 0, 1 }, {1, 1, 1, 1, 0, 1, 1, 0 }, {1, 1, 1, 1, 0, 1, 1, 1 },
	{1, 1, 1, 1, 1, 0, 0, 0 }, {1, 1, 1, 1, 1, 0, 0, 1 }, {1, 1, 1, 1, 1, 0, 1, 0 }, {1, 1, 1, 1, 1, 0, 1, 1 },
	{1, 1, 1, 1, 1, 1, 0, 0 }, {1, 1, 1, 1, 1, 1, 0, 1 }, {1, 1, 1, 1, 1, 1, 1, 0 }, {1, 1, 1, 1, 1, 1, 1, 1 },
};

const vector<unsigned char> mask_bit = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
const vector<unsigned char> mask_msb = { 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };
const vector<unsigned char> mask_lsb = { 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00 };


errno_t printByteToFile(vector<unsigned char>& byte_buf, int len, string file_name);
vector<unsigned char> readImage(string file_name);
void writeImage(string file_name, vector<unsigned char>& byte_buf);
unsigned char bits2byte(vector<unsigned char>& bit_buf, int idx, int len = 8);

void XOR(vector<unsigned char>& byte_image, vector<unsigned char>& byte_rand_keystream);
unsigned char predictedValue(vector<unsigned char>& byte_image, int idx, int width_image);



// huffman coding --------------------------------------------------------------

# define MAX_T 8

struct Trie
{
	bool isValid;
	unsigned char val_t;
	struct Trie* next[2];
};

void del(struct Trie* coder);
void build(struct Trie* coder, const vector<unsigned char>& bits_buf);
void assignValue(struct Trie* coder, vector<unsigned char>& bits_buf, size_t idx_start, size_t idx_end, unsigned char val);
pair<unsigned char, int> decode(struct Trie* coder, vector<unsigned char>& bits_buf, size_t idx_start);



class Node
{
public:
	int val;
	int freq;
	Node* next[2];

	Node(int v = 0, int f = 0) :
		val(v), freq(f)
	{
		next[0] = NULL;
		next[1] = NULL;
	}

	bool operator()(Node* a, Node* b)
	{
		return a->freq > b->freq;
	}
};

Node* buildHuffmanTree(const vector<int>& freq);
void del(Node* tree);
vector< vector<int> > buildHuffmanEncoder(const Node* tree, int size_domain);
void dfs(const Node* tree, vector< vector<int> >& encoder, int* code, int len);
void serializeHuffmanTree(const Node* tree, vector<unsigned char>& bit_topology, vector<unsigned char>& bit_val, int size_val);
Node* unserializeHuffmanTree(vector<unsigned char>& bit_topology, int& it, vector<unsigned char>& bit_val, int& iv, int size_val);
pair<unsigned char, int> decode(Node* tree, vector<unsigned char>& bits_buf, size_t idx_start);




// report ------------------------------------------------------

# define LEN_SAMPLE 2048
# define ROW_SAMPLE 8
# define WIDTH_REPORT 128
# define WIDTH_REPORT_1 WIDTH_REPORT - 1
# define WRAP(i) (i % WIDTH_REPORT == WIDTH_REPORT_1 ? "\n\t" : "")



extern FILE* fptr_report;

int reportBegin(int width_image, int size_image, const char* fn_report, const char* fn_cover_img);
void reportEnd();

void reportB(vector<unsigned char>& byte_image, vector<unsigned char>& byte_rand_keystream);
void reportB3(vector<unsigned char>& byte_image, const char* fn_enc_img);
//void reportC(vector<int>& h_encoder, vector<int>& cnt_t, int len_h_encoded_labelmap, int len_secret);
void reportD(vector<unsigned char>& bits_embed, int idx_prepare, int len_hc_rule);
void reportD2(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed, int len_len_h_encoded_labelmap, int len_hc_rule);
void reportD3(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed, int len_hcode_header);
void reportD4(vector<unsigned char>& byte_image, int idx_prepare, int len_embed, int width_image);
void reportD5(vector<unsigned char>& byte_image, int idx_prepare, int len_embed, int width_image);
void reportD6(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed, int len_auxinfo);
void reportD7(vector<unsigned char>& byte_image, int idx_embed, int width_image, int len_auxinfo);
void reportD8(vector<unsigned char>& byte_image, int idx_embed, int width_image, int len_auxinfo);
void reportF(vector<unsigned char>& byte_image, vector<unsigned char>& bit_extract, int idx_ex, int width_image);
void reportF2(vector<unsigned char>& byte_image, int idx_ex, int width_image);
void reportF5(vector<unsigned char>& val_ex_labelmap, int idx_ex, int idx_ex_proceed, int width_image);
void reportF6(vector<unsigned char>& byte_image, int idx_ex, int idx_ex_proceed, int width_image);
void reportF7(vector<unsigned char>& byte_image, int idx_ex, int idx_ex_proceed, int width_image);
void reportF8(vector<unsigned char>& bit_secret_extra, int idx_ex, int idx_ex_proceed);
void reportG(vector<unsigned char>& byte_image, vector<unsigned char>& byte_rand_keystream);
void reportG3(vector<unsigned char>& byte_image);
void reportG4(vector<unsigned char>& byte_image, const char* fn_restore);