
#include "libtiff/tiffio.h"

#include "../lib/lib.h"
#include "../msb_huffman/m_exp.h"
#include "../shorter_huffman/s_exp.h"
#include "../shorter_huffman/s_hm.h"

#include<filesystem>

using namespace std::filesystem;

string dummy = "../out/null.txt";

//void testlib()
//{
//	string path_cover_img = "../image/tiff/lena.tiff.raw";
//	string path_report_msb = "../out/lena.tiff.raw_report_msb.txt";
//	string path_report_sht = "../out/lena.tiff.raw_report_shr.txt";;
//
//	msb_huffman(path_cover_img, path_report_msb);
//
//	shr_huffman(path_cover_img, path_report_sht);
//}

void genCmpTex(bool with_baboon = false, bool with_man = false, bool with_bpp = false, bool with_avg_code_length = false)
{
	FILE* fptr = NULL;

	// the output file name has been link to /article/, DO NOT change it.
	int ren = fopen_s(&fptr, "../article/delta-table.tex", "w");

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

		struct Result delta = ret_shr - ret_msb;

		fprintf(fptr, "%d &", ret_msb.width_image);

		fprintf(fptr, "%d   & %d   & $ %+d $\t& ", ret_shr.header_len, ret_msb.header_len, delta.header_len);
		fprintf(fptr, "%d   & %d   & $ %+d $\t& ", ret_shr.len_ref_byte, ret_msb.len_ref_byte, delta.len_ref_byte);

		fprintf(fptr, "%d   & %d   & $ %+d $\t& ", ret_shr.size_capacity_pure, ret_msb.size_capacity_pure, delta.size_capacity_pure);
		fprintf(fptr, "%d   & %d   & $ %+d $\t ", ret_shr.len_h_encoded_labelmap, ret_msb.len_h_encoded_labelmap, delta.len_h_encoded_labelmap);

		if (with_bpp)
			fprintf(fptr, "& %.3f & %.3f & $ %+.3f $\t ", ret_shr.bbp, ret_msb.bbp, delta.bbp);

		if (with_avg_code_length)
			fprintf(fptr, "& %.3f & %.3f & $ %+.3f $\t ", ret_shr.avg_code_len, ret_msb.avg_code_len, delta.avg_code_len);

		fprintf(fptr, "\t\\\\");

		fprintf(fptr, "\n");
	}

	fclose(fptr);
}

void genCmpTexSample(bool with_bpp = false, bool with_avg_code_length = false)
{
	FILE* fptr = NULL;

	// the output file name has been link to /article/, DO NOT change it.
	int ren = fopen_s(&fptr, "../article/delta-table-2.tex", "w");

	if (ren || !fptr)
		return;

	vector<string> fn = {
		"Airplane", "Baboon", "Baboon-Yin", "Bridge", "Couple", "Jetplane", "Lake",
		"Lena", "Lena-Yin", "Milkdrop", "Peppers", "Tiffany", "Lax" , "Man" };

	for (string f : fn)
	{
		cout << endl << endl << endl << endl;
		cout << f << endl;

		if (f == "Lax")
			fprintf(fptr, "\\midrule \n");


		fprintf(fptr, "%-10s& ", f.c_str());

		struct Result ret_msb = msb_huffman("../dataset/SAMPLE/" + f + ".tiff", dummy, true);
		struct Result ret_shr = shr_huffman("../dataset/SAMPLE/" + f + ".tiff", dummy, true);

		printResult(ret_msb);
		printResult(ret_shr);

		struct Result delta = ret_shr - ret_msb;

		fprintf(fptr, "%d &", ret_msb.width_image);

		fprintf(fptr, "%d   & %d   & $ %+d $\t& ", ret_shr.header_len, ret_msb.header_len, delta.header_len);
		fprintf(fptr, "%d   & %d   & $ %+d $\t& ", ret_shr.len_ref_byte, ret_msb.len_ref_byte, delta.len_ref_byte);

		fprintf(fptr, "%d   & %d   & $ %+d $\t& ", ret_shr.size_capacity_pure, ret_msb.size_capacity_pure, delta.size_capacity_pure);
		fprintf(fptr, "%d   & %d   & $ %+d $\t ", ret_shr.len_h_encoded_labelmap, ret_msb.len_h_encoded_labelmap, delta.len_h_encoded_labelmap);

		if (with_bpp)
			fprintf(fptr, "& %.3f & %.3f & $ %+.3f $\t ", ret_shr.bbp, ret_msb.bbp, delta.bbp);

		if (with_avg_code_length)
			fprintf(fptr, "& %.3f & %.3f & $ %+.3f $\t ", ret_shr.avg_code_len, ret_msb.avg_code_len, delta.avg_code_len);

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

void genLenaTex()
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

void genSingleTex(string f)
{
	//string f = "Man";
	struct Result ret_msb = msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb.txt");
	struct Result ret_shr = shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");

	printResult(ret_msb);
	printResult(ret_shr);

	// the output file name has been link to /article/, DO NOT change it.

	struct ResultDelta d_shr = writeTableSingle("../article/single-res-table-shr-" + f + ".tex", ret_shr);
	struct ResultDelta d_msb = writeTableSingle("../article/single-res-table-msb-" + f + ".tex", ret_msb);


	FILE* fptr = NULL;

	int ren = fopen_s(&fptr, string("../article/single-res-table-dif-" + f + ".tex").c_str(), "w");

	if (ren || !fptr)
		return;

	fprintf(fptr, "$\\Delta$ &  $%+d$ & &   $%+d$ & &   &  $%+d$  \\\\ \n",
		d_shr.sumf - d_msb.sumf, d_shr.sumc - d_msb.sumc, d_shr.sumd - d_msb.sumd);

	fclose(fptr);
}

void genAirplaneTex()
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

//void debug()
//{
//	string f = "Baboon";
//	shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");
//}

//void man()
//{
//	string f = "Man";
//	msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb.txt");
//	//shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");
//}

//void log_2()
//{
//	for (int i = 0; i <= 256; ++i)
//		cout << i << " " << floor(log2(i)) << ", ";
//}

//void testlen()
//{
//	int len = 1023 * 1023 * 5;
//	cout << log2(len) << endl;
//
//	len = 511 * 511 * 5;
//	cout << log2(len) << endl;
//}

//void testres()
//{
//	string f = "Lena";
//	struct Result ret_msb = msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb.txt");
//	struct Result ret_shr = shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr.txt");
//
//	printResult(ret_msb);
//	printResult(ret_shr);
//}

//struct tiffInfo testreadtiff(string file, vector<unsigned char> &buf)
//{
//	struct tiffInfo info;
//
//	TIFF* tif = TIFFOpen(file.c_str(), "r");
//	if (tif == NULL)
//		return info;
//
//	int width, height;
//	int spp = 0;
//	tmsize_t sls = 0;
//	tdata_t buf_tif = 0;
//
//	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
//
//	info.sampleperpixel = spp;
//
//	if (spp != 1)
//		return info;
//
//	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &width);
//	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
//
//	info.width = width;
//	info.height = height;
//
//	sls = TIFFScanlineSize(tif);
//	buf_tif = _TIFFmalloc(sls);
//
//	for (int row = 0; row < height; row++)
//	{
//		TIFFReadScanline(tif, buf_tif, row);
//		unsigned char *data = (unsigned char*)buf_tif;
//		
//		buf.insert(buf.end(), data, data + width);
//	}
//
//	printf("%d %d %d %zu\n", width, height, spp, sls);
//
//
//	_TIFFfree(buf_tif);
//	TIFFClose(tif);
//
//	return info;
//}

//void testlist()
//{
//	string path = "C:/Users/ttn/Desktop/xxx/USC-SIPI/misc";
//	for (const auto& entry : directory_iterator(path))
//	{
//		cout << entry.path() << endl;
//
//		vector<unsigned char> buf;
//		struct tiffInfo ret = testreadtiff(entry.path().string(), buf);
//
//		if (ret.sampleperpixel != 1)
//			continue;
//
//		break;
//	}
//}

//void testreadtifflena()
//{
//	string file = "../image/tiff/Lena.tiff";
//
//	vector<unsigned char> buf;
//	struct tiffInfo ret = testreadtiff(file, buf);
//
//	cout << buf.size() << endl;
//
//	FILE* fptr = NULL;
//	errno_t err = fopen_s(&fptr, "../out/testreadtiff.hd.txt", "w");
//
//	if (err || fptr == NULL)
//		return;
//
//	cout << buf.size() << endl;
//
//	for (size_t i = 0; i < buf.size(); ++i)
//	{
//		fprintf(fptr, "%02X%s", buf[i], i % ret.width == ret.width - 1 ? "\n" : " ");
//	}
//
//	fclose(fptr);
//}

//void testrawtiff()
//{
//	string f = "Lena";
//	struct Result ret_msb_raw = msb_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_msb_raw.txt");
//	printResult(ret_msb_raw);
//
//	struct Result ret_msb_tiff = msb_huffman("../image/tiff/" + f + ".tiff", "../out/" + f + ".tiff_msb_tiff.txt", true);
//	printResult(ret_msb_tiff);
//
//	struct Result ret_shr_raw = shr_huffman("../image/tiff/" + f + ".raw", "../out/" + f + ".tiff_shr_raw.txt");
//	printResult(ret_shr_raw);
//
//	struct Result ret_shr_tiff = shr_huffman("../image/tiff/" + f + ".tiff", "../out/" + f + ".tiff_shr_tiff.txt", true);
//	printResult(ret_shr_tiff);
//}


//string bigreport = "../out/bigreport-usc.txt";
FILE* fptr = NULL;

bool proceed2Res(path file)
{
	string report = "../out/tmp/" + file.string();

	//struct Result ret_msb_tiff = msb_huffman(file.string(), dummy, true);
	//struct Result ret_shr_tiff = shr_huffman(file.string(), dummy, true);
	struct Result ret_msb_tiff = msb_huffman(file.string(), report + ".msb.txt", true);
	struct Result ret_shr_tiff = shr_huffman(file.string(), report + ".shr.txt", true);

	if (ret_msb_tiff.isValid && ret_shr_tiff.isValid)
	{
		struct Result delta = ret_shr_tiff - ret_msb_tiff;

		fprintf(fptr, "%s, %s, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %.3f, %.3f, %.3f\n",
			file.string().c_str(), file.filename().string().c_str(),
			ret_msb_tiff.width_image, ret_msb_tiff.height_image,
			ret_shr_tiff.header_len, ret_msb_tiff.header_len,
			delta.header_len,
			ret_shr_tiff.len_ref, ret_msb_tiff.len_ref,
			delta.len_ref,
			ret_shr_tiff.size_capacity_pure, ret_msb_tiff.size_capacity_pure,
			delta.size_capacity_pure,
			ret_shr_tiff.len_h_encoded_labelmap, ret_msb_tiff.len_h_encoded_labelmap,
			delta.len_h_encoded_labelmap,
			ret_shr_tiff.bbp, ret_msb_tiff.bbp,
			delta.bbp);

		//printResult(ret_msb_tiff);
		//printResult(ret_shr_tiff);

		return true;
	}
	else
	{
		if (!ret_msb_tiff.isValid)
			cout << "error msb " << endl;
		if (!ret_shr_tiff.isValid)
			cout << "error shr " << endl;

		return false;
	}
}

void listdir_(path p)
{
	if (!filesystem::exists(p))
	{
		cout << p << " not exists" << endl;
		return;
	}

	for (const auto& entry : directory_iterator(p))
	{
		if (entry.is_directory())
		{
			cout << "d: " << entry.path() << endl;
			listdir_(entry.path());
		}
		else
		{
			cout << "f: " << entry.path() << endl;
			/*if (proceed2Res(entry.path()))
				return;*/

			proceed2Res(entry.path());
		}
	}
}

void listdir(path p, bool(*ptr)(path file))
{
	if (!filesystem::exists(p))
	{
		cout << p << " not exists" << endl;
		return;
	}

	if (is_directory(p))
	{
		cout << "d: " << p << endl;
		for (const auto& entry : directory_iterator(p))
			listdir(entry.path(), ptr);
	}
	else
	{
		cout << "f: " << p << endl;
		//cout << ".";
		(*ptr)(p);
	}
}

void bigdata(string dir, string report)
{
	errno_t err = fopen_s(&fptr, report.c_str(), "w");

	if (err || !fptr)
		return;

	fprintf(fptr, "%s, %s, %s, %s, %s, %s\n",
		"path, file_name, width, height",
		"header1, header2, delta",
		"ref1, ref2, delta",
		"capacity1, capacity2, delta",
		"lm1, lm2, delta",
		"bbp1, bbp2, delta");

	listdir(dir, &proceed2Res);

	fclose(fptr);
}

void genBigData()
{
	bigdata("../dataset/USC-SIPI/", "../out/bigreport-usc.csv");
	//bigdata("../dataset/BOSSbase/", "../out/bigreport-boss.csv");
	//bigdata("../dataset/BOWS2/", "../out/bigreport-bows2.csv");
	//bigdata("../dataset/UCID/", "../out/bigreport-ucid.csv");
}

bool tryRef(path file)
{
	//struct Result ret_msb_tiff = msb_huffman(file.string(), dummy, true);
	struct Result ret_shr_tiff = shr_huffman(file.string(), dummy, true);

	if (ret_shr_tiff.loopInvariantViolation)
	{
		cout << "xx " << file << endl;

		fprintf(fptr, "%s\n", file.string().c_str());
	}
	else
	{

	}

	return true;
}

void testRef()
{
	//string dir = "../dataset/USC-SIPI/misc/";
	string dir = "../dataset/";
	string report = "../out/tryRef.txt";


	errno_t err = fopen_s(&fptr, report.c_str(), "w");

	if (err || !fptr)
		return;

	listdir(dir, &tryRef);

	fclose(fptr);
}

void getDataSetListRec(path p, vector<path>& ret)
{
	if (!filesystem::exists(p))
	{
		cout << p << " not exists" << endl;
		return;
	}

	if (is_directory(p))
	{
		//cout << "d: " << p << endl;
		for (const auto& entry : directory_iterator(p))
			getDataSetListRec(entry.path(), ret);
	}
	else
	{
		//cout << "f: " << p << endl;
		ret.push_back(p);
	}
}

vector<path> getDataSetList()
{
	vector<path> ret;
	getDataSetListRec("../dataset/", ret);

	cout << ret.size() << endl;

	return ret;
}

//void testDataSetRefLimit()
//{
//	vector<path> lst = getDataSetList();
//
//	for (size_t i = 0; i < lst.size(); ++i)
//	{
//		string report = "../out/tmp/" + lst[i].string();
//
//		//cout << i << " " << lst[i] << endl;
//		struct Result ret_shr_tiff = shr_huffman(lst[i].string(), report + ".shr.txt", true);
//		struct Result ret_msb_tiff = msb_huffman(lst[i].string(), report + ".msb.txt", true);
//
//		//cout << lst[i] << endl;
//		cout << i << ", ";
//
//
//		if (ret_shr_tiff.loopInvariantViolation)
//		{
//			cout << i << " shr " << lst[i] << endl;
//		}
//
//		if (ret_msb_tiff.loopInvariantViolation)
//		{
//			cout << i << " msb " << lst[i] << endl;
//		}
//	}
//}

//void testsingle()
//{
//	//string f = "../dataset/BOSSbase_101_tif/8223.tif";
//	//string f = "../dataset/BOWS2OrigEp3_tif/3386.tif";
//	//string f = "../dataset/BOSSbase/339.tif";
//	//string f = "../dataset/BOSSbase/8880.tif";
//	//string f = "../dataset/UCID/ucid00067.tif";
//	//string f = "../dataset/UCID/ucid00770.tif";
//	string f = "../dataset/UCID/ucid00851.tif";
//
//	string report = "../out/tmp/" + f;
//
//	cout << report + ".msb.txt" << endl;
//
//
//	struct Result ret_msb = msb_huffman(f, report + ".msb.txt", true);
//	printResult(ret_msb);
//
//	cout << ret_msb.isValid << endl;
//
//	struct Result ret_shr = shr_huffman(f, report + ".shr.txt", true);
//	printResult(ret_shr);
//
//	cout << ret_shr.isValid << endl;
//}


void genDataTex()
{
	genCmpTex(true, true);
	genCmpTexSample();

	genSingleTex("Lena");
	genSingleTex("Airplane");
	genSingleTex("Man");
}

void printTree(Node* tree, int height = 0)
{
	if (tree == NULL)
		return;

	if (tree->next[0] == NULL && tree->next[1] == NULL)
	{
		printf("0 (%d) ", tree->val);
	}
	else
	{
		printf("1 ");
		printTree(tree->next[0], height + 1);
		printTree(tree->next[1], height + 1);
	}
}

void htBoundary_shr(vector<int>& cnt_t)
{
	Node* tree = buildHuffmanTree(cnt_t);
	vector< vector<unsigned char> > h_encoder = buildHuffmanEncoder(tree, (int)cnt_t.size());

	int cnt = accumulate(cnt_t.begin(), cnt_t.end(), 0);

	size_t len = 0;
	for (size_t i = 0; i < cnt_t.size(); ++i)
	{
		len += cnt_t[i] * h_encoder[i].size();
	}

	printTree(tree);
	printf("\n");
	printHuffmanEncoder(h_encoder);
	printf("total count = %d\n", cnt);
	printf("encoded len = %Iu\n", len);
	printf("average     = %f\n", (float)len / cnt);
}

void printCode(int code)
{
	while (code)
	{
		cout << (code % 2);
		code >>= 1;
	}
	cout << " ";
}

void htBoundary_msb(vector<int>& cnt_t)
{
	vector<int> h_encoder(9);
	size_t len = buildHuffmanEncoder(h_encoder, cnt_t);

	int cnt = accumulate(cnt_t.begin(), cnt_t.end(), 0);

	for (size_t i = 0; i < cnt_t.size(); ++i)
	{
		int code = h_encoder[i];
		printCode(code);
	}
	cout << endl;

	printf("total count = %d\n", cnt);
	printf("encoded len = %Iu\n", len);
	printf("average     = %f\n", (float)len / cnt);
}

void printvec(vector<int>& vec)
{
	for (size_t i = 0; i < vec.size(); ++i)
		cout << vec[i] << ", ";
	cout << endl;
}

//void testhtBoundary_shr()
//{
//	vector<int> cnt_t1 = { 1, 1, 2, 4, 8, 16, 32, 64, 128 };
//	cout << "fibonacci" << endl;
//	printvec(cnt_t1);
//	htBoundary_shr(cnt_t1);
//
//
//	vector<int> cnt_t2 = { 1, 1, 1, 1, 1, 1, 1, 1, 128 };
//	cout << endl << "very unbalanced" << endl;
//	printvec(cnt_t2);
//	htBoundary_shr(cnt_t2);
//
//	vector<int> cnt_t3 = { 0, 1, 1, 2, 4, 8, 16, 32, 64 };
//	cout << endl << "good fibonacci" << endl;
//	printvec(cnt_t3);
//	htBoundary_shr(cnt_t3);
//
//	// airplane
//	vector<int> cnt_t4 = { 1793, 13899 , 4896 , 24131 , 27749 , 47275 , 36632  , 1827 , 103916 };
//	cout << endl << "airplane" << endl;
//	printvec(cnt_t4);
//	htBoundary_shr(cnt_t4);
//
//	vector<int> cnt_t6 = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
//	cout << endl << "sequencial" << endl;
//	printvec(cnt_t6);
//	htBoundary_shr(cnt_t6);
//
//	vector<int> cnt_t7 = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
//	cout << endl << "balanced" << endl;
//	printvec(cnt_t7);
//	htBoundary_shr(cnt_t7);
//
//	vector<int> cnt_t8 = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
//	int cnt8 = 9;
//	for (size_t i = 0; i < cnt_t8.size(); ++i)
//		cnt_t8[i] = cnt_t8[i] * (511 * 511) / cnt8;
//	cout << endl << "balanced many" << endl;
//	printvec(cnt_t8);
//	htBoundary_shr(cnt_t8);
//}

//void testhtBoundary_msb()
//{
//	vector<int> cnt_t1 = { 1, 1, 2, 4, 8, 16, 32, 64, 128 };
//	cout << "fibonacci" << endl;
//	printvec(cnt_t1);
//	htBoundary_msb(cnt_t1);
//
//
//	vector<int> cnt_t2 = { 1, 1, 1, 1, 1, 1, 1, 1, 128 };
//	cout << endl << "very unbalanced" << endl;
//	printvec(cnt_t2);
//	htBoundary_msb(cnt_t2);
//
//	vector<int> cnt_t3 = { 0, 1, 1, 2, 4, 8, 16, 32, 64 };
//	cout << endl << "good fibonacci" << endl;
//	printvec(cnt_t3);
//	htBoundary_msb(cnt_t3);
//
//	// airplane
//	vector<int> cnt_t4 = { 1793, 13899 , 4896 , 24131 , 27749 , 47275 , 36632  , 1827 , 103916 };
//	cout << endl << "airplane" << endl;
//	printvec(cnt_t4);
//	htBoundary_msb(cnt_t4);
//
//	vector<int> cnt_t6 = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
//	cout << endl << "sequencial" << endl;
//	printvec(cnt_t6);
//	htBoundary_msb(cnt_t6);
//
//	vector<int> cnt_t7 = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
//	cout << endl << "balanced" << endl;
//	printvec(cnt_t7);
//	htBoundary_msb(cnt_t7);
//
//	vector<int> cnt_t8 = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
//	int cnt8 = 9;
//	for (size_t i = 0; i < cnt_t8.size(); ++i)
//		cnt_t8[i] = cnt_t8[i] * (511 * 511) / cnt8;
//	cout << endl << "balanced many" << endl;
//	printvec(cnt_t8);
//	htBoundary_msb(cnt_t8);
//}
//
//void printlog2()
//{
//	cout << "Airplane " << log2(682803) << endl;
//	cout << "Baboon " << log2(794941) << endl;
//	cout << "Jetplane " << log2(793441) << endl;
//	cout << "Lena " << log2(793304) << endl;
//	cout << "Tiffany " << log2(786527) << endl;
//	cout << "Man " << log2(3121506) << endl;
//
//	cout << "511 " << log2(511 * 511) << endl;
//	cout << "511 3 " << log2(511 * 511 * 3.555) << endl;
//
//
//	cout << "for man" << endl;
//	cout << "1023 3.5 " << log2(1023 * 1023 * 3.555) << endl;
//}


//void testLenaLena()
//{
//	string f = "Lena";
//	struct Result ret_raw = msb_huffman("../image/tiff/" + f + ".raw", dummy);
//	struct Result ret_tif = msb_huffman("../image/tiff/" + f + ".tiff", dummy, true);
//
//	struct Result ret_mis = msb_huffman("../dataset/MISC/" + f + ".tiff", dummy, true);
//	struct Result ret_sam = msb_huffman("../dataset/SAMPLE/" + f + ".tiff", dummy, true);
//
//	printResult(ret_raw);
//	printResult(ret_tif);
//	printResult(ret_mis);
//	printResult(ret_sam);
//}


void test_med()
{
	vector<vector<int>> p = {
		{100, 110, 112, 110, 101},
		{120, 121, 124, 120, 119},
		{130, 132, 140, 150, 142},
		{140, 131, 143, 145, 145},
		{120, 121, 124, 145, 145},
	};

	auto pv = med(p);
	auto pe = pred_err(p, pv, 3);
	auto t = emb_cap(pe, 3);

	print(p);
	print(pv);
	print(pe);
	print(t);
}

void gen_statistic_data(string fn)
{
	//"../image/tiff/" + f + ".raw"
	vector<unsigned char> buf;

	auto info = readImageTiff(fn, buf);

	vector<int> freq(256);

	for (auto x : buf)
		++freq[x];

	cout << fn << "\n";

	cout << "pixel,";
	for (int i = 0; i < 256; ++i)
		cout << i << ",";
	cout << "\n";

	cout << "count,";
	for (int i = 0; i < 256; ++i)
		cout << freq[i] << ",";
	cout << "\n";
}

void gen_statistic_data_random(int size)
{
	vector<int> freq(256);

	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
		++freq[rand() % 256];

	cout << "random" << "\n";

	cout << "pixel,";
	for (int i = 0; i < 256; ++i)
		cout << i << ",";
	cout << "\n";

	cout << "count,";
	for (int i = 0; i < 256; ++i)
		cout << freq[i] << ",";
	cout << "\n";
}

int test_shr_huffman()
{
	shr_huffman("../image/tiff/lena.tiff.raw", 
		"../out/lena.tiff.raw_report_sh.txt", 
		false, 
		"../image/tiff/out/lena.tiff.raw_encrypt_sh.raw", 
		"../image/tiff/out/lena.tiff.raw_stego_sh.raw", 
		"../image/tiff/out/lena.tiff.raw_restore_sh.raw", 
		"../out/lena.tiff.raw_hex_sh.txt", 
		"../out/lena.tiff.raw_restore_hex_sh.txt");

	shr_huffman("../image/tiff/Jetplane.tiff.raw",
		"../out/Jetplane.tiff.raw_report_sh.txt",
		false,
		"../image/tiff/out/Jetplane.tiff.raw_encrypt_sh.raw",
		"../image/tiff/out/Jetplane.tiff.raw_stego_sh.raw",
		"../image/tiff/out/Jetplane.tiff.raw_restore_sh.raw",
		"../out/Jetplane.tiff.raw_hex_sh.txt",
		"../out/Jetplane.tiff.raw_restore_hex_sh.txt");

	shr_huffman("../image/tiff/Baboon.tiff.raw",
		"../out/Baboon.tiff.raw_report_sh.txt",
		false,
		"../image/tiff/out/Baboon.tiff.raw_encrypt_sh.raw",
		"../image/tiff/out/Baboon.tiff.raw_stego_sh.raw",
		"../image/tiff/out/Baboon.tiff.raw_restore_sh.raw",
		"../out/Baboon.tiff.raw_hex_sh.txt",
		"../out/Baboon.tiff.raw_restore_hex_sh.txt");
	//shr_huffman(PATH_COVER_IMG, PATH_REPORT);

	return 0;
}

int main()
{
	//genDataTex();
	//genBigData();
	//bigdata("../dataset/USC-SIPI/", "../out/bigreport-usc.csv");
	//bigdata("../dataset/UCID/", "../out/bigreport-ucid.csv");
	//bigdata("../dataset/TAMPERE17/", "../out/bigreport-tampere17.csv");

	//genCmpTexSample();

	//genManTex();
	//genLenaTex();
	//genAirplaneTex();

	//testLenaLena();


	//testsingle();
	//testDataSetRefLimit();

	//testhtBoundary_shr();
	//testhtBoundary_msb();

	//printlog2();

	test_med();
	//gen_statistic_data("../image/tiff/Jetplane.tiff");
	//gen_statistic_data("../image/tiff/Lena.tiff");
#if 0
	gen_statistic_data_random(512);
	gen_statistic_data_random(512);
	gen_statistic_data_random(512);
	gen_statistic_data_random(512);
	gen_statistic_data_random(512);
	gen_statistic_data_random(512);
#endif

	//test_shr_huffman();


	return 0;
}