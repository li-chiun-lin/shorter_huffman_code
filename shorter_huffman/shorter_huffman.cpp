#include "../lib/lib.h"
#include "shorter_huffman.h"

# define REPORT 1
# define PRINT 1

# if 0
int encodeLabelMap(vector<unsigned char>& val_labelmap, int width_image,
	vector<int>& h_encoder, vector<unsigned char>& bits_h_encoded_labelmap, int& len_secret)
{
	int idx_h_encoded_labelmap = 0;

	for (int i = 1; i < width_image; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int t = val_labelmap[row + col];
			int idx_hc = h_encoder[t];
			int len_hc = (int)bits_hcode[idx_hc].size();

			for (int k = 0; k < len_hc; ++k)
				bits_h_encoded_labelmap[idx_h_encoded_labelmap++] = bits_hcode[idx_hc][k];

			if (t <= 2)
				len_secret += -len_hc;
			else
				len_secret += (t < 7 ? t + 1 : 8) - len_hc;
		}
	}

	return idx_h_encoded_labelmap;
}
# else
vector<unsigned char> encodeLabelMap_(vector< vector<int> >& encoder, vector<unsigned char>& val_labelmap)
{
	vector<unsigned char> bits_h_encoded_labelmap;

	for (size_t i = CNT_REF; i < val_labelmap.size(); ++i)
	{
		bits_h_encoded_labelmap.insert(bits_h_encoded_labelmap.end(), encoder[val_labelmap[i]].begin(), encoder[val_labelmap[i]].end());
	}

	return bits_h_encoded_labelmap;
}
# endif

void reportA_(vector<unsigned char>& val_labelmap, vector<int>& cnt_t, int width_image, int domain_hcode)
{
	fprintf(fptr_report, "A. Label Map Generation, show only first few.\n\t");

	// reference bytes
	for (int col = 0; col < CNT_REF; ++col)
		fprintf(fptr_report, "x ");

	// other rows
	for (int i = CNT_REF; i < 4096; ++i)
		fprintf(fptr_report, "%d%s", val_labelmap[i], i % 512 == 511 ? "\n\t" : " ");

	fprintf(fptr_report, "\n\tDistribution of t\n");
	fprintf(fptr_report, "\t%6s ", "t");
	for (int i = 0; i < domain_hcode; ++i)
		fprintf(fptr_report, "%6d ", i);
	fprintf(fptr_report, "\n\t%6s ", "freq");
	int sum = 0;
	for (int i = 0; i < domain_hcode; ++i)
	{
		fprintf(fptr_report, "%6d ", cnt_t[i]);
		sum += cnt_t[i];
	}
	fprintf(fptr_report, "\n\t%6s %d", "sum", sum);
	fprintf(fptr_report, "\n\n");
}

void reportC_(vector<int>& cnt_t, vector< vector<int> >& encoder,
	int len_capacity, int len_h_encoded_labelmap, int len_ref, int header1, int header2)
{
	fprintf(fptr_report, "C. Huffman Coding, total capacity = %d, \n\theader = %d + %d, length of LM = %d, LM = %d, ref = %d\n\textra-space = %d\n",
		len_capacity, header1, header2, LEN_LEN_H_ENCODED_LABELMAP, len_h_encoded_labelmap, len_ref, len_capacity - header1 - header2 - LEN_LEN_H_ENCODED_LABELMAP - len_h_encoded_labelmap - len_ref);

	fprintf(fptr_report, "%8s %8s %8s %8s %8s %8s %8s\n", "t", "freq", "code", "cap.", "c len", "p len", "delta");
	fprintf(fptr_report, "%8s------------------------------------------------------\n", "-");

	for (int t = 0; t < DOMAIN_HCODE; ++t)
	{
		int cap = t < 8 ? t + 1 : t;
		int len = (int)encoder[t].size();
		int pay = cap - len;
		int del = pay * cnt_t[t];

		fprintf(fptr_report, "%8d %8d ", t, cnt_t[t]);

		int j = 0;
		while (j < len)
		{
			fprintf(fptr_report, "%d", encoder[t][j]);
			++j;
		}

		while (j < 8)
		{
			fprintf(fptr_report, " ");
			++j;
		}

		fprintf(fptr_report, " %8d %8d %8d %8d\n",
			cap, len, pay, del);
	}

	fprintf(fptr_report, "\n");
}

void reportD_(vector<unsigned char>& bits_embed, int len_h_topology, int len_h_value, int len_h_labmap, int idx_prepare)
{
	fprintf(fptr_report, "D. Label Map Embedding\n\n");
	fprintf(fptr_report, "D1. Side Info, Huffman tree\n\t");

	fprintf(fptr_report, "\n\thuffman tree topology ");
	for (size_t i = 0; i < len_h_topology; ++i)
		fprintf(fptr_report, "%d", bits_embed[i]);

	fprintf(fptr_report, "\n\thuffman code value    ");
	for (size_t i = 0; i < len_h_value; ++i)
		fprintf(fptr_report, "%d%s", bits_embed[len_h_topology + i], i % LEN_T == LEN_T - 1 ? " " : "");

	fprintf(fptr_report, "\n\tlength of encoded LM  ");
	for (size_t i = 0; i < len_h_labmap; ++i)
		fprintf(fptr_report, "%d", bits_embed[len_h_topology + len_h_value + i]);

	fprintf(fptr_report, "\n\n\tprepare embeding bits %d \n\n", idx_prepare);
}

void reportD2_(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed, int offset)
{
	fprintf(fptr_report, "D2. Side Info, hEncode(LabelMap) LM bits, only show first few bits\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%d %s", bits_embed[offset + i], WRAP(i));
	fprintf(fptr_report, "\n\tprepare embeding %d / %d bits\n\n", idx_prepare, len_embed);
}

void reportD3_(vector<unsigned char>& byte_image, int idx_prepare, int len_embed, int cnt_ref)
{
	fprintf(fptr_report, "D3. reference value %d bytes\n\t", cnt_ref);

	for (int i = 0; i < cnt_ref; ++i)
	{
		fprintf(fptr_report, "%02X  ", byte_image[i]);
		for (int j = 0; j < 8; ++j)
			fprintf(fptr_report, "%d", byte2bits[byte_image[i]][j]);
		fprintf(fptr_report, "\n\t");
	}

	fprintf(fptr_report, "\n\tprepare embeding %d / %d bits\n\n", idx_prepare, len_embed);
}

void reportD4_(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed, int offset)
{
	fprintf(fptr_report, "D4. random secret bits, only show first few bits\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%d %s", bits_embed[offset + i], WRAP(i));
	fprintf(fptr_report, "\n\tprepare embeding %d / %d bits\n\n", idx_prepare, len_embed);
}

void reportD5_(vector<unsigned char>& byte_image, int idx_embed, int len_sideinfo)
{
	fprintf(fptr_report, "D5. embed into reference bytes\n\t");
	for (int col = 0; col < CNT_REF; ++col)
		fprintf(fptr_report, "%02X %s", byte_image[col], WRAP(col));
	fprintf(fptr_report, "\n\n\tembeding progress %d / %d bits \n\n", idx_embed, len_sideinfo);
}

void reportF_(vector<unsigned char>& byte_image, vector<unsigned char>& bit_extract, int idx_ex)
{
	fprintf(fptr_report, "F. Data Extraction and Image Recovery\n\n");
	fprintf(fptr_report, "F1. extract reference bytes\n\t");
	for (int col = 0; col < CNT_REF; ++col)
		fprintf(fptr_report, "%02X %s", byte_image[col], WRAP(col));
	fprintf(fptr_report, "\n\n\tin bits\n\t");
	for (int i = 0; i < CNT_REF * 8; ++i)
		fprintf(fptr_report, "%x %s", bit_extract[i], WRAP(i));
	fprintf(fptr_report, "\n\n\textract %d bits\n\n", idx_ex);
}

void reportF3_(Node* tree)
{
	fprintf(fptr_report, "F3. recover Huffman tree\n\t");

	vector< vector<int> > h_encoder = buildHuffmanEncoder(tree, DOMAIN_HCODE);

	for (int i = 0; i < DOMAIN_HCODE; ++i)
	{
		fprintf(fptr_report, "%2d ", i);

		for (int j = 0; j < h_encoder[i].size(); ++j)
			fprintf(fptr_report, "%d", h_encoder[i][j]);
		fprintf(fptr_report, "\n\t");
	}
}

// similar to A
void reportF5_(vector<unsigned char>& val_ex_labelmap, int idx_ex, int idx_ex_proceed)
{
	fprintf(fptr_report, "F5. recover Label Map, proceed %d / %d bits, show only first few.\n\t", idx_ex_proceed, idx_ex);

	// reference bytes
	for (int col = 0; col < CNT_REF; ++col)
		fprintf(fptr_report, "x ");

	// other rows
	for (int i = CNT_REF; i < 4096; ++i)
		fprintf(fptr_report, "%d%s", val_ex_labelmap[i], i % 512 == 511 ? "\n\t" : " ");

	fprintf(fptr_report, "\n");
}

void reportF6_(vector<unsigned char>& byte_image, int idx_ex, int idx_ex_proceed)
{
	fprintf(fptr_report, "F6. recover reference bytes, proceed %d / %d bits\n\t", idx_ex_proceed, idx_ex);
	for (int col = 0; col < CNT_REF; ++col)
		fprintf(fptr_report, "%02X %s", byte_image[col], WRAP(col));
	fprintf(fptr_report, "\n\n");
}

void reportF7_(vector<unsigned char>& bit_secret_extra, int idx_ex, int idx_ex_proceed)
{
	fprintf(fptr_report, "F7. recover secret %d / %d bits, show only first few.\n\t", idx_ex_proceed, idx_ex);
	for (size_t i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%2d %s", bit_secret_extra[i], WRAP(i));
	fprintf(fptr_report, "\n");
}


# if 0
int embedMain(vector<unsigned char>& val_labelmap, vector<unsigned char>& byte_image, vector<unsigned char>& bits_embed, int width_image, int idx_embed)
{
	for (int i = 1; i < width_image; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int idx = row + col;
			int t = val_labelmap[idx];

			if (t <= 2)
			{
				//skip
			}
			else if (t >= 7)
			{
				byte_image[idx] = bits2byte(bits_embed, idx_embed);
				idx_embed += 8;
			}
			else
			{
				byte_image[idx] = bits2byte(bits_embed, idx_embed, t + 1) | (byte_image[idx] & mask_lsb[t]);
				idx_embed += t + 1;
			}
		}
	}

	return idx_embed;
}
# else
int embedMain_(vector<unsigned char>& val_labelmap, vector<unsigned char>& byte_image, vector<unsigned char>& bits_embed, int idx_embed)
{
	for (size_t i = CNT_REF; i < val_labelmap.size(); ++i)
	{
		int t = val_labelmap[i];

		if (t >= 7)
		{
			byte_image[i] = bits2byte(bits_embed, idx_embed);
			idx_embed += 8;
		}
		else
		{
			byte_image[i] = bits2byte(bits_embed, idx_embed, t + 1) | (byte_image[i] & mask_lsb[t]);
			idx_embed += t + 1;
		}
	}

	return idx_embed;
}
# endif

# if 0
void restoreImage(vector<unsigned char>& byte_image, vector<unsigned char>& val_ex_labelmap, int width_image)
{
	for (int row = 1; row < width_image; ++row)
	{
		int rw = row * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int idx = rw + col;
			int t = val_ex_labelmap[idx];

			unsigned char px = predictedValue(byte_image, idx, width_image);

			if (t <= 2)
				; // skip
			else if (t < 8)
				byte_image[idx] = ((px ^ mask_bit[t]) & mask_msb[t]) | (byte_image[idx] & mask_lsb[t]);
			else
				byte_image[idx] = px;
		}
	}
}
# else
void restoreImage(vector<unsigned char>& byte_image, vector<unsigned char>& val_ex_labelmap, int width_image)
{
	// first row
	for (int idx = CNT_REF; idx < width_image; ++idx)
	{
		int t = val_ex_labelmap[idx];

		if (t < 8)
			byte_image[idx] = ((byte_image[idx - 1] ^ mask_bit[t]) & mask_msb[t]) | (byte_image[idx] & mask_lsb[t]);
		else
			byte_image[idx] = byte_image[idx - 1];
	}

	int r1 = 0;
	int rw = width_image;

	for (int row = 1; row < width_image; ++row)
	{
		// first column
		int t = val_ex_labelmap[rw];

		if (t < 8)
			byte_image[rw] = ((byte_image[r1] ^ mask_bit[t]) & mask_msb[t]) | (byte_image[rw] & mask_lsb[t]);
		else
			byte_image[rw] = byte_image[r1];



		//----------------------------------

		for (int col = 1; col < width_image; ++col)
		{
			int idx = rw + col;
			t = val_ex_labelmap[idx];

			unsigned char px = predictedValue(byte_image, idx, width_image);

			if (t < 8)
				byte_image[idx] = ((px ^ mask_bit[t]) & mask_msb[t]) | (byte_image[idx] & mask_lsb[t]);
			else
				byte_image[idx] = px;
		}

		r1 = rw;
		rw += width_image;
	}
}
# endif

int genLabelMap_(vector<unsigned char>& byte_image, int width_image, vector<int>& cnt_t, vector<unsigned char>& val_labelmap)
{
	int len_embed = 0;

	// first row
	for (int col = CNT_REF; col < width_image; ++col)
	{
		val_labelmap[col] = pixel2t[byte_image[col - 1] ^ byte_image[col]];
		++cnt_t[val_labelmap[col]];

		len_embed += val_labelmap[col] < 8 ? val_labelmap[col] + 1 : val_labelmap[col];
	}

	int r1 = 0;
	int rw = width_image;

	for (int row = 1; row < width_image; ++row)
	{
		// first column
		val_labelmap[rw] = pixel2t[byte_image[r1] ^ byte_image[rw]];
		++cnt_t[val_labelmap[rw]];

		len_embed += val_labelmap[rw] < 8 ? val_labelmap[rw] + 1 : val_labelmap[rw];

		//int rw = row * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int idx = rw + col;
			unsigned char px = predictedValue(byte_image, idx, width_image);

			// this pixel can embed (t + 1) bits.
			val_labelmap[idx] = pixel2t[byte_image[idx] ^ px];
			++cnt_t[val_labelmap[idx]];

			len_embed += val_labelmap[idx] < 8 ? val_labelmap[idx] + 1 : val_labelmap[idx];
		}

		r1 = rw;
		rw += width_image;
	}

	return len_embed;
}



int main()
{
	vector<unsigned char> byte_image = readImage(PATH_COVER_IMG);
	const int size_image = (int)byte_image.size();
	const int width_image = (int)sqrt(size_image);
	const int width_image1 = width_image + 1;
	const int width_image_1 = width_image - 1;

	srand(0);

# if REPORT
	reportBegin(width_image, size_image, PATH_REPORT, PATH_COVER_IMG);
# endif

# if PRINT
	printByteToFile(byte_image, width_image, PATH_OG_IMG_HD);
# endif

	//------------------------------------------------------
	// A Label Map Generation
	const int len_ref = CNT_REF * 8;

	vector<int> cnt_t(DOMAIN_HCODE);
	vector<unsigned char> val_labelmap(size_image);
	// carefull with this len_ref
	const int len_embed = genLabelMap_(byte_image, width_image, cnt_t, val_labelmap) + len_ref;

# if REPORT
	reportA_(val_labelmap, cnt_t, width_image, DOMAIN_HCODE);
# endif

	//------------------------------------------------------
	// B Image Encryption
	vector<unsigned char> byte_rand_keystream(size_image);
	generate(byte_rand_keystream.begin(), byte_rand_keystream.end(), rand);

# if REPORT
	reportB(byte_image, byte_rand_keystream);
# endif

	XOR(byte_image, byte_rand_keystream);

	writeImage(PATH_ENCRY_IMG, byte_image);

# if REPORT
	reportB3(byte_image, PATH_ENCRY_IMG);
# endif



	//------------------------------------------------------
	// C Huffman Coding  

# if 0
	vector<int> h_encoder(DOMAIN_HCODE);
	int len_h_encoded_labelmap = buildHuffmanEncoder(h_encoder, cnt_t);

	int len_secret = 0;
	vector<unsigned char> bits_h_encoded_labelmap(len_h_encoded_labelmap);
	int idx_h_encoded_labelmap = encodeLabelMap(val_labelmap, width_image, h_encoder,
		bits_h_encoded_labelmap, len_secret);

	assert(idx_h_encoded_labelmap == len_h_encoded_labelmap);
# else
	Node* tree = buildHuffmanTree(cnt_t);
	vector< vector<int> > h_encoder = buildHuffmanEncoder(tree, DOMAIN_HCODE);
	vector<unsigned char> bit_topology;
	vector<unsigned char> bit_val;
	serializeHuffmanTree(tree, bit_topology, bit_val, LEN_T);

	del(tree);

	assert(bit_topology.size() == LEN_H_TOPOLOGY);
	assert(bit_val.size() == LEN_H_VALUE);

	vector<unsigned char> bits_h_encoded_labelmap = encodeLabelMap_(h_encoder, val_labelmap);

	const int len_hcode_header = (int)bit_topology.size() + (int)bit_val.size();
	const int len_h_encoded_labelmap = (int)bits_h_encoded_labelmap.size();

	

	const int len_sideinfo = len_hcode_header + LEN_LEN_H_ENCODED_LABELMAP + len_h_encoded_labelmap + len_ref;

	const int len_secret = len_embed - len_sideinfo;
# endif


#
# if REPORT
	reportC_(cnt_t, h_encoder, len_embed, len_h_encoded_labelmap, len_ref, (int)bit_topology.size(), (int)bit_val.size());
# endif
# 


	//------------------------------------------------------
	// D Label Map Embedding
# if 0
	const int len_sideinfo = LEN_HCODE_HEADER + len_h_encoded_labelmap;
	const int len_ref = (width_image + width_image_1) * 8;
	const int len_auxinfo = len_sideinfo + len_ref;
	const int len_embed = len_auxinfo + (len_secret -= LEN_HCODE_HEADER);

	vector<unsigned char> bits_rand_secret(len_secret);
	generate(bits_rand_secret.begin(), bits_rand_secret.end(), rand);

	vector<unsigned char> bits_embed(len_embed);
	int idx_prepare = 0;
# else
	vector<unsigned char> bits_embed(len_embed);
	int idx_prepare = 0;
# endif

# if 0
	//side info 1, Huffman coding rule - 0 to 8 
	for (int t = 0; t < DOMAIN_HCODE; ++t)
	{
		int idx = h_encoder[t];
		int len = (int)bits_hcode[idx].size();

		for (int j = 0; j < len; ++j)
			bits_embed[idx_prepare++] = bits_hcode[idx][j];
	}

# else
	for (size_t i = 0; i < bit_topology.size(); ++i)
		bits_embed[idx_prepare++] = bit_topology[i];

	for (size_t i = 0; i < bit_val.size(); ++i)
		bits_embed[idx_prepare++] = bit_val[i];

	unsigned int mask_bit_long = 1 << (LEN_LEN_H_ENCODED_LABELMAP - 1);

	while (mask_bit_long)
	{
		bits_embed[idx_prepare++] = (len_h_encoded_labelmap & mask_bit_long) ? 1 : 0;
		mask_bit_long >>= 1;
	}

	assert(idx_prepare == len_hcode_header + LEN_LEN_H_ENCODED_LABELMAP);

# endif

# if REPORT
	reportD_(bits_embed, LEN_H_TOPOLOGY, LEN_H_VALUE, LEN_LEN_H_ENCODED_LABELMAP, idx_prepare);
# endif

# if 0
	//side info 2, length of encode(lab map) => lm, 20 bits
	unsigned int mask_bit_long = 1 << (LEN_LEN_H_ENCODED_LABELMAP - 1);

	while (mask_bit_long)
	{
		bits_embed[idx_prepare++] = (len_h_encoded_labelmap & mask_bit_long) ? 1 : 0;
		mask_bit_long >>= 1;
	}
# if REPORT
	reportD2(bits_embed, idx_prepare, len_embed, LEN_LEN_H_ENCODED_LABELMAP, LEN_HC_RULE);
# endif
# endif



# if 0
	//side info 3, encode(lab map) => lm bits
	for (int i = 0; i < len_h_encoded_labelmap; ++i)
		bits_embed[idx_prepare++] = bits_h_encoded_labelmap[i];

	assert(idx_prepare == len_sideinfo);
# else
	for (int i = 0; i < len_h_encoded_labelmap; ++i)
		bits_embed[idx_prepare++] = bits_h_encoded_labelmap[i];

	assert(idx_prepare == len_hcode_header + LEN_LEN_H_ENCODED_LABELMAP + len_h_encoded_labelmap);
# endif

# if REPORT
	reportD2_(bits_embed, idx_prepare, len_embed, len_hcode_header + LEN_LEN_H_ENCODED_LABELMAP);
# endif

# if 0
	// reference values
	// first row
	for (int col = 0; col < width_image; ++col)
		for (int j = 0; j < 8; ++j)
			bits_embed[idx_prepare++] = byte2bits[byte_image[col]][j];
# else
	for (int i = 0; i < CNT_REF; ++i)
		for (int j = 0; j < 8; ++j)
			bits_embed[idx_prepare++] = byte2bits[byte_image[i]][j];

	assert(idx_prepare == len_hcode_header + LEN_LEN_H_ENCODED_LABELMAP + len_h_encoded_labelmap + len_ref);
# endif

# if REPORT
	reportD3_(byte_image, idx_prepare, len_embed, CNT_REF);
# endif

# if 0
	// first col, but except (0, 0)
	for (int row = 1; row < width_image; ++row)
		for (int j = 0; j < 8; ++j)
			bits_embed[idx_prepare++] = byte2bits[(unsigned char)byte_image[row * width_image]][j];

	assert(idx_prepare == len_auxinfo);

# if REPORT
	reportD5(byte_image, idx_prepare, len_embed, width_image);
# endif
# endif


# if 0
	for (int i = 0; i < len_secret; ++i)
	{
		bits_embed[idx_prepare] = bits_rand_secret[i] & 0x01;
		++idx_prepare;
	}

	assert(idx_prepare == len_embed);
# else
	vector<unsigned char> bits_rand_secret(len_secret);
	generate(bits_rand_secret.begin(), bits_rand_secret.end(), rand);

	for (int i = 0; i < len_secret; ++i)
	{
		bits_embed[idx_prepare] = bits_rand_secret[i] & 0x01;
		++idx_prepare;
	}

	assert(idx_prepare == len_embed);
# endif

# if REPORT
	reportD4_(bits_embed, idx_prepare, len_embed, len_sideinfo);
# endif

# if 0
	// embed auxiliary info into 1st row and 1st col, 8 bits * 1023
	int idx_embed = 0;
	for (int col = 0; col < width_image; ++col)
	{
		byte_image[col] = bits2byte(bits_embed, idx_embed);
		idx_embed += 8;
	}
# else
	int idx_embed = 0;
	for (int col = 0; col < CNT_REF; ++col)
	{
		byte_image[col] = bits2byte(bits_embed, idx_embed);
		idx_embed += 8;
	}
# endif

# if REPORT
	reportD5_(byte_image, idx_embed, len_sideinfo);
# endif

# if 0
	// first col, but except (0, 0)
	for (int row = 1; row < width_image; ++row)
	{
		byte_image[row * width_image] = bits2byte(bits_embed, idx_embed);
		idx_embed += 8;
	}

# if REPORT
	reportD8(byte_image, idx_embed, width_image, len_auxinfo);
# endif
# endif


# if REPORT
	fprintf(fptr_report, "D6. embed into rest of the enc(image)\n");
# endif

# if 0
	idx_embed = embedMain(val_labelmap, byte_image, bits_embed, width_image, idx_embed);

	assert(idx_embed == idx_prepare);
# else
	idx_embed = embedMain_(val_labelmap, byte_image, bits_embed, idx_embed);

	assert(idx_embed == idx_prepare);
# endif

	writeImage(PATH_STEGO_IMG, byte_image);

# if REPORT
	fprintf(fptr_report, "\nE. write stego image to %s\n\n", PATH_STEGO_IMG);
# endif












	vector<unsigned char> bit_extract(size_image * 8);
	int idx_ex = 0;


# if 0
	// first row
	for (int col = 0; col < width_image; ++col)
		for (int j = 0; j < 8; ++j)
			bit_extract[idx_ex++] = byte2bits[byte_image[col]][j];
# else
	// reference bytes
	for (int col = 0; col < CNT_REF; ++col)
		for (int j = 0; j < 8; ++j)
			bit_extract[idx_ex++] = byte2bits[byte_image[col]][j];
# endif

# if REPORT
	reportF_(byte_image, bit_extract, idx_ex);
# endif


# if 0
	// first col, but except (0, 0)
	for (int row = 1; row < width_image; ++row)
		for (int j = 0; j < 8; ++j)
			bit_extract[idx_ex++] = byte2bits[(unsigned char)byte_image[row * width_image]][j];

# if REPORT
	reportF2(byte_image, idx_ex, width_image);
# endif
# endif


# if 0
# if REPORT
	fprintf(fptr_report, "F3. recover Huffman coding rules\n");
# endif

	struct Trie* h_decoder = (struct Trie*)calloc(1, sizeof(struct Trie));
	for (int i = 0; i < RANGE_HCODE; ++i)
		build(h_decoder, bits_hcode[i]);
# else
	vector<unsigned char> bit_topology_ex(bit_extract.begin(), bit_extract.begin() + LEN_H_TOPOLOGY);
	vector<unsigned char> bit_val_ex(bit_extract.begin() + LEN_H_TOPOLOGY, bit_extract.begin() + LEN_H_TOPOLOGY + LEN_H_VALUE);
	int it = 0;
	int iv = 0;
	Node* tree_ex = unserializeHuffmanTree(bit_topology_ex, it, bit_val_ex, iv, LEN_T);

	assert(it == LEN_H_TOPOLOGY);
	assert(iv == LEN_H_VALUE);

# if REPORT
	reportF3_(tree_ex);
# endif
# endif

	int idx_ex_proceed = it + iv;

# if 0
	
	for (int i = 0; i < RANGE_HCODE; ++i)
	{
		auto p = decode(h_decoder, bit_extract, idx_ex_proceed);
		assert(p.first != -1);
		assert(p.second);

		int len_cur_hcode = p.second;

		assignValue(h_decoder, bit_extract, idx_ex_proceed, idx_ex_proceed + len_cur_hcode, i);

# if REPORT
		fprintf(fptr_report, "\tproceed %2d bits\t %d : ", idx_ex_proceed, i);
		for (int i = 0; i < len_cur_hcode; ++i)
			fprintf(fptr_report, "%d ", bit_extract[idx_ex_proceed + i]);
		fprintf(fptr_report, "\n");
# endif

		idx_ex_proceed += len_cur_hcode;
	}

	assert(idx_ex_proceed == LEN_HC_RULE);

# endif

# if 1

	int len_ex_h_encoded_labelmap = 0;

	for (int i = 0; i < LEN_LEN_H_ENCODED_LABELMAP; ++i)
	{
		len_ex_h_encoded_labelmap <<= 1;
		len_ex_h_encoded_labelmap |= bit_extract[idx_ex_proceed++];
	}

	assert(len_ex_h_encoded_labelmap == len_h_encoded_labelmap);

# if REPORT
	fprintf(fptr_report, "\nF4. recover length of hEncode(label map) = %d, proceed %d bits\n\n",
		len_ex_h_encoded_labelmap, idx_ex_proceed);
# endif
# endif

# if 0
	vector<unsigned char> val_ex_labelmap(size_image);
	int idx_row_ex_lm = 1;
	int idx_col_ex_lm = 1;

	// re-construct lable map
	while (idx_row_ex_lm < width_image)
	{
		pair<unsigned char, int> p = decode(h_decoder, bit_extract, idx_ex_proceed);

		assert(0 <= p.first && p.first <= MAX_T);
		assert(p.second);

		unsigned char t = p.first;
		int idx_ex_lm = idx_row_ex_lm * width_image + idx_col_ex_lm;
		val_ex_labelmap[idx_ex_lm] = p.first;

		assert(val_ex_labelmap[idx_ex_lm] == val_labelmap[idx_ex_lm]);

		for (int j = 0; j <= t && j < MAX_T; ++j)
			bit_extract[idx_ex++] = ((byte_image[idx_ex_lm] << j) & 0x80) ? 1 : 0;

		idx_ex_proceed += p.second;

		if (++idx_col_ex_lm == width_image)
		{
			idx_col_ex_lm = 1;
			++idx_row_ex_lm;
		}
	}

	del(h_decoder);
# else
	vector<unsigned char> val_ex_labelmap(size_image);
	int idx_ex_lm = CNT_REF;

	// re-construct lable map
	while (idx_ex_lm < val_ex_labelmap.size())
	{
		pair<unsigned char, int> p = decode(tree_ex, bit_extract, idx_ex_proceed);

		assert(0 <= p.first && p.first <= MAX_T);
		assert(p.second);

		unsigned char t = p.first;
		val_ex_labelmap[idx_ex_lm] = t;

		assert(val_ex_labelmap[idx_ex_lm] == val_labelmap[idx_ex_lm]);

		for (int j = 0; j <= t && j < MAX_T; ++j)
			bit_extract[idx_ex++] = ((byte_image[idx_ex_lm] << j) & 0x80) ? 1 : 0;

		idx_ex_proceed += p.second;

		++idx_ex_lm;
	}
# endif

# if REPORT
	reportF5_(val_ex_labelmap, idx_ex, idx_ex_proceed);
# endif

# if 0
	for (int col = 0; col < width_image; ++col)
	{
		byte_image[col] = bits2byte(bit_extract, idx_ex_proceed);
		idx_ex_proceed += 8;
	}
# else
	for (int col = 0; col < CNT_REF; ++col)
	{
		byte_image[col] = bits2byte(bit_extract, idx_ex_proceed);
		idx_ex_proceed += 8;
	}
# endif

# if REPORT
	reportF6_(byte_image, idx_ex, idx_ex_proceed);
# endif


# if 0
	for (int row = 1; row < width_image; ++row)
	{
		byte_image[row * width_image] = bits2byte(bit_extract, idx_ex_proceed);
		idx_ex_proceed += 8;
	}

# if REPORT
	reportF7(byte_image, idx_ex, idx_ex_proceed, width_image);
# endif
# endif


# if 1
	vector<unsigned char> bit_secret_extra(len_secret);

	for (size_t i = 0; i < bit_secret_extra.size(); ++i)
		bit_secret_extra[i] = bit_extract[idx_ex_proceed++];
# endif

# if REPORT
	reportF7_(bit_secret_extra, idx_ex, idx_ex_proceed);
# endif

# if 1
# if REPORT
	reportG(byte_image, byte_rand_keystream);
# endif

	XOR(byte_image, byte_rand_keystream);

# if REPORT
	reportG3(byte_image);
# endif
# endif

	//-------------------------------------------------------------------

# if 1
	restoreImage(byte_image, val_ex_labelmap, width_image);

	vector<unsigned char> byte_orig = readImage(PATH_COVER_IMG);

	for (int i = 0; i < size_image; ++i)
		assert(byte_image[i] == byte_orig[i]);

# endif

# if REPORT
	reportG4(byte_image, PATH_RESTO_IMG);
# endif

	writeImage(PATH_RESTO_IMG, byte_image);

# if PRINT
	printByteToFile(byte_image, width_image, PATH_RE_IMG_HD);
# endif


# if REPORT
	reportEnd();
# endif

	//system("pause");

	return 0;
}

