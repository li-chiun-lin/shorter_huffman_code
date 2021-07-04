#include <iostream>
#include <vector>

#include "../tiff-4.3.0/libtiff/tiffio.h"
#include "../lib/lib.h"

using namespace std;

void gentable()
{
	printf("unsigned char table[256] = {\n\t");
	for (int i = 0; i < 256; ++i)
	{
		int t = 0;
		unsigned char mask = 0x80;

		while (mask && (mask & i) == 0)
		{
			++t;
			mask >>= 1;
		}

		printf("%d, ", t);
		if (i % 16 == 15)
			printf("\n\t");
	}
	printf("};\n");
}

const static unsigned char mask[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

void genbits()
{
	printf("unsigned char byte2bits[256][8] = {\n");
	for (int byte = 0; byte < 256; ++byte)
	{
		printf("{");
		for (int b = 0; b < 8; ++b)
		{
			printf("%d%s ", (byte & mask[b]) ? 1 : 0, (b != 7) ? "," : "");
		}
		printf("}, ");
		if (byte % 4 == 3)
			printf("\n");
	}
	printf("}\n");
}

void printArray(unsigned char* array, int width)
{
	for (int i = 0; i < width; i++)
		printf("%X ", array[i]);
	printf("\n\n");
}

void readtiff()
{
	TIFF* tif = TIFFOpen("../image/Lena.tiff", "r");
	if (tif == NULL)
		return;

	FILE* fptr = NULL;
	errno_t err = fopen_s(&fptr, "../out/Lena.tiff.hd.txt", "w");

	if (err || fptr == NULL)
		return;

	FILE* fptr_b = NULL;
	err = fopen_s(&fptr_b, "../out/Lena.tiff.raw", "wb");

	if (err || fptr_b == NULL)
		return;

	int width, height;
	tdata_t buf_tif;

	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
	buf_tif = _TIFFmalloc(TIFFScanlineSize(tif));

	unsigned char* data;
	printf("w %d h %d\n", width, height);

	for (int row = 0; row < height; row++)
	{
		TIFFReadScanline(tif, buf_tif, row);
		data = (unsigned char*)buf_tif;
		for (int i = 0; i < width; i++)
		{
			fprintf(fptr, "%02X ", data[i]);
		}

		fwrite(data, sizeof(unsigned char), width, fptr_b);

		fprintf(fptr, "\n");
	}

	fclose(fptr_b);
	fclose(fptr);

	_TIFFfree(buf_tif);
	TIFFClose(tif);

}

void tiff2raw(string fn_tiff, string fn_raw, string fn_txt)
{
	errno_t err = 0;

	TIFF* tif = TIFFOpen(fn_tiff.c_str(), "r");
	if (tif == NULL)
		return;

	FILE* fptr = NULL;
	err = fopen_s(&fptr, fn_txt.c_str(), "w");

	if (err || fptr == NULL)
		return;

	FILE* fptr_b = NULL;
	err = fopen_s(&fptr_b, fn_raw.c_str(), "wb");

	if (err || fptr_b == NULL)
		return;

	int width, height;
	tdata_t buf_tif;

	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
	buf_tif = _TIFFmalloc(TIFFScanlineSize(tif));

	unsigned char* data;
	printf("w %d h %d\n", width, height);

	for (int row = 0; row < height; row++)
	{
		TIFFReadScanline(tif, buf_tif, row);
		data = (unsigned char*)buf_tif;
		for (int i = 0; i < width; i++)
			fprintf(fptr, "%02X ", data[i]);

		fwrite(data, sizeof(unsigned char), width, fptr_b);

		fprintf(fptr, "\n");
	}

	fclose(fptr_b);
	fclose(fptr);

	_TIFFfree(buf_tif);
	TIFFClose(tif);

}

void testHuffman()
{
	vector<int> freq = { 9818, 9742, 15247, 33246, 44509, 53359, 41758, 24353, 29089 };

	Node* tree = buildHuffmanTree(freq);
	vector< vector<int> > encoder = buildHuffmanEncoder(tree, (int)freq.size());

	for (int i = 0; i < freq.size(); ++i)
	{
		printf("%d %6d : ", i, freq[i]);
		for (int j = 0; j < encoder[i].size(); ++j)
			printf("%d", encoder[i][j]);
		printf("\n");
	}

	vector<unsigned char> bit_topology;
	vector<unsigned char> bit_val;

	serializeHuffmanTree(tree, bit_topology, bit_val, 5);

	printf("%2d : ", (int)bit_topology.size());
	for (int i = 0; i < bit_topology.size(); ++i)
		printf("%x", bit_topology[i]);
	printf("\n");

	printf("%2d : ", (int)bit_val.size());
	for (int i = 0; i < bit_val.size(); ++i)
		printf("%x%s", bit_val[i], i % 5 == 4 ? " " : "");
	printf("\n");

	int it = 0;
	int iv = 0;
	Node* tree2 = unserializeHuffmanTree(bit_topology, it, bit_val, iv, 5);

	vector< vector<int> > encoder2 = buildHuffmanEncoder(tree2, (int)freq.size());

	for (int i = 0; i < encoder2.size(); ++i)
	{
		printf("%d : ", i);
		for (int j = 0; j < encoder2[i].size(); ++j)
			printf("%d", encoder2[i][j]);
		printf("\n");
	}

}


void convertTiffs()
{
	vector<string> fn = { "Airplane", "Baboon", "Jetplane", "Lena", "Man", "Tiffany" };

	for (string f : fn)
	{
		tiff2raw("../image/tiff/" + f + ".tiff", "../image/tiff/" + f + ".raw", "../out/" + f + ".tiff.txt");
	}
}

int main()
{
	convertTiffs();

	return 0;
}

