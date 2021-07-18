#include "../lib/lib.h"
#include "s_hm.h"
#include "s_exp.h"

int main()
{
	shr_huffman(PATH_COVER_IMG, PATH_REPORT, PATH_ENCRY_IMG, PATH_STEGO_IMG, PATH_RESTO_IMG, PATH_OG_IMG_HD, PATH_RE_IMG_HD);

	shr_huffman(PATH_COVER_IMG, PATH_REPORT);

	return 0;
}