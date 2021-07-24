
#include "libtiff/tiffio.h"

#include "../lib/lib.h"
#include "../msb_huffman/m_exp.h"
#include "../shorter_huffman/s_exp.h"


#include<filesystem>


using namespace std::filesystem;

void testlib()
{
	string path_cover_img = "../image/tiff/lena.tiff.raw";
	string path_report_msb = "../out/lena.tiff.raw_report_msb.txt";
	string path_report_sht = "../out/lena.tiff.raw_report_shr.txt";;

	msb_huffman(path_cover_img, path_report_msb);

	shr_huffman(path_cover_img, path_report_sht);
}

//void testtiff()
//{
//	vector<string> fn = { "Airplane", "Baboon", "Jetplane", "Man", "Lena", "Tiffany"};
//
//	for (string f : fn)
//	{
//		struct Result ret_msb_raw = msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb.txt");
//		struct Result ret_shr_raw = shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");
//
//		printResult(ret_msb_raw);
//		printResult(ret_shr_raw);
//	}
//}

void testtiff(bool with_baboon = false, bool with_man = false)
{
	FILE* fptr = NULL;

	// the output file name has been link to /article/, DO NOT change it.
	int ren = fopen_s(&fptr, "../out/delta-table.tex", "w");

	if (ren || !fptr)
		return;

	vector<string> fn = { "Airplane", "Jetplane", "Lena", "Tiffany" };

	if (with_baboon)
		fn.push_back("Baboon");

	sort(fn.begin(), fn.end());

	if (with_man)
		fn.push_back("Man");

	

	for (string f : fn)
	{
		cout << endl << endl << endl << endl;
		cout << f << endl;

		if (f == "Man")
			fprintf(fptr, "\\midrule \n");


		fprintf(fptr, "%-10s& ", f.c_str());

		struct Result ret_msb = msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb.txt");
		struct Result ret_shr = shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");

		printResult(ret_msb);
		printResult(ret_shr);

		

		fprintf(fptr, "%d &", ret_msb.width_image);

		fprintf(fptr, "%d   & %d   & $ %+d $\t& ", ret_shr.header_len, ret_msb.header_len, ret_shr.header_len - ret_msb.header_len);
		fprintf(fptr, "%d   & %d   & $ %+d $\t& ", ret_shr.len_ref / 8, ret_msb.len_ref / 8, (ret_shr.len_ref / 8) - (ret_msb.len_ref / 8));

		fprintf(fptr, "%d   & %d   & $ %+d $\t& ", ret_shr.size_capacity, ret_msb.size_capacity, ret_shr.size_capacity - ret_msb.size_capacity);
		fprintf(fptr, "%d   & %d   & $ %+d $\t& ", ret_shr.len_h_encoded_labelmap, ret_msb.len_h_encoded_labelmap, ret_shr.len_h_encoded_labelmap - ret_msb.len_h_encoded_labelmap);
		
		fprintf(fptr, "%.3f & %.3f & $ %+.3f $\t ", ret_shr.bbp, ret_msb.bbp, ret_shr.bbp - ret_msb.bbp);

		fprintf(fptr, "\t\\\\");

		fprintf(fptr, "\n");
	}

	fclose(fptr);
}

struct ResultDelta
{
	int sumf;
	int sumc;
	int sumd;

	ResultDelta() {
		sumf = 0;
		sumc = 0;
		sumd = 0;
	}
};

struct ResultDelta writeTableSingle(string tex_fn, struct Result res)
{
	struct ResultDelta ret;

	FILE* fptr = NULL;

	// the output file name has been link to /article/, DO NOT change it.
	int ren = fopen_s(&fptr, tex_fn.c_str(), "w");

	if (ren || !fptr)
		return ret;

	int sumf = 0;
	int sumc = 0;
	int suml = 0;
	int sumd = 0;
	

	for (int i = 0; i < 9; ++i)
	{
		int cap = i < 8 ? i + 1 : i;
		int dif = cap - res.len_code[i];

		sumf += res.freq[i];
		sumc += cap * res.freq[i];
		suml += res.len_code[i] * res.freq[i];
		sumd += dif * res.freq[i];

		fprintf(fptr, "%d & %d  & %s & %d & %d & $%+d$ & $%+d$ \\\\ \n", 
			i, res.freq[i], res.code_str[i], cap, res.len_code[i], dif, dif * res.freq[i]);
	}

	fprintf(fptr, "\\midrule \n");
	fprintf(fptr, "$\\sum$ & %d   && $%d$ & $%d$ &    $%+d$ & \\\\ \n", sumf, sumc, suml, sumd);
	//fprintf(fptr, "Total/Avg. & %d   && $%d$ & $%d$ &   \\multicolumn{2}{c}{ $%+d ~/ %+.3f$ }\\\\ \n", sumf, sumc, suml, sumd, (float)sumd / res.size_image);
	//fprintf(fptr, "Average &  &&& %.3f &   & %+.3f \\\\ \n", (float)sumc / sumf, (float)sumd / res.size_image);

	fclose(fptr);

	ret.sumf = sumf;
	ret.sumc = sumc;
	ret.sumd = sumd;

	return ret;
}

void lena()
{

	string f = "Lena";
	struct Result ret_msb = msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb.txt");
	struct Result ret_shr = shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");

	printResult(ret_msb);
	printResult(ret_shr);

	// the output file name has been link to /article/, DO NOT change it.

	struct ResultDelta d_shr = writeTableSingle("../out/single-res-table-shr-lena.tex", ret_shr);
	struct ResultDelta d_msb = writeTableSingle("../out/single-res-table-msb-lena.tex", ret_msb);
	

	FILE* fptr = NULL;

	int ren = fopen_s(&fptr, "../out/single-res-table-dif-lena.tex", "w");

	if (ren || !fptr)
		return;

	fprintf(fptr, "$\\Delta$ &  $%+d$ & &   $%+d$ & &   &  $%+d$  \\\\ \n", 
		d_shr.sumf - d_msb.sumf, d_shr.sumc - d_msb.sumc, d_shr.sumd - d_msb.sumd);

	fclose(fptr);
}

void airplane()
{

	string f = "Airplane";
	struct Result ret_msb = msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb.txt");
	struct Result ret_shr = shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");

	printResult(ret_msb);
	printResult(ret_shr);

	// the output file name has been link to /article/, DO NOT change it.

	struct ResultDelta d_shr = writeTableSingle("../out/single-res-table-shr-airplane.tex", ret_shr);
	struct ResultDelta d_msb = writeTableSingle("../out/single-res-table-msb-airplane.tex", ret_msb);


	FILE* fptr = NULL;

	int ren = fopen_s(&fptr, "../out/single-res-table-dif-airplane.tex", "w");

	if (ren || !fptr)
		return;

	fprintf(fptr, "$\\Delta$ &  %+d & &   %+d & &   &  %+d  \\\\ \n",
		d_shr.sumf - d_msb.sumf, d_shr.sumc - d_msb.sumc, d_shr.sumd - d_msb.sumd);

	fclose(fptr);
}

void debug()
{
	string f = "Baboon";
	shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");
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
}

void testres()
{
	string f = "Lena";
	struct Result ret_msb = msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb.txt");
	struct Result ret_shr = shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");

	printResult(ret_msb);
	printResult(ret_shr);
}

struct tiffInfo testreadtiff(string file, vector<unsigned char> &buf)
{
	struct tiffInfo info;

	TIFF* tif = TIFFOpen(file.c_str(), "r");
	if (tif == NULL)
		return info;

	int width, height;
	int spp = 0;
	tmsize_t sls = 0;
	tdata_t buf_tif = 0;

	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);

	info.sampleperpixel = spp;

	if (spp != 1)
		return info;

	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

	info.width = width;
	info.height = height;

	sls = TIFFScanlineSize(tif);
	buf_tif = _TIFFmalloc(sls);

	for (int row = 0; row < height; row++)
	{
		TIFFReadScanline(tif, buf_tif, row);
		unsigned char *data = (unsigned char*)buf_tif;
		
		buf.insert(buf.end(), data, data + width);
	}
	

	


	printf("%d %d %d %zu\n", width, height, spp, sls);


	_TIFFfree(buf_tif);
	TIFFClose(tif);

	return info;
}

void testlist()
{
	string path = "C:/Users/ttn/Desktop/xxx/USC-SIPI/misc";
	for (const auto& entry : directory_iterator(path))
	{
		cout << entry.path() << endl;

		vector<unsigned char> buf;
		struct tiffInfo ret = testreadtiff(entry.path().string(), buf);

		if (ret.sampleperpixel != 1)
			continue;

		
		

		break;
	}
}

void testreadtifflena()
{
	string file = "../image/tiff/Lena.tiff";

	vector<unsigned char> buf;
	struct tiffInfo ret = testreadtiff(file, buf);

	cout << buf.size() << endl;

	FILE* fptr = NULL;
	errno_t err = fopen_s(&fptr, "../out/testreadtiff.hd.txt", "w");

	if (err || fptr == NULL)
		return;

	cout << buf.size() << endl;

	for (size_t i = 0; i < buf.size(); ++i)
	{
		fprintf(fptr, "%02X%s", buf[i], i % ret.width == ret.width - 1 ? "\n" : " ");
	}

	fclose(fptr);
}

void testrawtiff()
{
	string f = "Lena";
	struct Result ret_msb_raw = msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb_raw.txt");
	printResult(ret_msb_raw);

	struct Result ret_msb_tiff = msb_huffman("../image/tiff/" + f + ".tiff", "../out/" + f + ".tiff_msb_tiff.txt", true);
	printResult(ret_msb_tiff);


	struct Result ret_shr_raw = shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr_raw.txt");
	printResult(ret_shr_raw);

	struct Result ret_shr_tiff = shr_huffman("../image/tiff/" + f + ".tiff", "../out/" + f + ".tiff_shr_tiff.txt", true);
	printResult(ret_shr_tiff);
}



int main()
{

	//testtiff(true, true);
	//lena();
	//airplane();

	//testlist();
	//testreadtifflena();

	testrawtiff();

	system("pause");
	
	return 0;
}