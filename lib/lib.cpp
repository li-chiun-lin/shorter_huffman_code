// lib.cpp : 定義靜態程式庫的函式。
//

#include "pch.h"
#include "framework.h"


#include "lib.h"

using namespace std;

// TODO: 此為程式庫函式的範例


errno_t printByteToFile(vector<unsigned char>& byte_buf, int len, string file_name)
{
	FILE* fptr = NULL;
	errno_t err = fopen_s(&fptr, file_name.c_str(), "w");

	if (err || fptr == NULL)
		return err;

	for (int i = 0; i < len; ++i)
	{
		int r = i * len;
		for (int j = 0; j < len; ++j)
			fprintf(fptr, "%02X ", byte_buf[r + j]);
		fprintf(fptr, "\n");
	}

	fclose(fptr);
	return 0;
}

vector<unsigned char> readImage(string file_name)
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

	for (int i = 0; i < len; ++i)
	{
		byte_ret <<= 1;
		byte_ret |= bit_buf[idx + i];
	}

	return byte_ret << (8 - len);
}

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
			return make_pair(coder->val_t, i - idx_start);

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

unsigned char predictedValue(vector<unsigned char>& byte_image, int idx, int width_image)
{
	unsigned char x1 = byte_image[idx - width_image - 1];
	unsigned char x2 = byte_image[idx - width_image];
	unsigned char x3 = byte_image[idx - 1];
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

vector<unsigned char> genLabelMap(vector<unsigned char>& byte_image,
	int size_image, int width_image, vector<int>& cnt_t)
{
	vector<unsigned char> val_labelmap(size_image);

	// leave first row and first column as ref_value.
	for (int i = 1; i < width_image; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int idx = row + col;
			//assert(width_image1 <= idx && idx < size_image);

			unsigned char px = predictedValue(byte_image, idx, width_image);

			// this pixel can embed (t + 1) bits.
			val_labelmap[idx] = pixel2t[byte_image[idx] ^ px];
			++cnt_t[val_labelmap[idx]];
		}
	}

	return val_labelmap;
}

void XOR(vector<unsigned char>& byte_image, vector<unsigned char>& byte_rand_keystream)
{
	for (size_t i = 0; i < byte_image.size(); ++i)
		byte_image[i] ^= byte_rand_keystream[i];
}






