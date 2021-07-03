#pragma once

//# define FN_COVER_IMG "lena512.raw"
# define FN_COVER_IMG "lena.tiff.raw"
//# define FN_COVER_IMG "baboon512.raw"

# define PATH_COVER_IMG DIR_IMG FN_COVER_IMG
# define PATH_ENCRY_IMG DIR_OUT FN_COVER_IMG"_encrypt_sh.raw"
# define PATH_STEGO_IMG DIR_OUT FN_COVER_IMG"_stego_sh.raw"
# define PATH_RESTO_IMG DIR_OUT FN_COVER_IMG"_restore_sh.raw"


# define PATH_REPORT DIR_OUT FN_COVER_IMG"_report_sh.txt"


# define PATH_OG_IMG_HD DIR_OUT FN_COVER_IMG"_hex.txt"
# define PATH_RE_IMG_HD DIR_OUT FN_COVER_IMG"_restore_hex_sh.txt"

# define DOMAIN_HCODE 9
# define RANGE_HCODE 7

# define LEN_T 4
# define LEN_H_TOPOLOGY ((2 * DOMAIN_HCODE) - 1)
# define LEN_H_VALUE (LEN_T * DOMAIN_HCODE)

# define CNT_REF_TREE 7
# define CNT_REF_LEN 3
# define CNT_REF_T 2
# define CNT_REF (CNT_REF_TREE + CNT_REF_LEN + CNT_REF_T)

# if 0
const unsigned char byte_hcode[DOMAIN_HCODE] = { 0x00, 0x01, 0x04, 0x05, 0x07, 0x0c, 0x0d };
const vector< vector<unsigned char> > bits_hcode = {
	{0, 0},
	{0, 1},
	{1, 0, 0},
	{1, 0, 1},
	{1, 1, 1},
	{1, 1, 0, 0},
	{1, 1, 0, 1},
};
# endif

# define LEN_HC_RULE 21
# define LEN_LEN_H_ENCODED_LABELMAP 20
# define LEN_HCODE_HEADER  (LEN_HC_RULE + LEN_LEN_H_ENCODED_LABELMAP)
