#include <iostream>
#include <fstream>
#include <queue>
#include <assert.h>
//#include <cassert>
//#include <Wincodec.h>

# define DB_SEC 1

# define REPORT 1
# define PRINT 0



# define COVER_IMG_FN "../image/lena512.raw"
//# define COVER_IMG_FN "../image/Lena.tiff"
//# define TIFF 1

# define ENC_IMG_FN COVER_IMG_FN"_enc.raw"
# define STEGO_IMG_FN COVER_IMG_FN"_ste.raw"

# if REPORT
#	define REPORT_FN COVER_IMG_FN"_rep.txt"
# endif

# if PRINT
#	define PRINT_FN COVER_IMG_FN"_hex.txt"
# endif

using namespace std;

const static unsigned char pixel2t[256] = {
		8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const static unsigned char byte2bits[256][8] = {
{0, 0, 0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0, 0, 1 }, {0, 0, 0, 0, 0, 0, 1, 0 }, {0, 0, 0, 0, 0, 0, 1, 1 },
{0, 0, 0, 0, 0, 1, 0, 0 }, {0, 0, 0, 0, 0, 1, 0, 1 }, {0, 0, 0, 0, 0, 1, 1, 0 }, {0, 0, 0, 0, 0, 1, 1, 1 },
{0, 0, 0, 0, 1, 0, 0, 0 }, {0, 0, 0, 0, 1, 0, 0, 1 }, {0, 0, 0, 0, 1, 0, 1, 0 }, {0, 0, 0, 0, 1, 0, 1, 1 },
{0, 0, 0, 0, 1, 1, 0, 0 }, {0, 0, 0, 0, 1, 1, 0, 1 }, {0, 0, 0, 0, 1, 1, 1, 0 }, {0, 0, 0, 0, 1, 1, 1, 1 },
{0, 0, 0, 1, 0, 0, 0, 0 }, {0, 0, 0, 1, 0, 0, 0, 1 }, {0, 0, 0, 1, 0, 0, 1, 0 }, {0, 0, 0, 1, 0, 0, 1, 1 },
{0, 0, 0, 1, 0, 1, 0, 0 }, {0, 0, 0, 1, 0, 1, 0, 1 }, {0, 0, 0, 1, 0, 1, 1, 0 }, {0, 0, 0, 1, 0, 1, 1, 1 },
{0, 0, 0, 1, 1, 0, 0, 0 }, {0, 0, 0, 1, 1, 0, 0, 1 }, {0, 0, 0, 1, 1, 0, 1, 0 }, {0, 0, 0, 1, 1, 0, 1, 1 },
{0, 0, 0, 1, 1, 1, 0, 0 }, {0, 0, 0, 1, 1, 1, 0, 1 }, {0, 0, 0, 1, 1, 1, 1, 0 }, {0, 0, 0, 1, 1, 1, 1, 1 },
{0, 0, 1, 0, 0, 0, 0, 0 }, {0, 0, 1, 0, 0, 0, 0, 1 }, {0, 0, 1, 0, 0, 0, 1, 0 }, {0, 0, 1, 0, 0, 0, 1, 1 },
{0, 0, 1, 0, 0, 1, 0, 0 }, {0, 0, 1, 0, 0, 1, 0, 1 }, {0, 0, 1, 0, 0, 1, 1, 0 }, {0, 0, 1, 0, 0, 1, 1, 1 },
{0, 0, 1, 0, 1, 0, 0, 0 }, {0, 0, 1, 0, 1, 0, 0, 1 }, {0, 0, 1, 0, 1, 0, 1, 0 }, {0, 0, 1, 0, 1, 0, 1, 1 },
{0, 0, 1, 0, 1, 1, 0, 0 }, {0, 0, 1, 0, 1, 1, 0, 1 }, {0, 0, 1, 0, 1, 1, 1, 0 }, {0, 0, 1, 0, 1, 1, 1, 1 },
{0, 0, 1, 1, 0, 0, 0, 0 }, {0, 0, 1, 1, 0, 0, 0, 1 }, {0, 0, 1, 1, 0, 0, 1, 0 }, {0, 0, 1, 1, 0, 0, 1, 1 },
{0, 0, 1, 1, 0, 1, 0, 0 }, {0, 0, 1, 1, 0, 1, 0, 1 }, {0, 0, 1, 1, 0, 1, 1, 0 }, {0, 0, 1, 1, 0, 1, 1, 1 },
{0, 0, 1, 1, 1, 0, 0, 0 }, {0, 0, 1, 1, 1, 0, 0, 1 }, {0, 0, 1, 1, 1, 0, 1, 0 }, {0, 0, 1, 1, 1, 0, 1, 1 },
{0, 0, 1, 1, 1, 1, 0, 0 }, {0, 0, 1, 1, 1, 1, 0, 1 }, {0, 0, 1, 1, 1, 1, 1, 0 }, {0, 0, 1, 1, 1, 1, 1, 1 },
{0, 1, 0, 0, 0, 0, 0, 0 }, {0, 1, 0, 0, 0, 0, 0, 1 }, {0, 1, 0, 0, 0, 0, 1, 0 }, {0, 1, 0, 0, 0, 0, 1, 1 },
{0, 1, 0, 0, 0, 1, 0, 0 }, {0, 1, 0, 0, 0, 1, 0, 1 }, {0, 1, 0, 0, 0, 1, 1, 0 }, {0, 1, 0, 0, 0, 1, 1, 1 },
{0, 1, 0, 0, 1, 0, 0, 0 }, {0, 1, 0, 0, 1, 0, 0, 1 }, {0, 1, 0, 0, 1, 0, 1, 0 }, {0, 1, 0, 0, 1, 0, 1, 1 },
{0, 1, 0, 0, 1, 1, 0, 0 }, {0, 1, 0, 0, 1, 1, 0, 1 }, {0, 1, 0, 0, 1, 1, 1, 0 }, {0, 1, 0, 0, 1, 1, 1, 1 },
{0, 1, 0, 1, 0, 0, 0, 0 }, {0, 1, 0, 1, 0, 0, 0, 1 }, {0, 1, 0, 1, 0, 0, 1, 0 }, {0, 1, 0, 1, 0, 0, 1, 1 },
{0, 1, 0, 1, 0, 1, 0, 0 }, {0, 1, 0, 1, 0, 1, 0, 1 }, {0, 1, 0, 1, 0, 1, 1, 0 }, {0, 1, 0, 1, 0, 1, 1, 1 },
{0, 1, 0, 1, 1, 0, 0, 0 }, {0, 1, 0, 1, 1, 0, 0, 1 }, {0, 1, 0, 1, 1, 0, 1, 0 }, {0, 1, 0, 1, 1, 0, 1, 1 },
{0, 1, 0, 1, 1, 1, 0, 0 }, {0, 1, 0, 1, 1, 1, 0, 1 }, {0, 1, 0, 1, 1, 1, 1, 0 }, {0, 1, 0, 1, 1, 1, 1, 1 },
{0, 1, 1, 0, 0, 0, 0, 0 }, {0, 1, 1, 0, 0, 0, 0, 1 }, {0, 1, 1, 0, 0, 0, 1, 0 }, {0, 1, 1, 0, 0, 0, 1, 1 },
{0, 1, 1, 0, 0, 1, 0, 0 }, {0, 1, 1, 0, 0, 1, 0, 1 }, {0, 1, 1, 0, 0, 1, 1, 0 }, {0, 1, 1, 0, 0, 1, 1, 1 },
{0, 1, 1, 0, 1, 0, 0, 0 }, {0, 1, 1, 0, 1, 0, 0, 1 }, {0, 1, 1, 0, 1, 0, 1, 0 }, {0, 1, 1, 0, 1, 0, 1, 1 },
{0, 1, 1, 0, 1, 1, 0, 0 }, {0, 1, 1, 0, 1, 1, 0, 1 }, {0, 1, 1, 0, 1, 1, 1, 0 }, {0, 1, 1, 0, 1, 1, 1, 1 },
{0, 1, 1, 1, 0, 0, 0, 0 }, {0, 1, 1, 1, 0, 0, 0, 1 }, {0, 1, 1, 1, 0, 0, 1, 0 }, {0, 1, 1, 1, 0, 0, 1, 1 },
{0, 1, 1, 1, 0, 1, 0, 0 }, {0, 1, 1, 1, 0, 1, 0, 1 }, {0, 1, 1, 1, 0, 1, 1, 0 }, {0, 1, 1, 1, 0, 1, 1, 1 },
{0, 1, 1, 1, 1, 0, 0, 0 }, {0, 1, 1, 1, 1, 0, 0, 1 }, {0, 1, 1, 1, 1, 0, 1, 0 }, {0, 1, 1, 1, 1, 0, 1, 1 },
{0, 1, 1, 1, 1, 1, 0, 0 }, {0, 1, 1, 1, 1, 1, 0, 1 }, {0, 1, 1, 1, 1, 1, 1, 0 }, {0, 1, 1, 1, 1, 1, 1, 1 },
{1, 0, 0, 0, 0, 0, 0, 0 }, {1, 0, 0, 0, 0, 0, 0, 1 }, {1, 0, 0, 0, 0, 0, 1, 0 }, {1, 0, 0, 0, 0, 0, 1, 1 },
{1, 0, 0, 0, 0, 1, 0, 0 }, {1, 0, 0, 0, 0, 1, 0, 1 }, {1, 0, 0, 0, 0, 1, 1, 0 }, {1, 0, 0, 0, 0, 1, 1, 1 },
{1, 0, 0, 0, 1, 0, 0, 0 }, {1, 0, 0, 0, 1, 0, 0, 1 }, {1, 0, 0, 0, 1, 0, 1, 0 }, {1, 0, 0, 0, 1, 0, 1, 1 },
{1, 0, 0, 0, 1, 1, 0, 0 }, {1, 0, 0, 0, 1, 1, 0, 1 }, {1, 0, 0, 0, 1, 1, 1, 0 }, {1, 0, 0, 0, 1, 1, 1, 1 },
{1, 0, 0, 1, 0, 0, 0, 0 }, {1, 0, 0, 1, 0, 0, 0, 1 }, {1, 0, 0, 1, 0, 0, 1, 0 }, {1, 0, 0, 1, 0, 0, 1, 1 },
{1, 0, 0, 1, 0, 1, 0, 0 }, {1, 0, 0, 1, 0, 1, 0, 1 }, {1, 0, 0, 1, 0, 1, 1, 0 }, {1, 0, 0, 1, 0, 1, 1, 1 },
{1, 0, 0, 1, 1, 0, 0, 0 }, {1, 0, 0, 1, 1, 0, 0, 1 }, {1, 0, 0, 1, 1, 0, 1, 0 }, {1, 0, 0, 1, 1, 0, 1, 1 },
{1, 0, 0, 1, 1, 1, 0, 0 }, {1, 0, 0, 1, 1, 1, 0, 1 }, {1, 0, 0, 1, 1, 1, 1, 0 }, {1, 0, 0, 1, 1, 1, 1, 1 },
{1, 0, 1, 0, 0, 0, 0, 0 }, {1, 0, 1, 0, 0, 0, 0, 1 }, {1, 0, 1, 0, 0, 0, 1, 0 }, {1, 0, 1, 0, 0, 0, 1, 1 },
{1, 0, 1, 0, 0, 1, 0, 0 }, {1, 0, 1, 0, 0, 1, 0, 1 }, {1, 0, 1, 0, 0, 1, 1, 0 }, {1, 0, 1, 0, 0, 1, 1, 1 },
{1, 0, 1, 0, 1, 0, 0, 0 }, {1, 0, 1, 0, 1, 0, 0, 1 }, {1, 0, 1, 0, 1, 0, 1, 0 }, {1, 0, 1, 0, 1, 0, 1, 1 },
{1, 0, 1, 0, 1, 1, 0, 0 }, {1, 0, 1, 0, 1, 1, 0, 1 }, {1, 0, 1, 0, 1, 1, 1, 0 }, {1, 0, 1, 0, 1, 1, 1, 1 },
{1, 0, 1, 1, 0, 0, 0, 0 }, {1, 0, 1, 1, 0, 0, 0, 1 }, {1, 0, 1, 1, 0, 0, 1, 0 }, {1, 0, 1, 1, 0, 0, 1, 1 },
{1, 0, 1, 1, 0, 1, 0, 0 }, {1, 0, 1, 1, 0, 1, 0, 1 }, {1, 0, 1, 1, 0, 1, 1, 0 }, {1, 0, 1, 1, 0, 1, 1, 1 },
{1, 0, 1, 1, 1, 0, 0, 0 }, {1, 0, 1, 1, 1, 0, 0, 1 }, {1, 0, 1, 1, 1, 0, 1, 0 }, {1, 0, 1, 1, 1, 0, 1, 1 },
{1, 0, 1, 1, 1, 1, 0, 0 }, {1, 0, 1, 1, 1, 1, 0, 1 }, {1, 0, 1, 1, 1, 1, 1, 0 }, {1, 0, 1, 1, 1, 1, 1, 1 },
{1, 1, 0, 0, 0, 0, 0, 0 }, {1, 1, 0, 0, 0, 0, 0, 1 }, {1, 1, 0, 0, 0, 0, 1, 0 }, {1, 1, 0, 0, 0, 0, 1, 1 },
{1, 1, 0, 0, 0, 1, 0, 0 }, {1, 1, 0, 0, 0, 1, 0, 1 }, {1, 1, 0, 0, 0, 1, 1, 0 }, {1, 1, 0, 0, 0, 1, 1, 1 },
{1, 1, 0, 0, 1, 0, 0, 0 }, {1, 1, 0, 0, 1, 0, 0, 1 }, {1, 1, 0, 0, 1, 0, 1, 0 }, {1, 1, 0, 0, 1, 0, 1, 1 },
{1, 1, 0, 0, 1, 1, 0, 0 }, {1, 1, 0, 0, 1, 1, 0, 1 }, {1, 1, 0, 0, 1, 1, 1, 0 }, {1, 1, 0, 0, 1, 1, 1, 1 },
{1, 1, 0, 1, 0, 0, 0, 0 }, {1, 1, 0, 1, 0, 0, 0, 1 }, {1, 1, 0, 1, 0, 0, 1, 0 }, {1, 1, 0, 1, 0, 0, 1, 1 },
{1, 1, 0, 1, 0, 1, 0, 0 }, {1, 1, 0, 1, 0, 1, 0, 1 }, {1, 1, 0, 1, 0, 1, 1, 0 }, {1, 1, 0, 1, 0, 1, 1, 1 },
{1, 1, 0, 1, 1, 0, 0, 0 }, {1, 1, 0, 1, 1, 0, 0, 1 }, {1, 1, 0, 1, 1, 0, 1, 0 }, {1, 1, 0, 1, 1, 0, 1, 1 },
{1, 1, 0, 1, 1, 1, 0, 0 }, {1, 1, 0, 1, 1, 1, 0, 1 }, {1, 1, 0, 1, 1, 1, 1, 0 }, {1, 1, 0, 1, 1, 1, 1, 1 },
{1, 1, 1, 0, 0, 0, 0, 0 }, {1, 1, 1, 0, 0, 0, 0, 1 }, {1, 1, 1, 0, 0, 0, 1, 0 }, {1, 1, 1, 0, 0, 0, 1, 1 },
{1, 1, 1, 0, 0, 1, 0, 0 }, {1, 1, 1, 0, 0, 1, 0, 1 }, {1, 1, 1, 0, 0, 1, 1, 0 }, {1, 1, 1, 0, 0, 1, 1, 1 },
{1, 1, 1, 0, 1, 0, 0, 0 }, {1, 1, 1, 0, 1, 0, 0, 1 }, {1, 1, 1, 0, 1, 0, 1, 0 }, {1, 1, 1, 0, 1, 0, 1, 1 },
{1, 1, 1, 0, 1, 1, 0, 0 }, {1, 1, 1, 0, 1, 1, 0, 1 }, {1, 1, 1, 0, 1, 1, 1, 0 }, {1, 1, 1, 0, 1, 1, 1, 1 },
{1, 1, 1, 1, 0, 0, 0, 0 }, {1, 1, 1, 1, 0, 0, 0, 1 }, {1, 1, 1, 1, 0, 0, 1, 0 }, {1, 1, 1, 1, 0, 0, 1, 1 },
{1, 1, 1, 1, 0, 1, 0, 0 }, {1, 1, 1, 1, 0, 1, 0, 1 }, {1, 1, 1, 1, 0, 1, 1, 0 }, {1, 1, 1, 1, 0, 1, 1, 1 },
{1, 1, 1, 1, 1, 0, 0, 0 }, {1, 1, 1, 1, 1, 0, 0, 1 }, {1, 1, 1, 1, 1, 0, 1, 0 }, {1, 1, 1, 1, 1, 0, 1, 1 },
{1, 1, 1, 1, 1, 1, 0, 0 }, {1, 1, 1, 1, 1, 1, 0, 1 }, {1, 1, 1, 1, 1, 1, 1, 0 }, {1, 1, 1, 1, 1, 1, 1, 1 },
};

errno_t printBufToFile(char* buf, int n, string fn)
{
	FILE* fptr = NULL;
	errno_t err = fopen_s(&fptr, fn.c_str(), "w");

	if (err || fptr == NULL)
		return err;

# if TIFF
	buf += 8;
# endif

	for (int i = 0; i < n; ++i)
	{
		int r = i * n;
		for (int j = 0; j < n; ++j)
			fprintf(fptr, "%02X ", (unsigned char)buf[r + j]);
		fprintf(fptr, "\n");
	}

	fclose(fptr);
	return 0;
}

int fileSize(string fn)
{
	fstream::pos_type size = 0;
	ifstream ifs(fn, ios::binary | ios::in);

	ifs.seekg(0, ios::end);
	size = ifs.tellg();

	ifs.close();
	return (int)size;
}

void readImage(string fn, char* buf, int size)
{
	ifstream ifs(fn, ios::binary | ios::in);
	ifs.read(buf, size);
	ifs.close();
}

void writeImage(string fn, char* buf, int size)
{
	ofstream ofs(fn, ios::binary | ios::out);
	ofs.write(buf, size);
	ofs.close();
}

void LabelMapGeneration(char* buf, char* lmap, int len, int* freq)
{
	// first row and first column as ref_value.

	int r = 0;
	int idx = 0;
	int x1 = 0;
	int x2 = 0;
	int x3 = 0;
	int px = 0;
	int dif = 0;

	for (int i = 1; i < len; ++i)
	{
		r = i * len;
		for (int j = 1; j < len; ++j)
		{
			idx = r + i;
			x1 = buf[idx - len - 1];
			x2 = buf[idx - len];
			x3 = buf[idx - 1];

			if (x1 <= x2 && x1 <= x3)
			{
				px = x2 > x3 ? x2 : x3;
			}
			else if (x1 >= x2 && x1 >= x3)
			{
				px = x2 < x3 ? x2 : x3;
			}
			else
			{
				px = x2 + x3 - x1;
			}

			// this pixel can embed (t + 1) bits.
			lmap[idx] = pixel2t[buf[idx] ^ px];
			++freq[lmap[idx]];
		}
	}
}

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
	printf("byte2bits[256][8] = {\n");
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

unsigned char bits2byte(vector<unsigned char>& vec, int idx, int n = 8)
{
	unsigned char ret = 0;

	for (int i = 0; i < n; ++i)
	{
		ret <<= 1;
		ret |= vec[idx + i];
	}

	return (ret << (8 - n)) & 0xff;
}

struct Trie
{
	bool isValid;
	int value;
	struct Trie* next[2];
};

// build a valid huffman coding structure
void build(struct Trie* obj, vector<unsigned char>& bits)
{
	assert(obj);
	for (size_t i = 0; i < bits.size(); ++i)
	{
		if (obj->next[bits[i]] == NULL)
			obj->next[bits[i]] = (struct Trie*)calloc(1, sizeof(struct Trie));
		obj = obj->next[bits[i]];
	}

	obj->isValid = true;
}

// set the value of a valid huffman coding structure
void insertValue(struct Trie* obj, vector<unsigned char>& bits, size_t idx_start, size_t idx_end, int val)
{
	for (size_t i = idx_start; i < bits.size() && i < idx_end; ++i)
	{
		int idx = bits[i];
		assert(idx == 1 || idx == 0);

		if (obj->next[idx] == NULL)
			obj->next[idx] = (struct Trie*)calloc(1, sizeof(struct Trie));
		obj = obj->next[idx];
	}

	if (obj->isValid)
		obj->value = val;
}

// return (value, length)
pair<int, int> decode(struct Trie* obj, vector<unsigned char>& bits, size_t idx_start, size_t idx_end)
{
	assert(obj);

	for (size_t i = idx_start; i < bits.size() && i < idx_end; ++i)
	{
		int idx = bits[i];
		assert(idx == 1 || idx == 0);

		if (obj->isValid)
			return make_pair(obj->value, i - idx_start);

		// integrity check
		assert(obj->next[idx]);

		obj = obj->next[idx];
	}

	// not enough bits to decode next value
	return make_pair(-1, -1);
}

int main()
{
	const int size_image = fileSize(COVER_IMG_FN);
	if (size_image <= 0)
		return size_image;
	const int width_image = (int)sqrt(size_image);
	char* byte_image = (char*)calloc(size_image, sizeof(char));
	if (byte_image == NULL)
		return -3;

# if REPORT
	FILE* fptr_report = NULL;
	int ren = fopen_s(&fptr_report, REPORT_FN, "w");
	if (ren || fptr_report == NULL)
		return -2;
	fprintf(fptr_report, "cover image: %s, %d x %d = %d\n\n", COVER_IMG_FN, width_image, width_image, size_image);
# endif

	readImage(COVER_IMG_FN, byte_image, size_image);

	//printBufToFile(buf, len, PRINT_FN);

	//------------------------------------------------------
	// A Label Map Generation
	unsigned char* label_map = (unsigned char*)calloc(size_image, sizeof(unsigned char));
	if (label_map == NULL)
		return -3;
	int cnt_t[9] = { 0 };
	const int width_image1 = width_image + 1;

	// leave first row and first column as ref_value.
	for (int i = 1; i < width_image; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int idx = row + col;

			assert(width_image1 <= idx && idx < size_image);

			unsigned char x1 = byte_image[idx - width_image1];
			unsigned char x2 = byte_image[idx - width_image];
			unsigned char x3 = byte_image[idx - 1];
			unsigned char px = 0;

			if (x1 <= x2 && x1 <= x3)
			{
				px = x2 > x3 ? x2 : x3;
			}
			else if (x1 >= x2 && x1 >= x3)
			{
				px = x2 < x3 ? x2 : x3;
			}
			else
			{
				px = x2 + x3 - x1;
			}

			// this pixel can embed (t + 1) bits.
			label_map[idx] = pixel2t[byte_image[idx] ^ px];
			++cnt_t[label_map[idx]];
		}
	}

	assert(pixel2t[156 ^ 150] == 4);


# if REPORT
	fprintf(fptr_report, "A. Label Map Generation, okay, show only first few.\n\t");
	for (int i = 1; i < 8; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int idx = row + col;
			fprintf(fptr_report, "%2x", label_map[idx]);
		}
		fprintf(fptr_report, "\n\t");
	}
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "Frequency of t\n");
	fprintf(fptr_report, "%6s ", "t");
	for (int i = 0; i <= 8; ++i)
		fprintf(fptr_report, "%6d ", i);
	fprintf(fptr_report, "\n%6s ", "dist");
	for (int i = 0; i <= 8; ++i)
		fprintf(fptr_report, "%6d ", cnt_t[i]);
	fprintf(fptr_report, "\n\n");
# endif

	//------------------------------------------------------
	// B Image Encryption
	// an un-initialized array as pseudo-random matrix
	const char* const byte_rand_keystream = new char[size_image];

	for (int i = 0; i < size_image; ++i)
		byte_image[i] ^= byte_rand_keystream[i];

	writeImage(ENC_IMG_FN, byte_image, size_image);

# if REPORT
	fprintf(fptr_report, "B. Image Encryption, okay, write to %s\n\n", ENC_IMG_FN);
# endif









	//------------------------------------------------------
	// C Huffman Coding                00,   01,  100,  101, 1100, 1101, 1110,11110,11111    
	const unsigned char hcode[9] = { 0x00, 0x01, 0x04, 0x05, 0x0c, 0x0d, 0x0e, 0x1e, 0x1f };
	const unsigned char hclen[9] = { 2, 2, 3, 3, 4, 4, 4, 5, 5 };
	vector< vector<unsigned char> > hbits = { {0, 0}, {0, 1}, {1, 0, 0}, {1, 0, 1}, {1, 1, 0, 0}, {1, 1, 0, 1}, {1, 1, 1, 0}, {1, 1, 1, 1, 0}, {1, 1, 1, 1, 1} };
	int encode[9] = { 0 };
	priority_queue<pair<int, int>> pq;
	int lm = 0;
	int len_secret = 0;



# if 0
	for (int i = 0; i < 9; ++i)
		if (query(hcChecker, hbits[i]))
			printf("%d ", i);

	printf("%d \n", query(hcChecker, { 1, 0 }));
	printf("%d \n", query(hcChecker, { 1, 0, 1 }));
	printf("%d \n", query(hcChecker, { 1, 0, 1, 0, 0, 1 }));
# endif

	for (int t = 0; t <= 8; ++t)
		pq.push(make_pair(cnt_t[t], t));

	for (int i = 0; i <= 8; ++i)
	{
		auto p = pq.top();
		pq.pop();
		encode[p.second] = i;
		lm += p.first * hclen[i];
	}

	unsigned char* bits_hencode_labmap = (unsigned char*)calloc(lm, sizeof(unsigned char));
	if (bits_hencode_labmap == NULL)
		return -3;
	int idx_hencode_labmap = 0;

	for (int i = 1; i < width_image; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int t = label_map[row + col];
			int code_idx = encode[t];
			int code_len = hclen[code_idx];
			for (int k = 0; k < code_len; ++k)
			{
				bits_hencode_labmap[idx_hencode_labmap++] = hbits[code_idx][k];
			}

			if (t < 7)
			{
				len_secret += t + 1 - code_len;
			}
			else
			{
				len_secret += 8 - code_len;
			}
		}
	}

	assert(idx_hencode_labmap == lm);

# if REPORT
	fprintf(fptr_report, "C. Huffman Coding, okay, %d, LM = %d, extra-capacity = %d + 52\n", lm + len_secret, lm, len_secret - 52);
	fprintf(fptr_report, "Huffman coding\n");
	fprintf(fptr_report, "%6s %6s %6s %6s %6s %6s %6s %6s %6s %6s \n",
		"", "00", "01", "100", "101", "1100", "1101", "1110", "11110", "11111");
	fprintf(fptr_report, "%6s ", "");
	for (int i = 0; i <= 8; ++i)
		fprintf(fptr_report, "%6X ", hcode[i]);
	fprintf(fptr_report, "\n%6s ", "len");
	for (int i = 0; i <= 8; ++i)
		fprintf(fptr_report, "%6d ", hclen[i]);
	fprintf(fptr_report, "\n\n");

	fprintf(fptr_report, "coding rules of t\n");
	fprintf(fptr_report, "%6s ", "t");
	for (int i = 0; i <= 8; ++i)
		fprintf(fptr_report, "%6d ", i);
	fprintf(fptr_report, "\n%6s ", "code");
	for (int i = 0; i <= 8; ++i)
		fprintf(fptr_report, "%6X ", hcode[encode[i]]);
	fprintf(fptr_report, "\n%6s ", "len");
	for (int i = 0; i <= 8; ++i)
		fprintf(fptr_report, "%6d ", hclen[encode[i]]);
	fprintf(fptr_report, "\n\n");
# endif





	//------------------------------------------------------
	// D Label Map Embedding

	const int len_hcrule = 32;
	const int len_hlmap = 20;
	const int len_hc1st = len_hcrule + len_hlmap;
	len_secret -= len_hc1st;

	const int len_sideinfo = len_hc1st + lm;
	const int len_ref = (width_image + width_image - 1) * 8;
	const int len_ai = len_sideinfo + len_ref;
	const int len_ai_bytes = (int)ceil(len_ai / 8);
	const int len_embed = len_ai + len_secret;

	// an un-initialized array as random secret bits
	unsigned char* const bits_rand_secret = new unsigned char[len_secret];
	vector<unsigned char> bits_embed(len_embed);
	int idx_embed = 0;

	//cout << lm + len_secret + (1023 * 8) << endl;

	//side info 1, Huffman coding rule - 0 to 8 - 32 bits
	for (int t = 0; t <= 8; ++t)
	{
		int idx = encode[t];
		int len = hclen[idx];

		for (int j = 0; j < len; ++j)
			bits_embed[idx_embed++] = hbits[idx][j];
	}

# if REPORT
	fprintf(fptr_report, "D. Label Map Embedding\n");
	fprintf(fptr_report, "D1. Side Info, Huffman coding rule 32 bits\n\t");
	for (int i = 0; i < 32; ++i)
		fprintf(fptr_report, "%d ", bits_embed[i]);
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tprepare embeding bits %d \n\n", idx_embed);
# endif

	//side info 2, length of encode(lab map) => lm, 20 bits
	unsigned int lmask = 1 << 19;
	while (lmask)
	{
		bits_embed[idx_embed++] = (lm & lmask) ? 1 : 0;
		lmask >>= 1;
	}

# if REPORT
	fprintf(fptr_report, "D2. Side Info, length of hEncode(LabelMap) 20 bits\n\t");
	for (int i = 0; i < 20; ++i)
		fprintf(fptr_report, "%d ", bits_embed[len_hcrule + i]);
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tprepare embeding bits %d \n\n", idx_embed);
# endif

	//side info 3, encode(lab map) => lm bits
	for (int i = 0; i < lm; ++i)
	{
		bits_embed[idx_embed++] = bits_hencode_labmap[i];
	}

	assert(idx_embed == len_sideinfo);

# if REPORT
	fprintf(fptr_report, "D3. Side Info, hEncode(LabelMap) LM bits, okay, only show first few bits\n\t");
	for (int i = 0; i < 512; ++i)
		fprintf(fptr_report, "%d %s", bits_embed[len_hc1st + i], i % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tprepare embeding bits %d \n\n", idx_embed);
# endif

	// reference values
	// first row
	for (int col = 0; col < width_image; ++col)
	{
		for (int j = 0; j < 8; ++j)
		{
			bits_embed[idx_embed++] = byte2bits[(unsigned char)byte_image[col]][j];
		}
	}

# if REPORT
	fprintf(fptr_report, "D4. reference value, 1st row of enc(image), 512 bytes\n\t");
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "%02X %s", (unsigned char)byte_image[col], col % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tprepare embeding bits %d \n\n", idx_embed);
# endif

	// first col, but except (0, 0)
	for (int row = 1; row < width_image; ++row)
	{
		for (int j = 0; j < 8; ++j)
		{
			bits_embed[idx_embed++] = byte2bits[(unsigned char)byte_image[row * width_image]][j];
		}
	}

	assert(idx_embed == len_ai);

# if REPORT
	fprintf(fptr_report, "D5. reference value, 1st col of enc(image), 511 bytes\n\t");
	fprintf(fptr_report, "   ");
	for (int row = 1; row < width_image; ++row)
		fprintf(fptr_report, "%02X %s", (unsigned char)byte_image[row * width_image], row % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tprepare embeding bits %d \n\n", idx_embed);
	fprintf(fptr_report, "total auxiliary info size = %d bits = %d bytes \n\n", idx_embed, len_ai_bytes);
# endif


	//---------------------
# if DB_SEC
	for (int i = 0; i < 16; ++i)
		bits_rand_secret[i] = i % 2;
# endif
	//---------------------

	for (int i = 0; i < len_secret; ++i)
	{
		bits_embed[idx_embed] = (unsigned char)(bits_rand_secret[i] & 0x01);
		assert(bits_embed[idx_embed] == 0 || bits_embed[idx_embed] == 1);
		idx_embed++;
	}

	assert(idx_embed == len_embed);

# if REPORT
	fprintf(fptr_report, "D6. random secret bits, only show first few bits\n\t");
	for (int i = 0; i < 512; ++i)
		fprintf(fptr_report, "%d %s", bits_embed[len_ai + i], i % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tprepare embeding bits %d \n\n", idx_embed);

# endif

	// embed auxiliary info into 1st row and 1st col, 8 bits * 1023
	idx_embed = 0;
	for (int col = 0; col < width_image; ++col)
	{
		byte_image[col] = bits2byte(bits_embed, idx_embed);
		idx_embed += 8;
	}

# if REPORT
	fprintf(fptr_report, "D7. embed into the 1st row of enc(image), 512 bytes\n\t");
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "%02X %s", (unsigned char)byte_image[col], col % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tembeding progress %d / %d bits \n\n", idx_embed, len_ai);
# endif

	// first col, but except (0, 0)
	for (int row = 1; row < width_image; ++row)
	{
		byte_image[row * width_image] = bits2byte(bits_embed, idx_embed);
		idx_embed += 8;
	}

# if REPORT
	fprintf(fptr_report, "D8. embed into the 1st col of enc(image), 511 bytes\n\t");
	fprintf(fptr_report, "   ");
	for (int row = 1; row < width_image; ++row)
		fprintf(fptr_report, "%02X %s", (unsigned char)byte_image[row * width_image], row % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tembeding progress %d / %d bits \n\n", idx_embed, len_ai);

	fprintf(fptr_report, "D9. embed into rest of the enc(image)\n");

	bool printmark = true;
# endif

# if DB_SEC
	const int tarCol = 10;
# endif

	for (int i = 1; i < width_image; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int idx = row + col;
			int t = label_map[idx];

			assert(idx_embed <= len_embed);

# if DB_SEC
			if (idx_embed > len_ai && col == tarCol)
			{
				printf("%3d p %02x t %d ", i, (unsigned char)byte_image[idx], t);
				for (int d = 0; d <= t && d < 8; ++d)
					printf("%d ", bits_embed[idx_embed + d]);
			}
# endif

			if (t >= 7)
			{
# if DB_SEC
				if (idx_embed > len_ai && col == tarCol)
				{
					printf("total %02x ", bits2byte(bits_embed, idx_embed));
				}
# endif

				byte_image[idx] = bits2byte(bits_embed, idx_embed);
				idx_embed += 8;
			}
			else
			{
# if DB_SEC
				if (idx_embed > len_ai && col == tarCol)
				{
					printf("left %02x msk %02x right %02x ", bits2byte(bits_embed, idx_embed, t + 1), mask[t] - 1, (byte_image[idx] & (mask[t] - 1)));
				}
# endif

				byte_image[idx] = bits2byte(bits_embed, idx_embed, t + 1) | (byte_image[idx] & (mask[t] - 1));
				//assert(((unsigned char)byte_image[idx] % mask[t]) < mask[t]);
				//byte_image[idx] = (unsigned char)((unsigned char)(byte_image[idx] % mask[t]) + (unsigned char)bits2byte(bits_embed, idx_embed, t + 1));
				idx_embed += t + 1;
			}
# if DB_SEC
			if (idx_embed > len_ai && col == tarCol)
			{
				printf(" ret %02x\n", (unsigned char)byte_image[idx]);
			}
#endif
		}

# if 0
		fprintf(fptr_report, "\t(%3d) progress %6d / %6d / %6d bits %s\n", i, idx_ai, len_ai, len_embed, (printmark && idx_ai >= len_ai) ? "*" : "");
		if (printmark && idx_ai >= len_ai)
			printmark = false;
# endif
	}

	writeImage(STEGO_IMG_FN, byte_image, size_image);

# if REPORT
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "E. write to %s\n", STEGO_IMG_FN);
	fprintf(fptr_report, "\n");
# endif











	vector<unsigned char> bit_extract(size_image * 8);
	int idx_extra = 0;

	// first row
	for (int col = 0; col < width_image; ++col)
	{
		//printf("%02X : ", (unsigned char)byte_image[col]);
		for (int j = 0; j < 8; ++j)
		{
			bit_extract[idx_extra] = byte2bits[(unsigned char)byte_image[col]][j];
			//printf("%x ", (unsigned char)bit_extract[idx_extra]);
			assert(bit_extract[idx_extra] == 1 || bit_extract[idx_extra] == 0);
			idx_extra++;
		}
		//printf("\n");
	}

# if REPORT
	fprintf(fptr_report, "F. Data Extraction and Image Recovery\n\n");
	fprintf(fptr_report, "F1. extract 1st row\n\t");
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "%02X %s", (unsigned char)byte_image[col], col % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n\t");
	fprintf(fptr_report, "the first few bits\n\t");
	for (int i = 0; i < 128; ++i)
		fprintf(fptr_report, "%x %s", bit_extract[i], i % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\textract %d bits\n", idx_extra);
	fprintf(fptr_report, "\n");
# endif

	// first col, but except (0, 0)
	for (int row = 1; row < width_image; ++row)
	{
		for (int j = 0; j < 8; ++j)
		{
			bit_extract[idx_extra] = byte2bits[(unsigned char)byte_image[row * width_image]][j];
			assert(bit_extract[idx_extra] == 1 || bit_extract[idx_extra] == 0);
			idx_extra++;
		}
	}

# if REPORT
	fprintf(fptr_report, "F2. extract 1st col, except (0, 0)\n\t");
	fprintf(fptr_report, "   ");
	for (int row = 1; row < width_image; ++row)
		fprintf(fptr_report, "%02X %s", (unsigned char)byte_image[row * width_image], row % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\textract %d bits\n", idx_extra);
	fprintf(fptr_report, "\n");

	fprintf(fptr_report, "F3. recover Huffman coding rules\n");
# endif

	struct Trie* hCoder = (struct Trie*)calloc(1, sizeof(struct Trie));
	for (int i = 0; i < 9; ++i)
		build(hCoder, hbits[i]);

	int idx_extra_proceed = 0;
	for (int i = 0; i <= 8; ++i)
	{
		pair<int, int> p = decode(hCoder, bit_extract, idx_extra_proceed, idx_extra_proceed + 6);
		assert(p.first != -1);
		assert(p.second);

		int len_cur_hcrule = p.second;

		insertValue(hCoder, bit_extract, idx_extra_proceed, idx_extra_proceed + len_cur_hcrule, i);

# if REPORT
		fprintf(fptr_report, "\tproceed %2d bits\t %d : ", idx_extra_proceed, i);
		for (int i = 0; i < len_cur_hcrule; ++i)
			fprintf(fptr_report, "%d ", bit_extract[idx_extra_proceed + i]);
		fprintf(fptr_report, "\n");
# endif

		idx_extra_proceed += len_cur_hcrule;
	}

	int len_extra_hcencode_lm = 0;

	for (int i = 0; i < 20; ++i)
	{
		len_extra_hcencode_lm <<= 1;
		len_extra_hcencode_lm |= bit_extract[idx_extra_proceed++];
	}

	assert(len_extra_hcencode_lm == lm);

# if REPORT
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "F4. recover length of hEncode(label map) = %d, proceed %d bits\n", len_extra_hcencode_lm, idx_extra_proceed);
	fprintf(fptr_report, "\n");
# endif

	unsigned char* label_map_extra = (unsigned char*)calloc(size_image, sizeof(unsigned char));
	if (label_map == NULL)
		return -3;

	int idx_row_extra_lm = 1;
	int idx_col_extra_lm = 1;

	// re-construct lable map
	while (true)
	{
		pair<int, int> p = decode(hCoder, bit_extract, idx_extra_proceed, idx_extra);
		if (p.first == -1)
			break;

		assert(0 <= p.first && p.first <= 8);
		assert(p.second);

		unsigned char t = p.first;
		int idx_extra_lm = idx_row_extra_lm * width_image + idx_col_extra_lm;
		label_map_extra[idx_extra_lm] = (unsigned char)p.first;

		assert(label_map_extra[idx_extra_lm] == label_map[idx_extra_lm]);
#if 0
		if (label_map_extra[idx_extra_lm] != label_map[idx_extra_lm])
		{
			printf("%d %d, %d %d \n", idx_row_extra_lm, idx_col_extra_lm, label_map_extra[idx_extra_lm], label_map[idx_extra_lm]);
			break;
		}
# endif

# if DB_SEC
		if (idx_extra > len_ai && idx_col_extra_lm == tarCol)
		{
			printf("%d p %02x t %d ", idx_row_extra_lm, (unsigned char)byte_image[idx_extra_lm], t);
		}
# endif

		for (int j = 0; j <= t && j < 8; ++j)
		{
			bit_extract[idx_extra] = (unsigned char)((byte_image[idx_extra_lm] << j) & 0x80) ? 1 : 0;
			assert(bit_extract[idx_extra] == 1 || bit_extract[idx_extra] == 0);
# if DB_SEC
			if (idx_extra > len_ai && idx_col_extra_lm == tarCol)
			{
				printf("%x ", bit_extract[idx_extra]);
			}
# endif
			++idx_extra;
		}

# if DB_SEC
		if (idx_extra > len_ai && idx_col_extra_lm == tarCol)
		{
			printf("\n");
		}
# endif

		idx_extra_proceed += p.second;

		if (idx_col_extra_lm + 1 == width_image)
		{
			idx_col_extra_lm = 1;
			++idx_row_extra_lm;

			//printf("%d %d, ", idx_row_extra_lm, t);

			if (idx_row_extra_lm == width_image)
				break;
		}
		else
			++idx_col_extra_lm;
	}

# if REPORT
	fprintf(fptr_report, "F5. recover Label Map, proceed %d / %d bits, show only first few.\n\t", idx_extra_proceed, idx_extra);
	for (int i = 1; i < 8; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int idx = row + col;
			fprintf(fptr_report, "%2d", label_map_extra[idx]);
		}
		fprintf(fptr_report, "\n\t");
	}
	fprintf(fptr_report, "\n");
# endif

	for (int col = 0; col < width_image; ++col)
	{
		byte_image[col] = (unsigned char)bits2byte(bit_extract, idx_extra_proceed);
		idx_extra_proceed += 8;
	}

# if REPORT
	fprintf(fptr_report, "F6. recover first row, proceed %d / %d bits, show only first few.\n\t", idx_extra_proceed, idx_extra);
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "%02X %s", (unsigned char)byte_image[col], col % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
# endif

	for (int row = 1; row < width_image; ++row)
	{
		int idx = row * width_image + 1;
		byte_image[idx] = (unsigned char)bits2byte(bit_extract, idx_extra_proceed);
		idx_extra_proceed += 8;
	}

# if REPORT
	fprintf(fptr_report, "F7. recover first col, except(0, 0), proceed %d / %d bits, show only first few.\n\t", idx_extra_proceed, idx_extra);
	fprintf(fptr_report, "   "); 
	for (int row = 1; row < width_image; ++row)
	{
		int idx = row * width_image + 1;
		fprintf(fptr_report, "%02X %s", (unsigned char)byte_image[idx], row % 64 == 63 ? "\n\t" : "");
	}
	fprintf(fptr_report, "\n");
# endif

	vector<unsigned char> bit_secret_extra(len_secret);

	for (size_t i = 0; i < bit_secret_extra.size(); ++i)
	{
		bit_secret_extra[i] = (unsigned char)bit_extract[idx_extra_proceed++];
		assert(bit_secret_extra[i] == (bits_rand_secret[i] & 0x01));
	}

# if REPORT
	fprintf(fptr_report, "F8. recover secret %d / %d bits, show only first few.\n\t", idx_extra_proceed, idx_extra);
	for (size_t i = 0; i < 512; ++i)
		fprintf(fptr_report, "%2d %s", bit_secret_extra[i], i % 64 == 63 ? "\n\t" : "");

	fprintf(fptr_report, "\n");
# endif



















# if REPORT
	fclose(fptr_report);
# endif

	free(bits_hencode_labmap);
	delete[] byte_rand_keystream;
	free(label_map);
	free(byte_image);
	return 0;
}

