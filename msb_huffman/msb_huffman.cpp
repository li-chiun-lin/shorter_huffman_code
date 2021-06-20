#include <iostream>
#include <fstream>
#include <queue>
#include <assert.h>
//#include <cassert>
//#include <Wincodec.h>

# define DB_SEC 0

# define REPORT 1
# define PRINT 1

# define FN_COVER_IMG "../image/lena512.raw"
//# define COVER_IMG_FN "../image/Lena.tiff"
//# define TIFF 1

# define FN_ENC_IMG FN_COVER_IMG"_encrypt.raw"
# define FN_STE_IMG FN_COVER_IMG"_stego.raw"
# define FN_RES_IMG FN_COVER_IMG"_restore.raw"

# if REPORT
#	define FN_REPORT FN_COVER_IMG"_report.txt"
# endif

# if PRINT
#	define FN_COVER_IMAGE_HEXDUMP   FN_COVER_IMG"_hex.txt"
#	define FN_RESTORE_IMAGE_HEXDUMP FN_COVER_IMG"_restore_hex.txt"
# endif

using namespace std;

const static vector<unsigned char> pixel2t = {
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

const static vector< vector<unsigned char> > byte2bits = {
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

const static vector<unsigned char> mask = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };


errno_t printByteToFile(vector<unsigned char>& byte_buf, int len, string file_name)
{
	FILE* fptr = NULL;
	errno_t err = fopen_s(&fptr, file_name.c_str(), "w");

	if (err || fptr == NULL)
		return err;

	for (int i = 0; i < len; ++i)
	{
		int r = i * len;
		for (int j = 0; j < len; ++j)
			fprintf(fptr, "%02X ", byte_buf[r + j]);
		fprintf(fptr, "\n");
	}

	fclose(fptr);
	return 0;
}


vector<unsigned char> readImage(string file_name)
{
	ifstream file(file_name, ios::binary);
	streampos fileSize;

	// Stop eating new lines in binary mode!!!
	file.unsetf(ios::skipws);

	file.seekg(0, ios::end);
	fileSize = file.tellg();
	file.seekg(0, ios::beg);

	// reserve capacity
	vector<unsigned char> byte_buf;
	byte_buf.reserve((size_t)fileSize);

	// read the data:
	byte_buf.insert(byte_buf.begin(),
		istream_iterator<unsigned char>(file),
		istream_iterator<unsigned char>());

	return byte_buf;
}

void writeImage(string file_name, vector<unsigned char>& byte_buf)
{
	ofstream ofs(file_name, ios::binary | ios::out);
	ofs.write((const char*)byte_buf.data(), byte_buf.size());
	ofs.close();
}

unsigned char bits2byte(vector<unsigned char>& vec, int idx, int n = 8)
{
	unsigned char ret = 0;

	for (int i = 0; i < n; ++i)
	{
		ret <<= 1;
		ret |= vec[idx + i];
	}

	return (ret << (8 - n));
}

struct Trie
{
	bool isValid;
	int value;
	struct Trie* next[2];
};

// build a valid huffman coding structure
void build(struct Trie* obj, const vector<unsigned char>& bits)
{
	assert(obj);

	if (obj == NULL)
		return; 

	for (size_t i = 0; i < bits.size(); ++i)
	{
		if (obj && obj->next[bits[i]] == NULL)
			obj->next[bits[i]] = (struct Trie*)calloc(1, sizeof(struct Trie));

		if (obj)
			obj = obj->next[bits[i]];
	}

	if (obj)
		obj->isValid = true;
}

// set the value of a valid huffman coding structure
void insertValue(struct Trie* obj, vector<unsigned char>& bits, size_t idx_start, size_t idx_end, int val)
{
	for (size_t i = idx_start; i < bits.size() && i < idx_end; ++i)
	{
		int idx = bits[i];
		assert(idx == 1 || idx == 0);

		if (obj && obj->next[idx] == NULL)
			obj->next[idx] = (struct Trie*)calloc(1, sizeof(struct Trie));

		if (obj)
		obj = obj->next[idx];
	}

	if (obj && obj->isValid)
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
	vector<unsigned char> byte_image = readImage(FN_COVER_IMG);
	const int size_image = byte_image.size();
	const int width_image = (int)sqrt(size_image);
	
	srand(0);

# if REPORT
	FILE* fptr_report = NULL;
	int ren = fopen_s(&fptr_report, FN_REPORT, "w");
	if (ren || fptr_report == NULL)
		return -2;
	fprintf(fptr_report, "cover image: %s, %d x %d = %d\n\n", FN_COVER_IMG, width_image, width_image, size_image);
# endif


# if PRINT
	printByteToFile(byte_image, width_image, FN_COVER_IMAGE_HEXDUMP);
# endif

	//------------------------------------------------------
	// A Label Map Generation
	vector<unsigned char> val_labelmap(size_image);
	vector<int> cnt_t(9);

	// leave first row and first column as ref_value.
	for (int i = 1; i < width_image; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int idx = row + col;
			assert(idx < size_image);

			unsigned char x1 = byte_image[idx - width_image - 1];
			unsigned char x2 = byte_image[idx - width_image];
			unsigned char x3 = byte_image[idx - 1];
			unsigned char px = 0;

			if (x1 <= x2 && x1 <= x3)
				px = x2 > x3 ? x2 : x3;
			else if (x1 >= x2 && x1 >= x3)
				px = x2 < x3 ? x2 : x3;
			else
			{
				px = x2 + x3 - x1;
				assert(0 <= px && px <= 255);
			}

			// this pixel can embed (t + 1) bits.
			val_labelmap[idx] = pixel2t[byte_image[idx] ^ px];
			++cnt_t[val_labelmap[idx]];
		}
	}

	assert(pixel2t[156 ^ 150] == 4);


# if REPORT
	fprintf(fptr_report, "A. Label Map Generation, show only first few.\n\t");
	for (int i = 1; i < 8; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
			fprintf(fptr_report, "%2x", val_labelmap[row + col]);
		fprintf(fptr_report, "\n\t");
	}
	fprintf(fptr_report, "\n\tDistribution of t\n");
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
	vector<unsigned char> byte_rand_keystream(size_image);
	generate(byte_rand_keystream.begin(), byte_rand_keystream.end(), rand);

	for (int i = 0; i < size_image; ++i)
		byte_image[i] ^= byte_rand_keystream[i];

	writeImage(FN_ENC_IMG, byte_image);

# if REPORT
	fprintf(fptr_report, "B. Image Encryption, write to %s\n\n", FN_ENC_IMG);
# endif









	//------------------------------------------------------
	// C Huffman Coding                00,   01,  100,  101, 1100, 1101, 1110,11110,11111    
	const int size_hcode = 9;
	const vector<unsigned char> val_hcode = { 0x00, 0x01, 0x04, 0x05, 0x0c, 0x0d, 0x0e, 0x1e, 0x1f };
	const vector<unsigned char> len_hcode = { 2, 2, 3, 3, 4, 4, 4, 5, 5 };
	const vector< vector<unsigned char> > bits_hcode = { {0, 0}, {0, 1}, {1, 0, 0}, {1, 0, 1}, {1, 1, 0, 0}, {1, 1, 0, 1}, {1, 1, 1, 0}, {1, 1, 1, 1, 0}, {1, 1, 1, 1, 1} };
	vector<int> h_encode(size_hcode);
	priority_queue<pair<int, int>> pq;
	int len_h_encoded_labelmap = 0;
	int len_secret = 0;


	for (int t = 0; t < size_hcode; ++t)
		pq.push(make_pair(cnt_t[t], t));

	for (int i = 0; i < size_hcode; ++i)
	{
		pair<int, int> p = pq.top();
		pq.pop();
		h_encode[p.second] = i;
		len_h_encoded_labelmap += p.first * len_hcode[i];
	}

	vector<unsigned char> bits_h_encode_labelmap(len_h_encoded_labelmap);
	int idx_h_encode_labelmap = 0;

	for (int i = 1; i < width_image; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int t = val_labelmap[row + col];
			int idx_code = h_encode[t];
			int len_code = len_hcode[idx_code];
			for (int k = 0; k < len_code; ++k)
			{
				bits_h_encode_labelmap[idx_h_encode_labelmap++] = bits_hcode[idx_code][k];
			}

			if (t < 7)
			{
				len_secret += t + 1 - len_code;
			}
			else
			{
				len_secret += 8 - len_code;
			}
		}
	}

	assert(idx_h_encode_labelmap == len_h_encoded_labelmap);

# if REPORT
	fprintf(fptr_report, "C. Huffman Coding, okay, %d, LM = %d, extra-capacity = %d + 52\n", len_h_encoded_labelmap + len_secret, len_h_encoded_labelmap, len_secret - 52);
	fprintf(fptr_report, "\tcoding rules of t\n");
	fprintf(fptr_report, "%6s ", "t");
	for (int i = 0; i < size_hcode; ++i)
		fprintf(fptr_report, "%6d ", i);
	fprintf(fptr_report, "\n%6s ", "code");
	for (int i = 0; i < size_hcode; ++i)
		fprintf(fptr_report, "%6X ", val_hcode[h_encode[i]]);
	fprintf(fptr_report, "\n%6s ", "len");
	for (int i = 0; i < size_hcode; ++i)
		fprintf(fptr_report, "%6d ", len_hcode[h_encode[i]]);
	fprintf(fptr_report, "\n\n");
# endif





	//------------------------------------------------------
	// D Label Map Embedding

	const int len_hc_rule = 32;
	const int len_len_h_encoded_lm = 20;
	const int len_hc_header = len_hc_rule + len_len_h_encoded_lm;
	len_secret -= len_hc_header;

	const int len_sideinfo = len_hc_header + len_h_encoded_labelmap;
	const int len_ref = (width_image + width_image - 1) * 8;
	const int len_auxinfo = len_sideinfo + len_ref;
	const int len_embed = len_auxinfo + len_secret;

	// an un-initialized array as random secret bits
	vector<unsigned char> bits_rand_secret(len_secret);
	generate(bits_rand_secret.begin(), bits_rand_secret.end(), rand);

	vector<unsigned char> bits_embed(len_embed);
	int idx_embed = 0;

	//side info 1, Huffman coding rule - 0 to 8 - 32 bits
	for (int t = 0; t < size_hcode; ++t)
	{
		int idx = h_encode[t];
		int len = len_hcode[idx];

		for (int j = 0; j < len; ++j)
			bits_embed[idx_embed++] = bits_hcode[idx][j];
	}

	assert(idx_embed == len_hc_rule);

# if REPORT
	fprintf(fptr_report, "D. Label Map Embedding\n");
	fprintf(fptr_report, "D1. Side Info, Huffman coding rule 32 bits\n\t");
	for (int i = 0; i < len_hc_rule; ++i)
		fprintf(fptr_report, "%d ", bits_embed[i]);
	fprintf(fptr_report, "\n\tprepare embeding bits %d \n\n", idx_embed);
# endif

	//side info 2, length of encode(lab map) => lm, 20 bits
	unsigned int bit_long_mask = 1 << (len_len_h_encoded_lm - 1);
	while (bit_long_mask)
	{
		bits_embed[idx_embed++] = (len_h_encoded_labelmap & bit_long_mask) ? 1 : 0;
		bit_long_mask >>= 1;
	}

	assert(idx_embed == len_hc_header);

# if REPORT
	fprintf(fptr_report, "D2. Side Info, length of hEncode(LabelMap) 20 bits\n\t");
	for (int i = 0; i < len_len_h_encoded_lm; ++i)
		fprintf(fptr_report, "%d ", bits_embed[len_hc_rule + i]);
	fprintf(fptr_report, "\n\tprepare embeding bits %d \n\n", idx_embed);
# endif

	//side info 3, encode(lab map) => lm bits
	for (int i = 0; i < len_h_encoded_labelmap; ++i)
	{
		bits_embed[idx_embed++] = bits_h_encode_labelmap[i];
	}

	assert(idx_embed == len_sideinfo);

# if REPORT
	fprintf(fptr_report, "D3. Side Info, hEncode(LabelMap) LM bits, okay, only show first few bits\n\t");
	for (int i = 0; i < 512; ++i)
		fprintf(fptr_report, "%d %s", bits_embed[len_hc_header + i], i % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tprepare embeding bits %d \n\n", idx_embed);
# endif

	// reference values
	// first row
	for (int col = 0; col < width_image; ++col)
	{
		for (int j = 0; j < 8; ++j)
		{
			bits_embed[idx_embed++] = byte2bits[byte_image[col]][j];
		}
	}

# if REPORT
	fprintf(fptr_report, "D4. reference value, 1st row of enc(image), 512 bytes\n\t");
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "%02X %s", byte_image[col], col % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tprepare embeding bits %d \n\n", idx_embed);
# endif

	// first col, but except (0, 0)
	for (int row = 1; row < width_image; ++row)
	{
		for (int j = 0; j < 8; ++j)
		{
			bits_embed[idx_embed++] = byte2bits[byte_image[row * width_image]][j];
		}
	}

	assert(idx_embed == len_auxinfo);

# if REPORT
	fprintf(fptr_report, "D5. reference value, 1st col of enc(image), 511 bytes\n\t");
	fprintf(fptr_report, "   ");
	for (int row = 1; row < width_image; ++row)
		fprintf(fptr_report, "%02X %s", byte_image[row * width_image], row % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tprepare embeding bits %d \n\n", idx_embed);
	fprintf(fptr_report, "total auxiliary info size = %d bits\n\n", idx_embed);
# endif


	//---------------------
# if DB_SEC
	for (int i = 0; i < 16; ++i)
		bits_rand_secret[i] = i % 2;
# endif
	//---------------------

	for (int i = 0; i < len_secret; ++i)
	{
		bits_embed[idx_embed] = (bits_rand_secret[i] & 0x01);
		assert(bits_embed[idx_embed] == 0 || bits_embed[idx_embed] == 1);
		idx_embed++;
	}

	assert(idx_embed == len_embed);

# if REPORT
	fprintf(fptr_report, "D6. random secret bits, only show first few bits\n\t");
	for (int i = 0; i < 512; ++i)
		fprintf(fptr_report, "%d %s", bits_embed[len_auxinfo + i], i % 64 == 63 ? "\n\t" : "");
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
	fprintf(fptr_report, "D8. embed into the 1st col of enc(image), 511 bytes\n\t");
	fprintf(fptr_report, "   ");
	for (int row = 1; row < width_image; ++row)
		fprintf(fptr_report, "%02X %s", byte_image[row * width_image], row % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\tembeding progress %d / %d bits \n\n", idx_embed, len_auxinfo);

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
			int t = val_labelmap[idx];

			assert(idx_embed <= len_embed);

			if (t >= 7)
			{
				byte_image[idx] = bits2byte(bits_embed, idx_embed);
				idx_embed += 8;
			}
			else
			{
				byte_image[idx] = bits2byte(bits_embed, idx_embed, t + 1) | (byte_image[idx] & (mask[t] - 1));
				idx_embed += t + 1;
			}
		}

# if 0
		fprintf(fptr_report, "\t(%3d) progress %6d / %6d / %6d bits %s\n", i, idx_ai, len_ai, len_embed, (printmark && idx_ai >= len_ai) ? "*" : "");
		if (printmark && idx_ai >= len_ai)
			printmark = false;
# endif
	}

	writeImage(FN_STE_IMG, byte_image);

# if REPORT
	fprintf(fptr_report, "\nE. write to %s\n\n", FN_STE_IMG);
# endif











	vector<unsigned char> bit_extract(size_image * 8);
	int idx_ex = 0;

	// first row
	for (int col = 0; col < width_image; ++col)
	{
		for (int j = 0; j < 8; ++j)
		{
			bit_extract[idx_ex] = byte2bits[byte_image[col]][j];
			idx_ex++;
		}
	}

# if REPORT
	fprintf(fptr_report, "F. Data Extraction and Image Recovery\n\n");
	fprintf(fptr_report, "F1. extract 1st row\n\t");
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "%02X %s", byte_image[col], col % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n\t");
	fprintf(fptr_report, "the first few bits\n\t");
	for (int i = 0; i < 128; ++i)
		fprintf(fptr_report, "%x %s", bit_extract[i], i % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "\textract %d bits\n", idx_ex);
	fprintf(fptr_report, "\n");
# endif

	// first col, but except (0, 0)
	for (int row = 1; row < width_image; ++row)
	{
		for (int j = 0; j < 8; ++j)
		{
			bit_extract[idx_ex] = byte2bits[byte_image[row * width_image]][j];
			assert(bit_extract[idx_ex] == 1 || bit_extract[idx_ex] == 0);
			idx_ex++;
		}
	}

# if REPORT
	fprintf(fptr_report, "F2. extract 1st col, except (0, 0)\n\t");
	fprintf(fptr_report, "   ");
	for (int row = 1; row < width_image; ++row)
		fprintf(fptr_report, "%02X %s",byte_image[row * width_image], row % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n\textract %d bits\n\n", idx_ex);

	fprintf(fptr_report, "F3. recover Huffman coding rules\n");
# endif

	struct Trie* hCoder = (struct Trie*)calloc(1, sizeof(struct Trie));
	for (int i = 0; i < 9; ++i)
		build(hCoder, bits_hcode[i]);

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

	assert(len_extra_hcencode_lm == len_h_encoded_labelmap);

# if REPORT
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "F4. recover length of hEncode(label map) = %d, proceed %d bits\n", len_extra_hcencode_lm, idx_extra_proceed);
	fprintf(fptr_report, "\n");
# endif

	vector<unsigned char> val_labelmap_ex(size_image);

	int idx_row_ex_lm = 1;
	int idx_col_ex_lm = 1;

	// re-construct lable map
	while (true)
	{
		pair<int, int> p = decode(hCoder, bit_extract, idx_extra_proceed, idx_ex);
		if (p.first == -1)
			break;

		assert(0 <= p.first && p.first <= 8);
		assert(p.second);

		unsigned char t = p.first;
		int idx_extra_lm = idx_row_ex_lm * width_image + idx_col_ex_lm;
		val_labelmap_ex[idx_extra_lm] = (unsigned char)p.first;

		assert(val_labelmap_ex[idx_extra_lm] == val_labelmap[idx_extra_lm]);



		for (int j = 0; j <= t && j < 8; ++j)
		{
			bit_extract[idx_ex] = ((byte_image[idx_extra_lm] << j) & 0x80) ? 1 : 0;
			assert(bit_extract[idx_ex] == 1 || bit_extract[idx_ex] == 0);

			++idx_ex;
		}

		idx_extra_proceed += p.second;

		if (idx_col_ex_lm + 1 == width_image)
		{
			idx_col_ex_lm = 1;
			++idx_row_ex_lm;

			if (idx_row_ex_lm == width_image)
				break;
		}
		else
			++idx_col_ex_lm;
	}

# if REPORT
	fprintf(fptr_report, "F5. recover Label Map, proceed %d / %d bits, show only first few.\n\t", idx_extra_proceed, idx_ex);
	for (int i = 1; i < 8; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int idx = row + col;
			fprintf(fptr_report, "%2d", val_labelmap_ex[idx]);
		}
		fprintf(fptr_report, "\n\t");
	}
	fprintf(fptr_report, "\n");
# endif

	for (int col = 0; col < width_image; ++col)
	{
		byte_image[col] = bits2byte(bit_extract, idx_extra_proceed);
		idx_extra_proceed += 8;
	}

# if REPORT
	fprintf(fptr_report, "F6. recover first row, proceed %d / %d bits, show only first few.\n\t", idx_extra_proceed, idx_ex);
	for (int col = 0; col < width_image; ++col)
		fprintf(fptr_report, "%02X %s", byte_image[col], col % 64 == 63 ? "\n\t" : "");
	fprintf(fptr_report, "\n");
# endif

	for (int row = 1; row < width_image; ++row)
	{
		int idx = row * width_image;
		byte_image[idx] = bits2byte(bit_extract, idx_extra_proceed);
		idx_extra_proceed += 8;
	}

# if REPORT
	fprintf(fptr_report, "F7. recover first col, except(0, 0), proceed %d / %d bits, show only first few.\n\t", idx_extra_proceed, idx_ex);
	fprintf(fptr_report, "   "); 
	for (int row = 1; row < width_image; ++row)
	{
		int idx = row * width_image;
		fprintf(fptr_report, "%02X %s", byte_image[idx], row % 64 == 63 ? "\n\t" : "");
	}
	fprintf(fptr_report, "\n");
# endif

	vector<unsigned char> bit_secret_extra(len_secret);

	for (size_t i = 0; i < bit_secret_extra.size(); ++i)
	{
		bit_secret_extra[i] = bit_extract[idx_extra_proceed++];
		assert(bit_secret_extra[i] == (bits_rand_secret[i] & 0x01));
	}

# if REPORT
	fprintf(fptr_report, "F8. recover secret %d / %d bits, show only first few.\n\t", idx_extra_proceed, idx_ex);
	for (size_t i = 0; i < 512; ++i)
		fprintf(fptr_report, "%2d %s", bit_secret_extra[i], i % 64 == 63 ? "\n\t" : "");

	fprintf(fptr_report, "\n");
# endif

	for (int i = 0; i < size_image; ++i)
		byte_image[i] ^= byte_rand_keystream[i];

	


	for (int i = 1; i < width_image; ++i)
	{
		int row = i * width_image;
		for (int col = 1; col < width_image; ++col)
		{
			int idx = row + col;
			int t = val_labelmap_ex[idx];

			assert(idx < size_image);

			unsigned char x1 = byte_image[idx - width_image - 1];
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

			if (t < 8)
				px |= mask[t];

			byte_image[idx] = px;
		}
	}

# if PRINT
	printByteToFile(byte_image, width_image, FN_RESTORE_IMAGE_HEXDUMP);
# endif












# if REPORT
	fclose(fptr_report);
# endif

	return 0;
}

