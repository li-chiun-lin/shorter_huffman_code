#include "../lib/lib.h"
#include "msb_huffman.h"

void reportA(vector<unsigned char>& val_labelmap, vector<int>& cnt_t, int width_image, int domain_hcode)
{
	fprintf(fptr_report, "A. Label Map Generation, show only first few.\n\t");

	// reference bytes
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "x ");
	fprintf(fptr_report, "\n\t");

	for (int row = 1; row < ROW_SAMPLE; ++row)
	{
		// reference bytes
		fprintf(fptr_report, "x ");

		int rw = row * width_image;
		for (int col = 1; col < width_image; ++col)
			fprintf(fptr_report, "%d ", val_labelmap[rw + col]);
		fprintf(fptr_report, "\n\t");
	}
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

void reportC(vector<int>& h_encoder, vector<int>& cnt_t, int len_h_encoded_labelmap, int len_secret, int len_hcode_header)
{
	fprintf(fptr_report, "C. Huffman Coding, %d, LM = %d, extra-capacity = %d = %d + %d\n",
		len_h_encoded_labelmap + len_secret, len_h_encoded_labelmap, len_secret, len_secret - len_hcode_header, len_hcode_header);

	// header
	fprintf(fptr_report, "%8s %8s %8s %8s %8s %8s %8s \n", "t", "dist", "code", "cap.", "c len", "p len", "delta");
	fprintf(fptr_report, "%8s------------------------------------------------------\n", "-");
	fprintf(fptr_report, "\t \\hline \n");

	int total_cnt = 0;
	int total_cap = 0;
	int total_len = 0;

	for (int t = 0; t < DOMAIN_HCODE; ++t)
	{
		int cap = t < 8 ? t + 1 : t;
		int len = (int)bits_hcode[h_encoder[t]].size();
		int pay = cap - len;
		int del = pay * cnt_t[t];

		total_cnt += cnt_t[t];
		total_cap += cap * cnt_t[t];
		total_len += len * cnt_t[t];

		fprintf(fptr_report, "%8d & %8d & ", t, cnt_t[t]);

		size_t j = 0;
		while (j < bits_hcode[h_encoder[t]].size())
		{
			fprintf(fptr_report, "%x", bits_hcode[h_encoder[t]][j]);
			++j;
		}

		while (j < 8)
		{
			fprintf(fptr_report, " ");
			++j;
		}

		fprintf(fptr_report, " & %8d & %8d & %8d & %8d  \\\\ \n", cap, len, pay, del);
	}

	fprintf(fptr_report, "\t \\hline \n");
	fprintf(fptr_report, "%8s & %8d & & & %8d & %8d & %8d \\\\ \n", "Total", total_cnt, total_cap, total_len, total_cap - total_len);
	fprintf(fptr_report, "\t \\hline \n");
	fprintf(fptr_report, "\n\n");
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
	fprintf(fptr_report, "\n\tembeding progress %d / %d bits \n\n", idx_embed, len_auxinfo);
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