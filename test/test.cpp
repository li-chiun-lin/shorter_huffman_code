
#include "../lib/lib.h"
#include "../msb_huffman/m_exp.h"
#include "../shorter_huffman/s_exp.h"

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
//		struct Result ret_msb = msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb.txt");
//		struct Result ret_shr = shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");
//
//		printResult(ret_msb);
//		printResult(ret_shr);
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
	int sumd = 0;

	for (int i = 0; i < 9; ++i)
	{
		int cap = i < 8 ? i + 1 : i;
		int dif = cap - res.len_code[i];

		sumf += res.freq[i];
		sumc += cap * res.freq[i];
		sumd += dif * res.freq[i];

		fprintf(fptr, "%d & %d  & %s & %d & %d & %+d & %+d \\\\ \n", 
			i, res.freq[i], res.code_str[i], cap, res.len_code[i], dif, dif * res.freq[i]);
	}

	fprintf(fptr, "\\midrule \n");
	fprintf(fptr, "Total/Avg. & %d   && %d / %.3f & &   & %+d / %+.3f \\\\ \n", sumf, sumc, (float)sumc / sumf, sumd, (float)sumd / res.size_image);
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

	fprintf(fptr, "$\\Delta$ &  %+d & &   %+d & &   &  %+d  \\\\ \n", 
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
	// 
	testtiff(true, true);

	//debug();
	lena();
	airplane();
	//testres();

	
	//testlen();

	//man();

	

	//system("pause");
	
	return 0;
}