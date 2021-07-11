#include "../lib/lib.h"
#include "shorter_huffman.h"


vector<unsigned char> encodeLabelMap_(vector< vector<int> >& encoder, vector<unsigned char>& val_labelmap)
{
	vector<unsigned char> bits_h_encoded_labelmap;

	for (size_t i = CNT_REF; i < val_labelmap.size(); ++i)
	{
		bits_h_encoded_labelmap.insert(bits_h_encoded_labelmap.end(), encoder[val_labelmap[i]].begin(), encoder[val_labelmap[i]].end());
	}

	return bits_h_encoded_labelmap;
}

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

void restoreImage_(vector<unsigned char>& byte_image, vector<unsigned char>& val_ex_labelmap, int width_image)
{
	int t = 0;

	// first row
	for (int idx = CNT_REF; idx < width_image; ++idx)
	{
		t = val_ex_labelmap[idx];

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
		t = val_ex_labelmap[rw];

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

int shr_huffman(string path_cover_img, string path_report, string path_encry_img, string path_setgo_img, string path_resto_img, string path_ori_hex, string path_res_hex, unsigned int seed)
{
	vector<unsigned char> byte_image = readImageRaw(path_cover_img);
	const int size_image = (int)byte_image.size();

	cout << "shr " << path_cover_img << endl;
	//assert(size_image == 512 * 512);

	const int width_image = (int)sqrt(size_image);
	const int width_image1 = width_image + 1;
	const int width_image_1 = width_image - 1;

	// need more fine tune
	const int LEN_LEN_H_ENCODED_LABELMAP = (int)ceil(log2(width_image_1 * width_image_1 * 5) - 1);

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

	if (!path_encry_img.empty())
		writeImage(path_encry_img, byte_image);

# if REPORT
	reportB3(byte_image, path_encry_img.c_str());
# endif

	//------------------------------------------------------
	// C Huffman Coding  

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

# if REPORT
	reportC_(cnt_t, h_encoder, len_embed, len_h_encoded_labelmap, len_ref, (int)bit_topology.size(), (int)bit_val.size(), LEN_LEN_H_ENCODED_LABELMAP, size_image);
# endif
# 


	//------------------------------------------------------
	// D Label Map Embedding

	vector<unsigned char> bits_embed(len_embed);
	int idx_prepare = 0;

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



# if REPORT
	reportD_(bits_embed, LEN_H_TOPOLOGY, LEN_H_VALUE, LEN_LEN_H_ENCODED_LABELMAP, idx_prepare);
# endif





	for (int i = 0; i < len_h_encoded_labelmap; ++i)
		bits_embed[idx_prepare++] = bits_h_encoded_labelmap[i];

	assert(idx_prepare == len_hcode_header + LEN_LEN_H_ENCODED_LABELMAP + len_h_encoded_labelmap);


# if REPORT
	reportD2_(bits_embed, idx_prepare, len_embed, len_hcode_header + LEN_LEN_H_ENCODED_LABELMAP);
# endif


	for (int i = 0; i < CNT_REF; ++i)
		for (int j = 0; j < 8; ++j)
			bits_embed[idx_prepare++] = byte2bits[byte_image[i]][j];

	assert(idx_prepare == len_hcode_header + LEN_LEN_H_ENCODED_LABELMAP + len_h_encoded_labelmap + len_ref);


# if REPORT
	reportD3_(byte_image, idx_prepare, len_embed, CNT_REF);
# endif


	vector<unsigned char> bits_rand_secret(len_secret);
	generate(bits_rand_secret.begin(), bits_rand_secret.end(), rand);

	for (int i = 0; i < len_secret; ++i)
	{
		bits_embed[idx_prepare] = bits_rand_secret[i] & 0x01;
		++idx_prepare;
	}

	assert(idx_prepare == len_embed);


# if REPORT
	reportD4_(bits_embed, idx_prepare, len_embed, len_sideinfo);
# endif


	int idx_embed = 0;
	for (int col = 0; col < CNT_REF; ++col)
	{
		byte_image[col] = bits2byte(bits_embed, idx_embed);
		idx_embed += 8;
	}


# if REPORT
	reportD5_(byte_image, idx_embed, len_sideinfo);
# endif



# if REPORT
	fprintf(fptr_report, "D6. embed into rest of the enc(image)\n");
# endif


	idx_embed = embedMain_(val_labelmap, byte_image, bits_embed, idx_embed);

	assert(idx_embed == idx_prepare);

	if (!path_setgo_img.empty())
		writeImage(path_setgo_img, byte_image);

# if REPORT
	if (!path_setgo_img.empty())
		fprintf(fptr_report, "\nE. write stego image to %s\n\n", path_setgo_img.c_str());
	else
		fprintf(fptr_report, "\nE. do not write stego image\n\n");
# endif












	vector<unsigned char> bit_extract(size_image * 8);
	int idx_ex = 0;



	// reference bytes
	for (int col = 0; col < CNT_REF; ++col)
		for (int j = 0; j < 8; ++j)
			bit_extract[idx_ex++] = byte2bits[byte_image[col]][j];


# if REPORT
	reportF_(byte_image, bit_extract, idx_ex);
# endif



	vector<unsigned char> bit_topology_ex(bit_extract.begin(), bit_extract.begin() + LEN_H_TOPOLOGY);
	vector<unsigned char> bit_val_ex(bit_extract.begin() + LEN_H_TOPOLOGY, bit_extract.begin() + LEN_H_TOPOLOGY + LEN_H_VALUE);
	int it = 0;
	int iv = 0;
	Node* tree_ex = unserializeHuffmanTree(bit_topology_ex, it, bit_val_ex, iv, LEN_T);

	assert(it == LEN_H_TOPOLOGY);
	assert(iv == LEN_H_VALUE);

# if REPORT
	reportF2_(tree_ex);
# endif

	int idx_ex_proceed = it + iv;
	int len_ex_h_encoded_labelmap = 0;

	for (int i = 0; i < LEN_LEN_H_ENCODED_LABELMAP; ++i)
	{
		len_ex_h_encoded_labelmap <<= 1;
		len_ex_h_encoded_labelmap |= bit_extract[idx_ex_proceed++];
	}

	assert(len_ex_h_encoded_labelmap == len_h_encoded_labelmap);

# if REPORT
	fprintf(fptr_report, "\nF3. recover length of hEncode(label map) = %d, proceed %d bits\n\n",
		len_ex_h_encoded_labelmap, idx_ex_proceed);
# endif


	vector<unsigned char> val_ex_labelmap(size_image);
	int idx_ex_lm = CNT_REF;

	// re-construct lable map
	while (idx_ex_lm < val_ex_labelmap.size())
	{
		pair<unsigned char, int> p = decode(tree_ex, bit_extract, idx_ex_proceed);

		assert(0 <= p.first && p.first <= MAX_T);
		assert(p.second);
		assert(idx_ex_proceed + p.second <= idx_ex);

		unsigned char t = p.first;
		val_ex_labelmap[idx_ex_lm] = t;

		assert(val_ex_labelmap[idx_ex_lm] == val_labelmap[idx_ex_lm]);

		for (int j = 0; j <= t && j < MAX_T; ++j)
			bit_extract[idx_ex++] = ((byte_image[idx_ex_lm] << j) & 0x80) ? 1 : 0;

		idx_ex_proceed += p.second;

		++idx_ex_lm;
	}

	del(tree_ex);


# if REPORT
	reportF4_(val_ex_labelmap, idx_ex, idx_ex_proceed);
# endif


	for (int col = 0; col < CNT_REF; ++col)
	{
		byte_image[col] = bits2byte(bit_extract, idx_ex_proceed);
		idx_ex_proceed += 8;
	}

# if REPORT
	reportF6_(byte_image, idx_ex, idx_ex_proceed);
# endif

	vector<unsigned char> bit_secret_extra(len_secret);

	for (size_t i = 0; i < bit_secret_extra.size(); ++i)
		bit_secret_extra[i] = bit_extract[idx_ex_proceed++];

# if REPORT
	reportF7_(bit_secret_extra, idx_ex, idx_ex_proceed);
# endif


# if REPORT
	reportG(byte_image, byte_rand_keystream);
# endif

	XOR(byte_image, byte_rand_keystream);

# if REPORT
	reportG3(byte_image);
# endif


	//-------------------------------------------------------------------


	restoreImage_(byte_image, val_ex_labelmap, width_image);

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

	return 0;
}



