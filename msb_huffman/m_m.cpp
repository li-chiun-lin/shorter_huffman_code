#include "../lib/lib.h"
#include "m_hm.h"
#include "m_exp.h"

int main()
{
	msb_huffman(PATH_COVER_IMG, PATH_REPORT, PATH_ENCRY_IMG, PATH_STEGO_IMG, PATH_RESTO_IMG, PATH_OG_IMG_HD, PATH_RE_IMG_HD);

	msb_huffman(PATH_COVER_IMG, PATH_REPORT);

	return 0;
}