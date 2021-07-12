#include "../lib/lib.h"
#include "msb_huffman.h"

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
			int t = val_labelmap[static_cast<std::vector<unsigned char, std::allocator<unsigned char>>::size_type>(rw) + col];
			int idx_hc = h_encoder[t];
			int len_hc = (int)bits_hcode[idx_hc].size();

			for (int k = 0; k < len_hc; ++k)
				bits_h_encoded_labelmap[idx_h_encoded_labelmap++] = bits_hcode[idx_hc][k];

			len_secret += (t < 7 ? t + 1 : 8) - len_hc;
		}
	}

	return idx_h_encoded_labelmap;
}

vector<unsigned char> genLabelMap(vector<unsigned char>& byte_image,
	int size_image, int width_image, vector<int>& cnt_t)
{
	vector<unsigned char> val_labelmap(size_image);

	// leave first row and first column as ref_value.
	for (int row = 1; row < width_image; ++row)
	{
		int rw = row * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int idx = rw + col;
			unsigned char px = predictedValue(byte_image, idx, width_image);

			// this pixel can embed (t + 1) bits.
			val_labelmap[idx] = pixel2t[byte_image[idx] ^ px];
			++cnt_t[val_labelmap[idx]];
		}
	}

	return val_labelmap;
}

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
				byte_image[idx] = bits2byte(bits_embed, idx_embed, t + 1) + (byte_image[idx] & mask_lsb[t]);
				idx_embed += t + 1;
			}
		}
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

struct Result msb_huffman(string path_cover_img, string path_report, string path_encry_img, string path_setgo_img, string path_resto_img, string path_ori_hex, string path_res_hex, unsigned int seed)
{
	vector<unsigned char> byte_image = readImageRaw(path_cover_img);
	const int size_image = (int)byte_image.size();
	
	cout << "msb " << path_cover_img << endl;
	//assert(size_image == 512 * 512);

	const int width_image = (int)sqrt(size_image);
	const int width_image1 = width_image + 1;
	const int width_image_1 = width_image - 1;
	const int len_len_h_encoded_labelmap = (int)ceil(log2(width_image_1 * width_image_1 * 5) - 1);
	const int len_hcode_header = LEN_HC_RULE + len_len_h_encoded_labelmap;

	srand(seed);


# if REPORT
	reportBegin(width_image, size_image, path_report.c_str(), path_cover_img.c_str());
# endif

# if PRINT
	if (!path_ori_hex.empty())
		printByteToFile(byte_image, width_image, path_ori_hex);
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

	if (!path_encry_img.empty())
		writeImage(path_encry_img, byte_image);

# if REPORT
	reportB3(byte_image, path_encry_img.c_str());
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
	reportC(h_encoder, cnt_t, len_h_encoded_labelmap, len_secret, len_hcode_header, size_image);
# endif





	//------------------------------------------------------
	// D Label Map Embedding

	const int len_sideinfo = len_hcode_header + len_h_encoded_labelmap;
	const int len_ref = (width_image + width_image_1) * 8;
	const int len_auxinfo = len_sideinfo + len_ref;
	const int len_embed = len_auxinfo + (len_secret -= len_hcode_header);

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
	unsigned int mask_bit_long = 1 << (len_len_h_encoded_labelmap - 1);

	while (mask_bit_long)
	{
		bits_embed[idx_prepare++] = (len_h_encoded_labelmap & mask_bit_long) ? 1 : 0;
		mask_bit_long >>= 1;
	}

# if REPORT
	reportD2(bits_embed, idx_prepare, len_embed, len_len_h_encoded_labelmap, LEN_HC_RULE);
# endif

	//side info 3, encode(lab map) => lm bits
	for (int i = 0; i < len_h_encoded_labelmap; ++i)
		bits_embed[idx_prepare++] = bits_h_encoded_labelmap[i];

	assert(idx_prepare == len_sideinfo);

# if REPORT
	reportD3(bits_embed, idx_prepare, len_embed, len_hcode_header);
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
			bits_embed[idx_prepare++] = byte2bits[(unsigned char)byte_image[static_cast<std::vector<unsigned char, std::allocator<unsigned char>>::size_type>(row) * width_image]][j];

	assert(idx_prepare == len_auxinfo);

# if REPORT
	reportD5(byte_image, idx_prepare, len_embed, width_image);
# endif

	for (int i = 0; i < len_secret; ++i)
		bits_embed[idx_prepare++] = bits_rand_secret[i] & 0x01;

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
		byte_image[static_cast<std::vector<unsigned char, std::allocator<unsigned char>>::size_type>(row) * width_image] = bits2byte(bits_embed, idx_embed);
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

	if (!path_setgo_img.empty())
		writeImage(path_setgo_img, byte_image);

# if REPORT
	fprintf(fptr_report, "\nE. write stego image to %s\n\n", path_setgo_img.c_str());
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
			bit_extract[idx_ex++] = byte2bits[(unsigned char)byte_image[static_cast<std::vector<unsigned char, std::allocator<unsigned char>>::size_type>(row) * width_image]][j];

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
		assert(p.first != -1 && p.second != 0);

		int len_cur_hcode = p.second;

		assignValue(h_decoder, bit_extract, idx_ex_proceed, static_cast<size_t>(idx_ex_proceed) + len_cur_hcode, i);

# if REPORT
		fprintf(fptr_report, "\tproceed %2d bits\t %d : ", idx_ex_proceed, i);
		for (int i = 0; i < len_cur_hcode; ++i)
			fprintf(fptr_report, "%d ", bit_extract[static_cast<std::vector<unsigned char, std::allocator<unsigned char>>::size_type>(idx_ex_proceed) + i]);
		fprintf(fptr_report, "\n");
# endif

		idx_ex_proceed += len_cur_hcode;
	}

	assert(idx_ex_proceed == LEN_HC_RULE);

	int len_ex_h_encoded_labelmap = 0;

	for (int i = 0; i < len_len_h_encoded_labelmap; ++i)
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
		byte_image[static_cast<std::vector<unsigned char, std::allocator<unsigned char>>::size_type>(row) * width_image] = bits2byte(bit_extract, idx_ex_proceed);
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

	vector<unsigned char> byte_orig = readImageRaw(path_cover_img);

	for (int i = 0; i < size_image; ++i)
		assert(byte_image[i] == byte_orig[i]);

# if REPORT
	reportG4(byte_image, path_resto_img.c_str());
# endif

	if (!path_resto_img.empty())
		writeImage(path_resto_img, byte_image);

# if PRINT
	if (!path_res_hex.empty())
		printByteToFile(byte_image, width_image, path_res_hex);
# endif


# if REPORT
	reportEnd();
# endif



	struct Result res;
	res.size_image = size_image;
	res.width_image = width_image;
	res.len_len_h_encoded_labelmap = len_len_h_encoded_labelmap;
	res.len_h_values = LEN_HC_RULE;
	for (int i = 0; i < 9; ++i)
	{
		res.freq[i] = cnt_t[i];
		res.len_code[i] = (int)bits_hcode[h_encoder[i]].size();
		vector<unsigned char> buf = bits_hcode[h_encoder[i]];
		res.code[i] = bits2byte(buf);
	}
	res.len_h_encoded_labelmap = len_h_encoded_labelmap;
	res.len_secret = len_secret;
	res.len_ref = len_ref;

	return res;
}

