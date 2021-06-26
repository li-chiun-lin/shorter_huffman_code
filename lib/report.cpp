#include "pch.h"
#include "framework.h"


#include "lib.h"

FILE* fptr_report = NULL;

int reportBegin(int width_image, int size_image, const char* fn_report, const char* fn_cover_img)
{
	int ren = fopen_s(&fptr_report, fn_report, "w");

	if (ren || fptr_report == NULL)
		return -2;

	fprintf(fptr_report, "cover image: %s, %d x %d = %d\n\n", fn_cover_img, width_image, width_image, size_image);
	return 0;
}

void reportEnd()
{
	fclose(fptr_report);
}

void reportA(vector<unsigned char>& val_labelmap, vector<int>& cnt_t, int width_image)
{
	fprintf(fptr_report, "A. Label Map Generation, show only first few.\n\t");
	for (int row = 1; row < ROW_SAMPLE; ++row)
	{
		int rw = row * width_image;
		for (int col = 1; col < width_image; ++col)
			fprintf(fptr_report, "%d ", val_labelmap[rw + col]);
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
}

void reportB(vector<unsigned char>& byte_image, vector<unsigned char>& byte_rand_keystream)
{
	fprintf(fptr_report, "B. Image Encryption with random key stream\n\n");
	fprintf(fptr_report, "B1. original pixels, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_image[i], WRAP(i));
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "B2. random key stream, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_rand_keystream[i], WRAP(i));
	fprintf(fptr_report, "\n");
}

void reportB3(vector<unsigned char>& byte_image, const char* fn_enc_img)
{
	fprintf(fptr_report, "B3. cipher bytes, only show first few, write to %s\n\t", fn_enc_img);
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_image[i], WRAP(i));
	fprintf(fptr_report, "\n");
}



void reportD(vector<unsigned char>& bits_embed, int idx_prepare, int len_hc_rule)
{
	fprintf(fptr_report, "D. Label Map Embedding\n\n");
	fprintf(fptr_report, "D1. Side Info, Huffman coding rule bits\n\t");
	for (int i = 0; i < len_hc_rule; ++i)
		fprintf(fptr_report, "%d ", bits_embed[i]);
	fprintf(fptr_report, "\n\n\tprepare embeding bits %d \n\n", idx_prepare);
}

void reportD2(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed, int len_len_h_encoded_labelmap, int len_hc_rule)
{
	fprintf(fptr_report, "D2. Side Info, length of hEncode(LabelMap) 20 bits\n\t");
	for (int i = 0; i < len_len_h_encoded_labelmap; ++i)
		fprintf(fptr_report, "%d ", bits_embed[len_hc_rule + i]);
	fprintf(fptr_report, "\n\n\tprepare embeding %d / %d bits\n\n", idx_prepare, len_embed);
}

void reportD3(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed, int len_hcode_header)
{
	fprintf(fptr_report, "D3. Side Info, hEncode(LabelMap) LM bits, only show first few bits\n\t");
	for (int i = 0; i < 512; ++i)
		fprintf(fptr_report, "%d %s", bits_embed[len_hcode_header + i], WRAP(i));
	fprintf(fptr_report, "\n\tprepare embeding %d / %d bits\n\n", idx_prepare, len_embed);
}

void reportD4(vector<unsigned char>& byte_image, int idx_prepare, int len_embed, int width_image)
{
	fprintf(fptr_report, "D4. reference value, 1st row of enc(image), 512 bytes\n\t");
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "%02X %s", byte_image[col], WRAP(col));
	fprintf(fptr_report, "\n\tprepare embeding %d / %d bits\n\n", idx_prepare, len_embed);
}
void reportD5(vector<unsigned char>& byte_image, int idx_prepare, int len_embed, int width_image)
{
	fprintf(fptr_report, "D5. reference value, 1st col of enc(image), 511 bytes\n\t");
	fprintf(fptr_report, "   ");
	for (int row = 1; row < width_image; ++row)
		fprintf(fptr_report, "%02X %s", byte_image[row * width_image], WRAP(row));
	fprintf(fptr_report, "\n\tprepare embeding %d / %d bits\n\n", idx_prepare, len_embed);
	fprintf(fptr_report, "\ttotal auxiliary info size = %d bits\n\n", idx_prepare);
}

void reportD6(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed, int len_auxinfo)
{
	fprintf(fptr_report, "D6. random secret bits, only show first few bits\n\t");
	for (int i = 0; i < 512; ++i)
		fprintf(fptr_report, "%d %s", bits_embed[len_auxinfo + i], WRAP(i));
	fprintf(fptr_report, "\n\tprepare embeding %d / %d bits\n\n", idx_prepare, len_embed);
}

void reportD7(vector<unsigned char>& byte_image, int idx_embed, int width_image, int len_auxinfo)
{
	fprintf(fptr_report, "D7. embed into the 1st row of enc(image), 512 bytes\n\t");
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "%02X %s", byte_image[col], WRAP(col));
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tembeding progress %d / %d bits \n\n", idx_embed, len_auxinfo);
}

void reportD8(vector<unsigned char>& byte_image, int idx_embed, int width_image, int len_auxinfo)
{
	fprintf(fptr_report, "D8. embed into the 1st col of enc(image), except(0, 0), 511 bytes\n\t");
	fprintf(fptr_report, "   ");
	for (int row = 1; row < width_image; ++row)
		fprintf(fptr_report, "%02X %s", byte_image[row * width_image], WRAP(row));
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tembeding progress %d / %d bits \n\n", idx_embed, len_auxinfo);
}

void reportF(vector<unsigned char>& byte_image, vector<unsigned char>& bit_extract, int idx_ex, int width_image)
{
	fprintf(fptr_report, "F. Data Extraction and Image Recovery\n\n");
	fprintf(fptr_report, "F1. extract 1st row\n\t");
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "%02X %s", byte_image[col], WRAP(col));
	fprintf(fptr_report, "\n\tthe first few bits\n\t");
	for (int i = 0; i < 128; ++i)
		fprintf(fptr_report, "%x %s", bit_extract[i], WRAP(i));
	fprintf(fptr_report, "\n\textract %d bits\n\n", idx_ex);
}

void reportF2(vector<unsigned char>& byte_image, int idx_ex, int width_image)
{
	fprintf(fptr_report, "F2. extract 1st col, except (0, 0)\n\t");
	fprintf(fptr_report, "   ");
	for (int row = 1; row < width_image; ++row)
		fprintf(fptr_report, "%02X %s", byte_image[row * width_image], WRAP(row));
	fprintf(fptr_report, "\n\textract %d bits\n\n", idx_ex);
}

void reportF5(vector<unsigned char>& val_ex_labelmap, int idx_ex, int idx_ex_proceed, int width_image)
{
	fprintf(fptr_report, "F5. recover Label Map, proceed %d / %d bits, show only first few.\n\t", idx_ex_proceed, idx_ex);
	for (int i = 1; i < 8; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
			fprintf(fptr_report, "%2d", val_ex_labelmap[row + col]);
		fprintf(fptr_report, "\n\t");
	}
	fprintf(fptr_report, "\n");
}

void reportF6(vector<unsigned char>& byte_image, int idx_ex, int idx_ex_proceed, int width_image)
{
	fprintf(fptr_report, "F6. recover first row of enc(image), proceed %d / %d bits, show only first few.\n\t", idx_ex_proceed, idx_ex);
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "%02X %s", byte_image[col], WRAP(col));
	fprintf(fptr_report, "\n");
}

void reportF7(vector<unsigned char>& byte_image, int idx_ex, int idx_ex_proceed, int width_image)
{
	fprintf(fptr_report, "F7. recover first col of enc(image), except(0, 0), proceed %d / %d bits, show only first few.\n\t", idx_ex_proceed, idx_ex);
	fprintf(fptr_report, "   ");
	for (int row = 1; row < width_image; ++row)
		fprintf(fptr_report, "%02X %s", byte_image[row * width_image], WRAP(row));
	fprintf(fptr_report, "\n");
}

void reportF8(vector<unsigned char>& bit_secret_extra, int idx_ex, int idx_ex_proceed)
{
	fprintf(fptr_report, "F8. recover secret %d / %d bits, show only first few.\n\t", idx_ex_proceed, idx_ex);
	for (size_t i = 0; i < 512; ++i)
		fprintf(fptr_report, "%2d %s", bit_secret_extra[i], WRAP(i));
	fprintf(fptr_report, "\n");
}

void reportG(vector<unsigned char>& byte_image, vector<unsigned char>& byte_rand_keystream)
{
	fprintf(fptr_report, "G. Image Recovery with key stream, only show first few\n\n");
	fprintf(fptr_report, "G1. cipher bytes, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_image[i], WRAP(i));
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "G2. key stream, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_rand_keystream[i], WRAP(i));
	fprintf(fptr_report, "\n");
}

void reportG3(vector<unsigned char>& byte_image)
{
	fprintf(fptr_report, "G3. decrypted bytes, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_image[i], WRAP(i));
	fprintf(fptr_report, "\n");
}

void reportG4(vector<unsigned char>& byte_image, const char* fn_restore)
{
	fprintf(fptr_report, "G4. revoered pixels, only show first few, write stego image to %s\n\t", fn_restore);
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_image[i], WRAP(i));
	fprintf(fptr_report, "\n");
}