#include <iostream>
#include "../tiff-4.3.0/libtiff/tiffio.h"

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
		return ;

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

int main()
{
	readtiff();

	return 0;
}

