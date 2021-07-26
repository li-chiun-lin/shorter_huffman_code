#pragma once
#include<iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <string>



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


errno_t printByteToFile(vector<unsigned char>& byte_buf, int width, int height, string file_name);
vector<unsigned char> readImageRaw(string file_name);
struct tiffInfo readImageTiff(string file, vector<unsigned char>& buf);


void writeImage(string file_name, vector<unsigned char>& byte_buf);
unsigned char bits2byte(vector<unsigned char>& bit_buf, int idx = 0, int len = 8);
//unsigned char bits2byte(vector<unsigned char>& bit_buf);
//unsigned char bits2byte(vector<int>& bit_buf);

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
		return a->freq > b->freq || (a->freq == b->freq && a->val > b->val) ;
	}
};

Node* buildHuffmanTree(const vector<int>& freq);
void del(Node* tree);
vector< vector<unsigned char> > buildHuffmanEncoder(const Node* tree, int size_domain);
void dfs(const Node* tree, vector< vector<unsigned char> >& encoder, int* code, int len);
void serializeHuffmanTree(const Node* tree, vector<unsigned char>& bit_topology, vector<unsigned char>& bit_val, int size_val);
Node* unserializeHuffmanTree(vector<unsigned char>& bit_topology, int& it, vector<unsigned char>& bit_val, int& iv, int size_val);
pair<unsigned char, int> decode(Node* tree, vector<unsigned char>& bits_buf, size_t idx_start);
void printHuffmanEncoder(vector< vector<unsigned char> >& h_encoder);




// report ------------------------------------------------------

# define LEN_SAMPLE 2048
# define ROW_SAMPLE 8
# define WIDTH_REPORT 128
# define WIDTH_REPORT_1 WIDTH_REPORT - 1
# define WRAP(i) (i % WIDTH_REPORT == WIDTH_REPORT_1 ? "\n\t" : "")



extern FILE* fptr_report;

int reportBegin(int width_image, int height_image, int size_image, const char* fn_report, const char* fn_cover_img);
void reportEnd();

void reportB(vector<unsigned char>& byte_image, vector<unsigned char>& byte_rand_keystream);
void reportB3(vector<unsigned char>& byte_image, const char* fn_enc_img);
//void reportC(vector<int>& h_encoder, vector<int>& cnt_t, int len_h_encoded_labelmap, int len_secret);
void reportD(vector<unsigned char>& bits_embed, int idx_prepare, int len_hc_rule);
void reportD2(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed, int header_log2_len_h_encoded_labelmap, int len_hc_rule);
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


// msb:                          header_len_h_values || header_log2_len_h_encoded_labelmap || len_h_encoded_labelmap || Ref || secret
// shr: header_len_h_topology || header_len_h_values || header_log2_len_h_encoded_labelmap || len_h_encoded_labelmap || Ref || secret
//      ------------------------------------------------------------------------------------
//      |-->                                 header                                     <--|
//
//      len_secret = capacity - len_h_encoded_labelmap - header

struct Result
{
	int size_image;
	int width_image;
	int height_image;

	int header_len_h_topology;
	int header_len_h_values;
	int header_log2_len_h_encoded_labelmap;
	int header_len;
	
	int len_h_encoded_labelmap;

	int len_ref;

	int len_secret;
	int len_secret_without_header;

	int size_capacity;
	int size_capacity_pure;
	int size_capacity_with_header;

	float bbp;

	int freq[9];
	int len_code[9];
	unsigned char code_hex[9];
	char code_str[9][9];

	bool isValid;
	bool loopInvariantViolation;

	Result()
	{
		size_image = 0;
		width_image = 0;
		height_image = 0;

		header_len_h_topology = 0;
		header_len_h_values = 0;
		header_log2_len_h_encoded_labelmap = 0;
		header_len = 0;

		len_h_encoded_labelmap = 0;

		len_ref = 0;

		len_secret = 0;
		len_secret_without_header = 0;

		size_capacity = 0;
		size_capacity_pure = 0;
		size_capacity_with_header = 0;
		
		bbp = 0.0;

		for (int i = 0; i < 9; ++i)
		{
			freq[i] = 0;
			len_code[i] = 0;
			code_hex[i] = 0;
			code_str[i][0] = 0;
		}

		isValid = false;
		loopInvariantViolation = false;
	}

	void deduce()
	{
		header_len = header_len_h_topology + header_len_h_values + header_log2_len_h_encoded_labelmap;

		len_secret_without_header = len_secret + header_len;

		size_capacity_with_header = len_h_encoded_labelmap + len_secret;
		size_capacity_pure = size_capacity_with_header + header_len;

		bbp = (float)len_secret_without_header / size_image;
	}
};

struct Result operator-(struct Result lhs, struct Result rhs);



void printResult(struct Result res);


struct tiffInfo
{
	int width;
	int height;
	int sampleperpixel;

	tiffInfo() : 
		width(0), height(0), sampleperpixel(0)
	{}
};