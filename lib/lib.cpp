// lib.cpp : 定義靜態程式庫的函式。
//

#include "pch.h"
#include "framework.h"

#include "lib.h"

#include "libtiff/tiffio.h"

using namespace std;


errno_t printByteToFile(vector<unsigned char>& byte_buf, int len, string file_name)
{
	FILE* fptr = NULL;
	errno_t err = fopen_s(&fptr, file_name.c_str(), "w");

	if (err || fptr == NULL)
		return err;

	for (int row = 0; row < len; ++row)
	{
		int rw = row * len;
		for (int col = 0; col < len; ++col)
			fprintf(fptr, "%02X ", byte_buf[static_cast<std::vector<unsigned char, std::allocator<unsigned char>>::size_type>(rw) + col]);
		fprintf(fptr, "\n");
	}

	fclose(fptr);
	return 0;
}

vector<unsigned char> readImageRaw(string file_name)
{
	ifstream file(file_name, ios::binary);
	streampos fileSize;

	// Stop eating new lines in binary mode!!!
	file.unsetf(ios::skipws);

	file.seekg(0, ios::end);
	fileSize = file.tellg();
	file.seekg(0, ios::beg);

	// reserve capacity
	vector<unsigned char> byte_buf;
	byte_buf.reserve((size_t)fileSize);

	// read the data:
	byte_buf.insert(byte_buf.begin(),
		istream_iterator<unsigned char>(file),
		istream_iterator<unsigned char>());

	return byte_buf;
}

struct tiffInfo readImageTiff(string file, vector<unsigned char>& buf)
{
	struct tiffInfo info;

	TIFF* tif = TIFFOpen(file.c_str(), "r");
	if (tif == NULL)
		return info;

	
	int spp = 0;
	

	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);

	info.sampleperpixel = spp;

	if (spp != 1)
		return info;

	int width, height;
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

	info.width = width;
	info.height = height;

	tmsize_t sls = TIFFScanlineSize(tif);
	tdata_t buf_tif = _TIFFmalloc(sls);

	for (int row = 0; row < height; row++)
	{
		TIFFReadScanline(tif, buf_tif, row);
		unsigned char* data = (unsigned char*)buf_tif;

		buf.insert(buf.end(), data, data + width);
	}

	//printf("%d %d %d %zu\n", width, height, spp, sls);


	_TIFFfree(buf_tif);
	TIFFClose(tif);

	return info;
}

void writeImage(string file_name, vector<unsigned char>& byte_buf)
{
	ofstream ofs(file_name, ios::binary | ios::out);
	ofs.write((const char*)byte_buf.data(), byte_buf.size());
	ofs.close();
}

unsigned char bits2byte(vector<unsigned char>& bit_buf, int idx, int len)
{
	assert(0 < len && len <= 8);

	unsigned char byte_ret = 0;

	for (int i = 0; i < len && i < bit_buf.size(); ++i)
	{
		byte_ret <<= 1;
		byte_ret |= bit_buf[static_cast<std::vector<unsigned char, std::allocator<unsigned char>>::size_type>(idx) + i];
	}

	return byte_ret << (8 - len);
}

//unsigned char bits2byte(vector<unsigned char>& bit_buf)
//{
//	unsigned char byte_ret = 0;
//
//	for (int i = 0; i < bit_buf.size(); ++i)
//	{
//		byte_ret <<= 1;
//		byte_ret |= bit_buf[i];
//	}
//
//	return byte_ret;
//}

//unsigned char bits2byte(vector<int>& bit_buf)
//{
//	unsigned char byte_ret = 0;
//
//	for (int i = 0; i < bit_buf.size(); ++i)
//	{
//		byte_ret <<= 1;
//		byte_ret |= bit_buf[i];
//	}
//
//	return byte_ret;
//}

void del(struct Trie* coder)
{
	if (coder == NULL)
		return;

	del(coder->next[0]);
	del(coder->next[1]);

	free(coder);
}

// build a valid huffman coding structure
void build(struct Trie* coder, const vector<unsigned char>& bits_buf)
{
	assert(coder);

	for (size_t i = 0; i < bits_buf.size() && coder; ++i)
	{
		int idx = bits_buf[i];
		assert(idx == 1 || idx == 0);

		if (coder->next[idx] == NULL)
			coder->next[idx] = (struct Trie*)calloc(1, sizeof(struct Trie));
		coder = coder->next[idx];
	}

	if (coder)
		coder->isValid = true;
}

// set the value of a valid huffman coding structure
void assignValue(struct Trie* coder, vector<unsigned char>& bits_buf, size_t idx_start, size_t idx_end, unsigned char val)
{
	assert(idx_start < idx_end);

	for (size_t i = idx_start; i < bits_buf.size() && i < idx_end; ++i)
	{
		int idx = bits_buf[i];
		assert(idx == 1 || idx == 0);

		// integrity check
		assert(coder->next[idx]);

		coder = coder->next[idx];
	}

	if (coder->isValid)
		coder->val_t = val;
}

// return (value, length)
pair<unsigned char, int> decode(struct Trie* coder, vector<unsigned char>& bits_buf, size_t idx_start)
{
	assert(coder);
	assert(0 <= idx_start);

	for (size_t i = idx_start; i < bits_buf.size(); ++i)
	{
		int idx = bits_buf[i];
		assert(idx == 1 || idx == 0);

		if (coder->isValid)
			return make_pair(coder->val_t, (int)(i - idx_start));

		// integrity check
		assert(coder->next[idx]);

		coder = coder->next[idx];
	}

	// integrity check
	// should never reach here
	assert(false);

	// not enough bits to decode next value
	return make_pair(-1, -1);
}

Node* buildHuffmanTree(const vector<int>& freq)
{
	priority_queue<Node*, vector<Node*>, Node > pq;

	for (int t = 0; t < freq.size(); ++t)
		pq.push(new Node(t, freq[t]));

	//cout << "pq size " << pq.size() << endl;

	while (pq.size() > 1)
	{
		Node* node = new Node();

		node->next[0] = pq.top();
		pq.pop();
		node->next[1] = pq.top();
		pq.pop();

		//printf("%d (%d) + %d (%d) \n", node->next[0]->freq, node->next[0]->val, node->next[1]->freq, node->next[1]->val);

		node->freq = node->next[0]->freq + node->next[1]->freq;

		pq.push(node);
	}

	return pq.top();
}

void del(Node* tree)
{
	if (tree == NULL)
		return;

	del(tree->next[0]);
	del(tree->next[1]);

	delete tree;
}

vector<vector<unsigned char>> buildHuffmanEncoder(const Node* tree, int size_domain)
{
	vector<vector<unsigned char>> encoder(size_domain);
	const int size_range = 10;
	int code[size_range] = { 0 };

	dfs(tree, encoder, code, 0);

	return encoder;
}

void dfs(const Node* tree, vector<vector<unsigned char>>& encoder, int* code, int len)
{
	if (tree == NULL)
		return;

	if (tree->next[0] == NULL && tree->next[1] == NULL)
	{
		encoder[tree->val] = vector<unsigned char>(code, code + len);
		return;
	}

	code[len] = 0;
	dfs(tree->next[0], encoder, code, len + 1);

	code[len] = 1;
	dfs(tree->next[1], encoder, code, len + 1);
}

void serializeHuffmanTree(const Node* tree, vector<unsigned char>& bit_topology, vector<unsigned char>& bit_val, int size_bit_val)
{
	if (tree == NULL)
		return;

	if (tree->next[0] == NULL && tree->next[1] == NULL)
	{
		bit_topology.push_back(0);

		unsigned int mask = 1 << (size_bit_val - 1);

		for (int i = 0; i < size_bit_val; ++i, mask >>= 1)
			bit_val.push_back((tree->val & mask) ? 1 : 0);
	}
	else
	{
		bit_topology.push_back(1);
		serializeHuffmanTree(tree->next[0], bit_topology, bit_val, size_bit_val);
		serializeHuffmanTree(tree->next[1], bit_topology, bit_val, size_bit_val);
	}
}

Node* unserializeHuffmanTree(vector<unsigned char>& bit_topology, int& it, vector<unsigned char>& bit_val, int& iv, int size_val)
{
	if (it == bit_topology.size())
		assert(false);

	int bit = bit_topology[it++];

	if (bit == 0)
	{
		int val = 0;
		for (int i = 0; i < size_val; ++i)
			val = (val << 1) | bit_val[iv++];

		return new Node(val);
	}
	else if (bit == 1)
	{
		Node* node = new Node();
		node->next[0] = unserializeHuffmanTree(bit_topology, it, bit_val, iv, size_val);
		node->next[1] = unserializeHuffmanTree(bit_topology, it, bit_val, iv, size_val);
		return node;
	}
	else
	{
		assert(false);
		return NULL;
	}
}

pair<unsigned char, int> decode(Node* tree, vector<unsigned char>& bits_buf, size_t idx_start)
{
	assert(tree);
	assert(0 <= idx_start);

	for (size_t i = idx_start; i < bits_buf.size(); ++i)
	{
		// leaf
		if (tree->next[0] == NULL && tree->next[1] == NULL)
			return make_pair(tree->val, (int)(i - idx_start));

		int idx = bits_buf[i];
		assert(idx == 1 || idx == 0);

		// integrity check
		assert(tree->next[idx]);

		tree = tree->next[idx];
	}

	// integrity check
	// should never reach here
	assert(false);

	// not enough bits to decode next value
	return make_pair(-1, -1);
}

unsigned char predictedValue(vector<unsigned char>& byte_image, int idx, int width_image)
{
	assert(width_image < idx);

	unsigned char x1 = byte_image[static_cast<std::vector<unsigned char, std::allocator<unsigned char>>::size_type>(idx) - width_image - 1];
	unsigned char x2 = byte_image[static_cast<std::vector<unsigned char, std::allocator<unsigned char>>::size_type>(idx) - width_image];
	unsigned char x3 = byte_image[static_cast<std::vector<unsigned char, std::allocator<unsigned char>>::size_type>(idx) - 1];
	unsigned char px = 0;

	if (x1 <= x2 && x1 <= x3)
		px = x2 > x3 ? x2 : x3;
	else if (x1 >= x2 && x1 >= x3)
		px = x2 < x3 ? x2 : x3;
	else
	{
		px = x2 + x3 - x1;
		assert(0 <= px && px <= 255);
	}

	return px;
}

void XOR(vector<unsigned char>& byte_image, vector<unsigned char>& byte_rand_keystream)
{
	for (size_t i = 0; i < byte_image.size(); ++i)
		byte_image[i] ^= byte_rand_keystream[i];
}

void printHuffmanEncoder(vector< vector<unsigned char> >& h_encoder)
{
	printf("size = %Iu\n", h_encoder.size());

	for (size_t i = 0; i < h_encoder.size(); ++i)
	{
		printf("%Iu : ", i);
		for (size_t j = 0; j < h_encoder[i].size(); ++j)
			printf("%hhu", h_encoder[i][j]);
		printf("\n");
	}
}








