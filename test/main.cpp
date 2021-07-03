
#include "../lib/lib.h"
#include "../msb_huffman/export.h"
#include "../shorter_huffman/export.h"

int main()
{
	string path_cover_img = "../image/lena.tiff.raw";
	string path_report_msb = "../out/lena.tiff.raw_report_msb.txt";
	string path_report_sht = "../out/lena.tiff.raw_report_shr.txt";;
	
	msb_huffman(path_cover_img, path_report_msb);

	shorter_huffman(path_cover_img, path_report_sht);

	return 0;
}