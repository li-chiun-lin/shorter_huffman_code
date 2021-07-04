#include "../lib/lib.h"
#include "shorter_huffman.h"

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

	fprintf(fptr_report, "%8s & %8s & %8s & %8s & %8s & %8s & %8s \\\\ \n", "t", "freq", "code", "cap.", "c len", "p len", "delta");
	fprintf(fptr_report, "%8s------------------------------------------------------\n", "-");
	fprintf(fptr_report, "\t \\hline \n");

	int total_cnt = 0;
	int total_cap = 0;
	int total_len = 0;

	for (int t = 0; t < DOMAIN_HCODE; ++t)
	{
		int cap = t < 8 ? t + 1 : t;
		int len = (int)encoder[t].size();
		int pay = cap - len;
		int del = pay * cnt_t[t];

		total_cnt += cnt_t[t];
		total_cap += cap * cnt_t[t];
		total_len += len * cnt_t[t];

		fprintf(fptr_report, "%8d & %8d & ", t, cnt_t[t]);

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

		fprintf(fptr_report, " & %8d & %8d & %8d & %8d \\\\ \n", cap, len, pay, del);
	}

	fprintf(fptr_report, "\t \\hline \n");
	fprintf(fptr_report, "%8s & %8d & & & %8d & %8d & %8d \\\\ \n", "Total", total_cnt, total_cap, total_len, total_cap - total_len);
	fprintf(fptr_report, "\t \\hline \n");
	fprintf(fptr_report, "\n\n");
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

void reportF2_(Node* tree)
{
	fprintf(fptr_report, "F2. recover Huffman tree\n\t");

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
void reportF4_(vector<unsigned char>& val_ex_labelmap, int idx_ex, int idx_ex_proceed)
{
	fprintf(fptr_report, "F4. recover Label Map, proceed %d / %d bits, show only first few.\n\t", idx_ex_proceed, idx_ex);

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