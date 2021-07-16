
#include "../lib/lib.h"
#include "../msb_huffman/export.h"
#include "../shorter_huffman/export.h"

void testlib()
{
	string path_cover_img = "../image/tiff/lena.tiff.raw";
	string path_report_msb = "../out/lena.tiff.raw_report_msb.txt";
	string path_report_sht = "../out/lena.tiff.raw_report_shr.txt";;

	msb_huffman(path_cover_img, path_report_msb);

	shr_huffman(path_cover_img, path_report_sht);
}

void testtiff()
{
	vector<string> fn = { "Airplane", "Baboon", "Jetplane", "Man", "Lena", "Tiffany"};

	for (string f : fn)
	{
		struct Result ret_msb = msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb.txt");
		struct Result ret_shr = shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");

		printResult(ret_msb);
		printResult(ret_shr);
	}
}

void testtiff_baboon()
{
	vector<string> fn = { "Airplane", "Jetplane", "Man", "Lena", "Tiffany" };

	for (string f : fn)
	{
		struct Result ret_msb = msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb.txt");
		struct Result ret_shr = shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");

		printResult(ret_msb);
		printResult(ret_shr);
	}
}

void debug()
{
	string f = "Baboon";
	shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");
}

void lena()
{
	string f = "Lena";
	struct Result ret_msb = msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb.txt");
	struct Result ret_shr = shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");

	printResult(ret_msb);
	printResult(ret_shr);
}

void man()
{
	string f = "Man";
	msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb.txt");
	//shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");
}

void log_2()
{
	for (int i = 0; i <= 256; ++i)
		cout << i << " " << floor(log2(i)) << ", ";
}

void testlen()
{
	int len = 1023 * 1023 * 5;
	cout << log2(len) << endl;

	len = 511 * 511 * 5;
	cout << log2(len) << endl;

	system("pause");
}

void testres()
{
	string f = "Lena";
	struct Result ret_msb = msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb.txt");
	struct Result ret_shr = shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");

	printResult(ret_msb);
	printResult(ret_shr);

	system("pause");
}

int main()
{
	//testlib();
	//testtiff();
	testtiff_baboon();

	//debug();
	lena();
	//testres();

	
	//testlen();

	//man();

	

	system("pause");
	
	return 0;
}