#include "../lib/lib.h"
#include "msb_huffman.h"

# define REPORT 1
# define PRINT 1

int buildHuffmanEncoder(vector<int>& h_encoder, vector<int>& cnt_t)
{
	priority_queue<pair<int, int>> pq;
	int len_h_encoded_labelmap = 0;

	for (int t = 0; t < DOMAIN_HCODE; ++t)
		pq.push(make_pair(cnt_t[t], t));

	for (int i = 0; i < DOMAIN_HCODE; ++i)
	{
		pair<int, int> p = pq.top();
		pq.pop();
		h_encoder[p.second] = i;
		len_h_encoded_labelmap += p.first * (int)bits_hcode[i].size();
	}

	return len_h_encoded_labelmap;
}

int encodeLabelMap(vector<unsigned char>& val_labelmap, int width_image,
	vector<int>& h_encoder, vector<unsigned char>& bits_h_encoded_labelmap, int& len_secret)
{
	int idx_h_encoded_labelmap = 0;

	for (int row = 1; row < width_image; ++row)
	{
		int rw = row * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int t = val_labelmap[rw + col];
			int idx_hc = h_encoder[t];
			int len_hc = (int)bits_hcode[idx_hc].size();

			for (int k = 0; k < len_hc; ++k)
				bits_h_encoded_labelmap[idx_h_encoded_labelmap++] = bits_hcode[idx_hc][k];

			len_secret += (t < 7 ? t + 1 : 8) - len_hc;
		}
	}

	return idx_h_encoded_labelmap;
}

# if 1
void reportC(vector<int>& h_encoder, vector<int>& cnt_t, int len_h_encoded_labelmap, int len_secret, int len_hcode_header)
{
	fprintf(fptr_report, "C. Huffman Coding, %d, LM = %d, extra-capacity = %d = %d + %d\n",
		len_h_encoded_labelmap + len_secret, len_h_encoded_labelmap, len_secret, len_secret - len_hcode_header, len_hcode_header);

	// header
	fprintf(fptr_report, "%8s %8s %8s %8s %8s %8s %8s\n", "t", "dist", "code", "cap.", "c len", "p len", "delta");
	fprintf(fptr_report, "%8s------------------------------------------------------\n", "-");

	for (int t = 0; t < DOMAIN_HCODE; ++t)
	{
		int cap = t < 8 ? t + 1 : t;
		int len = (int)bits_hcode[h_encoder[t]].size();
		int pay = cap - len;
		int del = pay * cnt_t[t];

		fprintf(fptr_report, "%8d %8d %8X %8d %8d %8d %8d\n",
			t, cnt_t[t], byte_hcode[h_encoder[t]], cap, len, pay, del);
	}

	fprintf(fptr_report, "\n\n");
}
# else
void reportC(vector<int>& h_encoder, vector<int>& cnt_t, int len_h_encoded_labelmap, int len_secret, int len_hcode_header)
{
	fprintf(fptr_report, "C. Huffman Coding, %d, LM = %d, extra-capacity = %d = %d + %d\n",
		len_h_encoded_labelmap + len_secret, len_h_encoded_labelmap, len_secret, len_secret - len_hcode_header, len_hcode_header);

	fprintf(fptr_report, "%8s ", "t");
	for (int i = 0; i < SIZE_HCODE; ++i)
		fprintf(fptr_report, "%8d ", i);

	fprintf(fptr_report, "\n%8s ", "dist");
	for (int i = 0; i < SIZE_HCODE; ++i)
		fprintf(fptr_report, "%8d ", cnt_t[i]);

	fprintf(fptr_report, "\n%8s ", "code");
	for (int i = 0; i < SIZE_HCODE; ++i)
		fprintf(fptr_report, "%8X ", byte_hcode[h_encoder[i]]);

	fprintf(fptr_report, "\n%8s ", "len");
	for (int i = 0; i < SIZE_HCODE; ++i)
		fprintf(fptr_report, "%8d ", bits_hcode[h_encoder[i]].size());

	vector<int> delta(SIZE_HCODE);
	for (int i = 0; i < 8; ++i)
		delta[i] = i + 1 - bits_hcode[h_encoder[i]].size();
	delta[8] = 8 - bits_hcode[h_encoder[8]].size();

	fprintf(fptr_report, "\n%8s ", "delta");
	for (int i = 0; i < SIZE_HCODE; ++i)
		fprintf(fptr_report, "%+8d ", delta[i]);

	int sum = 0;
	fprintf(fptr_report, "\n%8s ", "total");
	for (int i = 0; i < SIZE_HCODE; ++i)
	{
		fprintf(fptr_report, "%+8d ", delta[i] * cnt_t[i]);
		sum += delta[i] * cnt_t[i];
	}

	fprintf(fptr_report, "\n%8s = %d", "sum", sum);
	fprintf(fptr_report, "\n\n");
}
# endif

int embedMain(vector<unsigned char>& val_labelmap, vector<unsigned char>& byte_image,
		vector<unsigned char>& bits_embed, int width_image, int idx_embed)
{
	for (int row = 1; row < width_image; ++row)
	{
		int rw = row * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int idx = rw + col;
			int t = val_labelmap[idx];

			if (t >= 7)
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

# if 0
		fprintf(fptr_report, "\t(%3d) progress %7d / %7d / %7d bits %s\n",
			i, idx_embed, len_auxinfo, len_embed, (printmark && idx_embed >= len_auxinfo) ? "*" : "");

		if (printmark && idx_embed >= len_auxinfo)
			printmark = false;
# endif
	}

	return idx_embed;
}

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

			if (t < 8)
				byte_image[idx] = ((px ^ mask_bit[t]) & mask_msb[t]) | (byte_image[idx] & mask_lsb[t]);
			else
				byte_image[idx] = px;
		}
	}
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
	vector<int> cnt_t(DOMAIN_HCODE);
	vector<unsigned char> val_labelmap =
		genLabelMap(byte_image, size_image, width_image, cnt_t);

# if REPORT
	reportA(val_labelmap, cnt_t, width_image, DOMAIN_HCODE);
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

	vector<int> h_encoder(DOMAIN_HCODE);
	int len_h_encoded_labelmap = buildHuffmanEncoder(h_encoder, cnt_t);

	int len_secret = 0;
	vector<unsigned char> bits_h_encoded_labelmap(len_h_encoded_labelmap);
	int idx_h_encoded_labelmap = encodeLabelMap(val_labelmap, width_image, h_encoder,
		bits_h_encoded_labelmap, len_secret);

	assert(idx_h_encoded_labelmap == len_h_encoded_labelmap);

# if REPORT
	reportC(h_encoder, cnt_t, len_h_encoded_labelmap, len_secret, LEN_HCODE_HEADER);
# endif





	//------------------------------------------------------
	// D Label Map Embedding

	const int len_sideinfo = LEN_HCODE_HEADER + len_h_encoded_labelmap;
	const int len_ref = (width_image + width_image_1) * 8;
	const int len_auxinfo = len_sideinfo + len_ref;
	const int len_embed = len_auxinfo + (len_secret -= LEN_HCODE_HEADER);

	vector<unsigned char> bits_rand_secret(len_secret);
	generate(bits_rand_secret.begin(), bits_rand_secret.end(), rand);

	vector<unsigned char> bits_embed(len_embed);
	int idx_prepare = 0;

	//side info 1, Huffman coding rule - 0 to 8 
	for (int t = 0; t < DOMAIN_HCODE; ++t)
	{
		int idx = h_encoder[t];
		int len = (int)bits_hcode[idx].size();

		for (int j = 0; j < len; ++j)
			bits_embed[idx_prepare++] = bits_hcode[idx][j];
	}

# if REPORT
	reportD(bits_embed, idx_prepare, LEN_HC_RULE);
# endif

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

	//side info 3, encode(lab map) => lm bits
	for (int i = 0; i < len_h_encoded_labelmap; ++i)
		bits_embed[idx_prepare++] = bits_h_encoded_labelmap[i];

	assert(idx_prepare == len_sideinfo);

# if REPORT
	reportD3(bits_embed, idx_prepare, len_embed, LEN_HCODE_HEADER);
# endif

	// reference values
	// first row
	for (int col = 0; col < width_image; ++col)
		for (int j = 0; j < 8; ++j)
			bits_embed[idx_prepare++] = byte2bits[byte_image[col]][j];

# if REPORT
	reportD4(byte_image, idx_prepare, len_embed, width_image);
# endif

	// first col, but except (0, 0)
	for (int row = 1; row < width_image; ++row)
		for (int j = 0; j < 8; ++j)
			bits_embed[idx_prepare++] = byte2bits[(unsigned char)byte_image[row * width_image]][j];

	assert(idx_prepare == len_auxinfo);

# if REPORT
	reportD5(byte_image, idx_prepare, len_embed, width_image);
# endif

	for (int i = 0; i < len_secret; ++i)
		bits_embed[idx_prepare ++] = bits_rand_secret[i] & 0x01;

	assert(idx_prepare == len_embed);

# if REPORT
	reportD6(bits_embed, idx_prepare, len_embed, len_auxinfo);
# endif

	// embed auxiliary info into 1st row and 1st col, 8 bits * 1023
	int idx_embed = 0;
	for (int col = 0; col < width_image; ++col)
	{
		byte_image[col] = bits2byte(bits_embed, idx_embed);
		idx_embed += 8;
	}

# if REPORT
	reportD7(byte_image, idx_embed, width_image, len_auxinfo);
# endif

	// first col, but except (0, 0)
	for (int row = 1; row < width_image; ++row)
	{
		byte_image[row * width_image] = bits2byte(bits_embed, idx_embed);
		idx_embed += 8;
	}

# if REPORT
	reportD8(byte_image, idx_embed, width_image, len_auxinfo);
# endif




# if REPORT
	fprintf(fptr_report, "D9. embed into rest of the enc(image)\n");
	bool printmark = true;
# endif

	idx_embed = embedMain(val_labelmap, byte_image, bits_embed, width_image, idx_embed);

	assert(idx_embed == idx_prepare);

	writeImage(PATH_STEGO_IMG, byte_image);

# if REPORT
	fprintf(fptr_report, "\nE. write stego image to %s\n\n", PATH_STEGO_IMG);
# endif











	vector<unsigned char> bit_extract(size_image * 8);
	int idx_ex = 0;

	// first row
	for (int col = 0; col < width_image; ++col)
		for (int j = 0; j < 8; ++j)
			bit_extract[idx_ex++] = byte2bits[byte_image[col]][j];

# if REPORT
	reportF(byte_image, bit_extract, idx_ex, width_image);
# endif

	// first col, but except (0, 0)
	for (int row = 1; row < width_image; ++row)
		for (int j = 0; j < 8; ++j)
			bit_extract[idx_ex++] = byte2bits[(unsigned char)byte_image[row * width_image]][j];

# if REPORT
	reportF2(byte_image, idx_ex, width_image);
# endif

# if REPORT
	fprintf(fptr_report, "F3. recover Huffman coding rules\n");
# endif

	struct Trie* h_decoder = (struct Trie*)calloc(1, sizeof(struct Trie));
	for (int i = 0; i < RANGE_HCODE; ++i)
		build(h_decoder, bits_hcode[i]);

	int idx_ex_proceed = 0;
	for (int i = 0; i < RANGE_HCODE; ++i)
	{
		pair<unsigned char, int> p = decode(h_decoder, bit_extract, idx_ex_proceed);
		assert(p.first != -1 && p.second);

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
		val_ex_labelmap[idx_ex_lm] = t;

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

# if REPORT
	reportF5(val_ex_labelmap, idx_ex, idx_ex_proceed, width_image);
# endif

	for (int col = 0; col < width_image; ++col)
	{
		byte_image[col] = bits2byte(bit_extract, idx_ex_proceed);
		idx_ex_proceed += 8;
	}

# if REPORT
	reportF6(byte_image, idx_ex, idx_ex_proceed, width_image);
# endif

	for (int row = 1; row < width_image; ++row)
	{
		byte_image[row * width_image] = bits2byte(bit_extract, idx_ex_proceed);
		idx_ex_proceed += 8;
	}

# if REPORT
	reportF7(byte_image, idx_ex, idx_ex_proceed, width_image);
# endif

	vector<unsigned char> bit_secret_extra(len_secret);

	for (size_t i = 0; i < bit_secret_extra.size(); ++i)
		bit_secret_extra[i] = bit_extract[idx_ex_proceed++];

# if REPORT
	reportF8(bit_secret_extra, idx_ex, idx_ex_proceed);
# endif

# if REPORT
	reportG(byte_image, byte_rand_keystream);
# endif

	XOR(byte_image, byte_rand_keystream);

# if REPORT
	reportG3(byte_image);
# endif

	//-------------------------------------------------------------------

	restoreImage(byte_image, val_ex_labelmap, width_image);

	vector<unsigned char> byte_orig = readImage(PATH_COVER_IMG);

	for (int i = 0; i < size_image; ++i)
		assert(byte_image[i] == byte_orig[i]);

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

	return 0;
}

