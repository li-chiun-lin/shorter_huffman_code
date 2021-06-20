
#include "../lib/Header.h"

# define REPORT 1
# define PRINT 1

# define COVER_IMG_FN "../image/lena512.raw"
//# define COVER_IMG_FN "../image/Lena.tiff"
//# define TIFF 1

# define ENC_IMG_FN     COVER_IMG_FN"_encrypt.raw"
# define STEGO_IMG_FN   COVER_IMG_FN"_stego.raw"
# define RESTORE_IMG_FN COVER_IMG_FN"_restore.raw"

# if REPORT
#	define REPORT_FN COVER_IMG_FN"_report.txt"
#	define LEN_SAMPLE 1024
# endif

# if PRINT
#	define FN_ORIGINAL_IMAGE_HEXDUMP COVER_IMG_FN"_hex.txt"
#	define FN_RESTORE_IMAGE_HEXDUMP  COVER_IMG_FN"_restore_hex.txt"
# endif

# define SIZE_HCODE 9
# define MAX_T 8
# define LEN_HC_RULE 32
# define LEN_LEN_H_ENCODED_LABELMAP 20
# define LEN_HCODE_HEADER  (LEN_HC_RULE + LEN_LEN_H_ENCODED_LABELMAP)
# define MAX_HCODE_LEN 5

static const unsigned char byte_hcode[SIZE_HCODE] = { 0x00, 0x01, 0x04, 0x05, 0x0c, 0x0d, 0x0e, 0x1e, 0x1f };
static const unsigned char len_hcode[SIZE_HCODE] = { 2, 2, 3, 3, 4, 4, 4, 5, 5 };
static const vector< vector<unsigned char> > bits_hcode = { 
	{0, 0}, 
	{0, 1}, 
	{1, 0, 0},
	{1, 0, 1}, 
	{1, 1, 0, 0}, 
	{1, 1, 0, 1}, 
	{1, 1, 1, 0}, 
	{1, 1, 1, 1, 0}, 
	{1, 1, 1, 1, 1} 
};

vector<unsigned char> genLabelMap(vector<unsigned char> &byte_image, 
	int size_image, int width_image, vector<int> &cnt_t)
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

void restoreImage(vector<unsigned char>& byte_image, vector<unsigned char>& val_ex_labelmap, int width_image)
{
	for (int i = 1; i < width_image; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int idx = row + col;
			int t = val_ex_labelmap[idx];

			unsigned char px = predictedValue(byte_image, idx, width_image);

			if (t < 7)
				px |= bit_mask[t + 1];

			byte_image[idx] = px;
		}
	}
}

int buildHuffmanEncoder(vector<int>& h_encoder, vector<int>& cnt_t)
{
	priority_queue<pair<int, int>> pq;
	int len_h_encoded_labelmap = 0;

	for (int t = 0; t < SIZE_HCODE; ++t)
		pq.push(make_pair(cnt_t[t], t));

	for (int i = 0; i < SIZE_HCODE; ++i)
	{
		auto p = pq.top();
		pq.pop();
		h_encoder[p.second] = i;
		len_h_encoded_labelmap += p.first * len_hcode[i];
	}

	return len_h_encoded_labelmap;
}

int encodeLabelMap(vector<unsigned char> &val_labelmap, int width_image, 
	vector<int>& h_encoder, vector<unsigned char> &bits_h_encoded_labelmap, int & len_secret)
{
	int idx_h_encoded_labelmap = 0;

	for (int i = 1; i < width_image; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int t = val_labelmap[row + col];
			int idx_hc = h_encoder[t];
			int len_hc = len_hcode[idx_hc];

			for (int k = 0; k < len_hc; ++k)
				bits_h_encoded_labelmap[idx_h_encoded_labelmap++] = bits_hcode[idx_hc][k];

			len_secret += (t < 7 ? t + 1 : 8) - len_hc;
		}
	}

	return idx_h_encoded_labelmap;
}

int main()
{
	vector<unsigned char> byte_image = readImage(COVER_IMG_FN);
	const int size_image = byte_image.size();
	const int width_image = (int)sqrt(size_image);
	const int width_image1 = width_image + 1;

	srand(0);

# if REPORT
	FILE* fptr_report = NULL;
	int ren = fopen_s(&fptr_report, REPORT_FN, "w");
	if (ren || fptr_report == NULL)
		return -2;
	fprintf(fptr_report, "cover image: %s, %d x %d = %d\n\n", COVER_IMG_FN, width_image, width_image, size_image);
# endif

# if PRINT
	printByteToFile(byte_image, width_image, FN_ORIGINAL_IMAGE_HEXDUMP);
# endif

	//------------------------------------------------------
	// A Label Map Generation
	vector<int> cnt_t(SIZE_HCODE);
	vector<unsigned char> val_labelmap = 
		genLabelMap(byte_image, size_image, width_image, cnt_t);

# if 0
	assert(pixel2t[156 ^ 150] == 4);
# endif

# if REPORT
	fprintf(fptr_report, "A. Label Map Generation, okay, show only first few.\n\t");
	for (int i = 1; i < 8; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
			fprintf(fptr_report, "%2x", val_labelmap[row + col]);
		fprintf(fptr_report, "\n\t");
	}
	fprintf(fptr_report, "\n\tDistribution of t\n");
	fprintf(fptr_report, "\t%6s ", "t");
	for (int i = 0; i < SIZE_HCODE; ++i)
		fprintf(fptr_report, "%6d ", i);
	fprintf(fptr_report, "\n\t%6s ", "dist");
	for (int i = 0; i < SIZE_HCODE; ++i)
		fprintf(fptr_report, "%6d ", cnt_t[i]);
	fprintf(fptr_report, "\n\n");
# endif

	//------------------------------------------------------
	// B Image Encryption
	vector<unsigned char> byte_rand_keystream(size_image);
	generate(byte_rand_keystream.begin(), byte_rand_keystream.end(), rand);

# if REPORT
	fprintf(fptr_report, "B. Image Encryption with random key stream, only show first few\n\n");
	fprintf(fptr_report, "B1. original pixels, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_image[i], i % 128 == 127 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "B2. random key stream, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_rand_keystream[i], i % 128 == 127 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
# endif

	XOR(byte_image, byte_rand_keystream);

# if REPORT
	fprintf(fptr_report, "B3. cipher bytes, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_image[i], i % 128 == 127 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
# endif









	//------------------------------------------------------
	// C Huffman Coding  
	
	vector<int> h_encoder(SIZE_HCODE);
	int len_h_encoded_labelmap = buildHuffmanEncoder(h_encoder, cnt_t);

	int len_secret = 0;
	vector<unsigned char> bits_h_encoded_labelmap(len_h_encoded_labelmap);
	int idx_h_encoded_labelmap = encodeLabelMap(val_labelmap, width_image, h_encoder, 
		bits_h_encoded_labelmap, len_secret);

	assert(idx_h_encoded_labelmap == len_h_encoded_labelmap);

# if REPORT
	fprintf(fptr_report, "C. Huffman Coding, %d, LM = %d, extra-capacity = %d + 52\n", 
		len_h_encoded_labelmap + len_secret, len_h_encoded_labelmap, len_secret - 52);

	fprintf(fptr_report, "%6s ", "t");
	for (int i = 0; i < SIZE_HCODE; ++i)
		fprintf(fptr_report, "%6d ", i);
	fprintf(fptr_report, "\n%6s ", "code");
	for (int i = 0; i < SIZE_HCODE; ++i)
		fprintf(fptr_report, "%6X ", byte_hcode[h_encoder[i]]);
	fprintf(fptr_report, "\n%6s ", "len");
	for (int i = 0; i < SIZE_HCODE; ++i)
		fprintf(fptr_report, "%6d ", len_hcode[h_encoder[i]]);
	fprintf(fptr_report, "\n\n");
# endif





	//------------------------------------------------------
	// D Label Map Embedding
	
	const int len_sideinfo = LEN_HCODE_HEADER + len_h_encoded_labelmap;
	const int len_ref = (width_image + width_image - 1) * 8;
	const int len_auxinfo = len_sideinfo + len_ref;
	const int len_embed = len_auxinfo + (len_secret -= LEN_HCODE_HEADER);

	vector<unsigned char> bits_rand_secret(len_secret);
	generate(bits_rand_secret.begin(), bits_rand_secret.end(), rand);

# if 0
	std::mt19937 rnd{ std::random_device()() };
	shuffle(bits_rand_secret.begin(), bits_rand_secret.end(), rnd);
# endif

	vector<unsigned char> bits_embed(len_embed);
	int idx_prepare = 0;

	//side info 1, Huffman coding rule - 0 to 8 - 32 bits
	for (int t = 0; t < SIZE_HCODE; ++t)
	{
		int idx = h_encoder[t];
		int len = len_hcode[idx];

		for (int j = 0; j < len; ++j)
			bits_embed[idx_prepare++] = bits_hcode[idx][j];
	}

# if REPORT
	fprintf(fptr_report, "D. Label Map Embedding\n\n");
	fprintf(fptr_report, "D1. Side Info, Huffman coding rule 32 bits\n\t");
	for (int i = 0; i < LEN_HC_RULE; ++i)
		fprintf(fptr_report, "%d ", bits_embed[i]);
	fprintf(fptr_report, "\n\n\tprepare embeding bits %d \n\n", idx_prepare);
# endif

	//side info 2, length of encode(lab map) => lm, 20 bits
	unsigned int bit_long_mask = 1 << (LEN_LEN_H_ENCODED_LABELMAP - 1);
	while (bit_long_mask)
	{
		bits_embed[idx_prepare++] = (len_h_encoded_labelmap & bit_long_mask) ? 1 : 0;
		bit_long_mask >>= 1;
	}

# if REPORT
	fprintf(fptr_report, "D2. Side Info, length of hEncode(LabelMap) 20 bits\n\t");
	for (int i = 0; i < LEN_LEN_H_ENCODED_LABELMAP; ++i)
		fprintf(fptr_report, "%d ", bits_embed[LEN_HC_RULE + i]);
	fprintf(fptr_report, "\n\n\tprepare embeding %d / %d bits\n\n", idx_prepare, len_embed);
# endif

	//side info 3, encode(lab map) => lm bits
	for (int i = 0; i < len_h_encoded_labelmap; ++i)
		bits_embed[idx_prepare++] = bits_h_encoded_labelmap[i];

	assert(idx_prepare == len_sideinfo);

# if REPORT
	fprintf(fptr_report, "D3. Side Info, hEncode(LabelMap) LM bits, only show first few bits\n\t");
	for (int i = 0; i < 512; ++i)
		fprintf(fptr_report, "%d %s", bits_embed[LEN_HCODE_HEADER + i], i % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n\tprepare embeding %d / %d bits\n\n", idx_prepare, len_embed);
# endif

	// reference values
	// first row
	for (int col = 0; col < width_image; ++col)
		for (int j = 0; j < 8; ++j)
			bits_embed[idx_prepare++] = byte2bits[byte_image[col]][j];

# if REPORT
	fprintf(fptr_report, "D4. reference value, 1st row of enc(image), 512 bytes\n\t");
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "%02X %s", byte_image[col], col % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n\tprepare embeding %d / %d bits\n\n", idx_prepare, len_embed);
# endif

	// first col, but except (0, 0)
	for (int row = 1; row < width_image; ++row)
		for (int j = 0; j < 8; ++j)
			bits_embed[idx_prepare++] = byte2bits[(unsigned char)byte_image[row * width_image]][j];

	assert(idx_prepare == len_auxinfo);

# if REPORT
	fprintf(fptr_report, "D5. reference value, 1st col of enc(image), 511 bytes\n\t");
	fprintf(fptr_report, "   ");
	for (int row = 1; row < width_image; ++row)
		fprintf(fptr_report, "%02X %s", byte_image[row * width_image], row % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n\tprepare embeding %d / %d bits\n\n", idx_prepare, len_embed);
	fprintf(fptr_report, "\ttotal auxiliary info size = %d bits\n\n", idx_prepare);
# endif

	for (int i = 0; i < len_secret; ++i)
	{
		bits_embed[idx_prepare] = bits_rand_secret[i] & 0x01;
		//assert(bits_embed[idx_embed] == 0 || bits_embed[idx_embed] == 1);
		++idx_prepare;
	}

	assert(idx_prepare == len_embed);

# if REPORT
	fprintf(fptr_report, "D6. random secret bits, only show first few bits\n\t");
	for (int i = 0; i < 512; ++i)
		fprintf(fptr_report, "%d %s", bits_embed[len_auxinfo + i], i % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n\tprepare embeding %d / %d bits\n\n", idx_prepare, len_embed);
# endif

	// embed auxiliary info into 1st row and 1st col, 8 bits * 1023
	int idx_embed = 0;
	for (int col = 0; col < width_image; ++col)
	{
		byte_image[col] = bits2byte(bits_embed, idx_embed);
		idx_embed += 8;
	}

# if REPORT
	fprintf(fptr_report, "D7. embed into the 1st row of enc(image), 512 bytes\n\t");
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "%02X %s", byte_image[col], col % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tembeding progress %d / %d bits \n\n", idx_embed, len_auxinfo);
# endif

	// first col, but except (0, 0)
	for (int row = 1; row < width_image; ++row)
	{
		byte_image[row * width_image] = bits2byte(bits_embed, idx_embed);
		idx_embed += 8;
	}

# if REPORT
	fprintf(fptr_report, "D8. embed into the 1st col of enc(image), except(0, 0), 511 bytes\n\t");
	fprintf(fptr_report, "   ");
	for (int row = 1; row < width_image; ++row)
		fprintf(fptr_report, "%02X %s", byte_image[row * width_image], row % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tembeding progress %d / %d bits \n\n", idx_embed, len_auxinfo);



	fprintf(fptr_report, "D9. embed into rest of the enc(image)\n");
	bool printmark = true;
# endif

	for (int i = 1; i < width_image; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int idx = row + col;
			int t = val_labelmap[idx];

			assert(idx_embed <= len_embed);

			if (t >= 7)
			{
				byte_image[idx] = bits2byte(bits_embed, idx_embed);
				idx_embed += 8;
			}
			else
			{
				byte_image[idx] = bits2byte(bits_embed, idx_embed, t + 1) | (byte_image[idx] & (bit_mask[t] - 1));
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

	assert(idx_embed == idx_prepare);

	writeImage(STEGO_IMG_FN, byte_image);

# if REPORT
	fprintf(fptr_report, "\nE. write stego image to %s\n\n", STEGO_IMG_FN);
# endif











	vector<unsigned char> bit_extract(size_image * 8);
	int idx_ex = 0;

	// first row
	for (int col = 0; col < width_image; ++col)
	{
		for (int j = 0; j < 8; ++j)
		{
			bit_extract[idx_ex] = byte2bits[byte_image[col]][j];
			//assert(bit_extract[idx_ex] == 1 || bit_extract[idx_ex] == 0);
			idx_ex++;
		}
	}

# if REPORT
	fprintf(fptr_report, "F. Data Extraction and Image Recovery\n\n");
	fprintf(fptr_report, "F1. extract 1st row\n\t");
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "%02X %s", byte_image[col], col % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n\tthe first few bits\n\t");
	for (int i = 0; i < 128; ++i)
		fprintf(fptr_report, "%x %s", bit_extract[i], i % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n\textract %d bits\n\n", idx_ex);
# endif

	// first col, but except (0, 0)
	for (int row = 1; row < width_image; ++row)
	{
		for (int j = 0; j < 8; ++j)
		{
			bit_extract[idx_ex] = byte2bits[(unsigned char)byte_image[row * width_image]][j];
			//assert(bit_extract[idx_ex] == 1 || bit_extract[idx_ex] == 0);
			idx_ex++;
		}
	}

# if REPORT
	fprintf(fptr_report, "F2. extract 1st col, except (0, 0)\n\t");
	fprintf(fptr_report, "   ");
	for (int row = 1; row < width_image; ++row)
		fprintf(fptr_report, "%02X %s", byte_image[row * width_image], row % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n\textract %d bits\n\n", idx_ex);

	fprintf(fptr_report, "F3. recover Huffman coding rules\n");
# endif

	struct Trie* h_decoder = (struct Trie*)calloc(1, sizeof(struct Trie));
	for (int i = 0; i < SIZE_HCODE; ++i)
		build(h_decoder, bits_hcode[i]);

	int idx_ex_proceed = 0;
	for (int i = 0; i < SIZE_HCODE; ++i)
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
		auto p = decode(h_decoder, bit_extract, idx_ex_proceed);

		assert(0 <= p.first && p.first <= MAX_T);
		assert(p.second);

		unsigned char t = p.first;
		int idx_ex_lm = idx_row_ex_lm * width_image + idx_col_ex_lm;
		val_ex_labelmap[idx_ex_lm] = p.first;

		assert(val_ex_labelmap[idx_ex_lm] == val_labelmap[idx_ex_lm]);

		for (int j = 0; j <= t && j < MAX_T; ++j)
		{
			bit_extract[idx_ex] = ((byte_image[idx_ex_lm] << j) & 0x80) ? 1 : 0;
			//assert(bit_extract[idx_ex] == 1 || bit_extract[idx_ex] == 0);
			++idx_ex;
		}

		idx_ex_proceed += p.second;

		++idx_col_ex_lm;

		if (idx_col_ex_lm == width_image)
		{
			idx_col_ex_lm = 1;
			++idx_row_ex_lm;
		}
	}

	del(h_decoder);

# if REPORT
	fprintf(fptr_report, "F5. recover Label Map, proceed %d / %d bits, show only first few.\n\t", idx_ex_proceed, idx_ex);
	for (int i = 1; i < 8; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
			fprintf(fptr_report, "%2d", val_ex_labelmap[row + col]);
		fprintf(fptr_report, "\n\t");
	}
	fprintf(fptr_report, "\n");
# endif

	for (int col = 0; col < width_image; ++col)
	{
		byte_image[col] = bits2byte(bit_extract, idx_ex_proceed);
		idx_ex_proceed += 8;
	}

# if REPORT
	fprintf(fptr_report, "F6. recover first row of enc(image), proceed %d / %d bits, show only first few.\n\t", idx_ex_proceed, idx_ex);
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "%02X %s", byte_image[col], col % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
# endif

	for (int row = 1; row < width_image; ++row)
	{
		byte_image[row * width_image + 1] = bits2byte(bit_extract, idx_ex_proceed);
		idx_ex_proceed += 8;
	}

# if REPORT
	fprintf(fptr_report, "F7. recover first col of enc(image), except(0, 0), proceed %d / %d bits, show only first few.\n\t", idx_ex_proceed, idx_ex);
	fprintf(fptr_report, "   ");
	for (int row = 1; row < width_image; ++row)
		fprintf(fptr_report, "%02X %s", byte_image[row * width_image + 1], row % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
# endif

	vector<unsigned char> bit_secret_extra(len_secret);

	for (size_t i = 0; i < bit_secret_extra.size(); ++i)
	{
		bit_secret_extra[i] = bit_extract[idx_ex_proceed];
		assert(bit_secret_extra[i] == (bits_rand_secret[i] & 0x01));
		++idx_ex_proceed;
	}

# if REPORT
	fprintf(fptr_report, "F8. recover secret %d / %d bits, show only first few.\n\t", idx_ex_proceed, idx_ex);
	for (size_t i = 0; i < 512; ++i)
		fprintf(fptr_report, "%2d %s", bit_secret_extra[i], i % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
# endif

# if REPORT
	fprintf(fptr_report, "G. Image Recovery with key stream, only show first few\n\n");
	fprintf(fptr_report, "G1. cipher bytes, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_image[i], i % 128 == 127 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "G2. key stream, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_rand_keystream[i], i % 128 == 127 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
# endif

	XOR(byte_image, byte_rand_keystream);

# if REPORT
	fprintf(fptr_report, "G3. decrypted bytes, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_image[i], i % 128 == 127 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
# endif

	//-------------------------------------------------------------------

	restoreImage(byte_image, val_ex_labelmap, width_image);

# if REPORT
	fprintf(fptr_report, "G4. revoered pixels, only show first few, write stego image to %s\n\t", RESTORE_IMG_FN);
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_image[i], i % 128 == 127 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
# endif

	writeImage(RESTORE_IMG_FN, byte_image);

# if PRINT
	printByteToFile(byte_image, width_image, FN_RESTORE_IMAGE_HEXDUMP);
# endif


# if REPORT
	fclose(fptr_report);
# endif

	return 0;
}

