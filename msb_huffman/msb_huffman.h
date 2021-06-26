#pragma once



//# define FN_COVER_IMG "lena512.raw"
# define FN_COVER_IMG "lena.tiff.raw"
//# define FN_COVER_IMG "baboon512.raw"

# define PATH_COVER_IMG DIR_IMG FN_COVER_IMG
# define PATH_ENCRY_IMG DIR_OUT FN_COVER_IMG"_encrypt.raw"
# define PATH_STEGO_IMG DIR_OUT FN_COVER_IMG"_stego.raw"
# define PATH_RESTO_IMG DIR_OUT FN_COVER_IMG"_restore.raw"


# define PATH_REPORT DIR_OUT FN_COVER_IMG"_report.txt"


# define PATH_OG_IMG_HD DIR_OUT FN_COVER_IMG"_hex.txt"
# define PATH_RE_IMG_HD DIR_OUT FN_COVER_IMG"_restore_hex.txt"

# define LEN_HC_RULE 32
# define LEN_LEN_H_ENCODED_LABELMAP 20
# define LEN_HCODE_HEADER  (LEN_HC_RULE + LEN_LEN_H_ENCODED_LABELMAP)

# define DOMAIN_HCODE 9
# define RANGE_HCODE 9

const unsigned char byte_hcode[DOMAIN_HCODE] = { 0x00, 0x01, 0x04, 0x05, 0x0c, 0x0d, 0x0e, 0x1e, 0x1f };
const vector< vector<unsigned char> > bits_hcode = {
	{0, 0},
	{0, 1},
	{1, 0, 0},
	{1, 0, 1},
	{1, 1, 0, 0},
	{1, 1, 0, 1},
	{1, 1, 1, 0},
	{1, 1, 1, 1, 0},
	{1, 1, 1, 1, 1}
};