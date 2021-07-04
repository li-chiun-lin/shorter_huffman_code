#pragma once

# define REPORT 1
# define PRINT 1

# define DIR_IMG "../image/"
# define DIR_IMG_OUT DIR_IMG "out/"
# define DIR_DATA_OUT "../out/"

//# define FN_COVER_IMG "lena512.raw"
# define FN_COVER_IMG "lena.tiff.raw"
//# define FN_COVER_IMG "baboon512.raw"

# define EXT_ENC "_encrypt.raw"
# define EXT_STE "_stego.raw"
# define EXT_RES "_restore.raw"

# define EXT_REP "_report.txt"

# define EXT_HEX_OG "_hex.txt"
# define EXT_HEX_RE "_restore_hex.txt"

# define PATH_COVER_IMG DIR_IMG FN_COVER_IMG

# define PATH_ENCRY_IMG DIR_IMG_OUT FN_COVER_IMG EXT_ENC
# define PATH_STEGO_IMG DIR_IMG_OUT FN_COVER_IMG EXT_STE
# define PATH_RESTO_IMG DIR_IMG_OUT FN_COVER_IMG EXT_RES


# define PATH_REPORT DIR_DATA_OUT FN_COVER_IMG EXT_REP

# define PATH_OG_IMG_HD DIR_DATA_OUT FN_COVER_IMG EXT_HEX_OG
# define PATH_RE_IMG_HD DIR_DATA_OUT FN_COVER_IMG EXT_HEX_RE



# define LEN_HC_RULE 32
# define LEN_LEN_H_ENCODED_LABELMAP 20
# define LEN_HCODE_HEADER  (LEN_HC_RULE + LEN_LEN_H_ENCODED_LABELMAP)

# define DOMAIN_HCODE 9
# define RANGE_HCODE 9

# define MAX_HCODE_LEN 5

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

void reportA(vector<unsigned char>& val_labelmap, vector<int>& cnt_t, int width_image, int size_hcode);
void reportC(vector<int>& h_encoder, vector<int>& cnt_t, int len_h_encoded_labelmap, int len_secret, int len_hcode_header);
void reportD(vector<unsigned char>& bits_embed, int idx_prepare, int len_hc_rule);
void reportD2(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed, int len_len_h_encoded_labelmap, int len_hc_rule);
void reportD3(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed, int len_hcode_header);
void reportD4(vector<unsigned char>& byte_image, int idx_prepare, int len_embed, int width_image);
void reportD5(vector<unsigned char>& byte_image, int idx_prepare, int len_embed, int width_image);
void reportD6(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed, int len_auxinfo);
void reportD7(vector<unsigned char>& byte_image, int idx_embed, int width_image, int len_auxinfo);
void reportD8(vector<unsigned char>& byte_image, int idx_embed, int width_image, int len_auxinfo);
void reportF(vector<unsigned char>& byte_image, vector<unsigned char>& bit_extract, int idx_ex, int width_image);
void reportF2(vector<unsigned char>& byte_image, int idx_ex, int width_image);
void reportF5(vector<unsigned char>& val_ex_labelmap, int idx_ex, int idx_ex_proceed, int width_image);
void reportF6(vector<unsigned char>& byte_image, int idx_ex, int idx_ex_proceed, int width_image);
void reportF7(vector<unsigned char>& byte_image, int idx_ex, int idx_ex_proceed, int width_image);
void reportF8(vector<unsigned char>& bit_secret_extra, int idx_ex, int idx_ex_proceed);

