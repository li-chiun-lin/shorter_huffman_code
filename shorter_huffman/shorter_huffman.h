#pragma once

using namespace std;

# define REPORT 1
# define PRINT 1

# define DIR_IMG "../image/"
# define DIR_IMG_OUT DIR_IMG "out/"
# define DIR_DATA_OUT "../out/"

# define FN_COVER_IMG "lena.tiff.raw"

# define EXT_ENC "_encrypt_sh.raw"
# define EXT_STE "_stego_sh.raw"
# define EXT_RES "_restore_sh.raw"
# define EXT_REP "_report_sh.txt"
# define EXT_HEX_OG "_hex_sh.txt"
# define EXT_HEX_RE "_restore_hex_sh.txt"

# define PATH_COVER_IMG DIR_IMG FN_COVER_IMG

# define PATH_ENCRY_IMG DIR_IMG_OUT FN_COVER_IMG EXT_ENC
# define PATH_STEGO_IMG DIR_IMG_OUT FN_COVER_IMG EXT_STE
# define PATH_RESTO_IMG DIR_IMG_OUT FN_COVER_IMG EXT_RES


# define PATH_REPORT DIR_DATA_OUT FN_COVER_IMG EXT_REP

# define PATH_OG_IMG_HD DIR_DATA_OUT FN_COVER_IMG EXT_HEX_OG
# define PATH_RE_IMG_HD DIR_DATA_OUT FN_COVER_IMG EXT_HEX_RE

# define DOMAIN_HCODE 9
# define RANGE_HCODE 7

# define LEN_T 4
# define LEN_H_TOPOLOGY ((2 * DOMAIN_HCODE) - 1)
# define LEN_H_VALUE (LEN_T * DOMAIN_HCODE)

# define LEN_LEN_H_ENCODED_LABELMAP 20

# define CNT_REF_TREE 7
# define CNT_REF_LEN 3

//# define CNT_REF_T 2
// this is for Baboon
# define CNT_REF_T 16

# define CNT_REF (CNT_REF_TREE + CNT_REF_LEN + CNT_REF_T)


void reportA_(vector<unsigned char>& val_labelmap, vector<int>& cnt_t, int width_image, int domain_hcode);
void reportC_(vector<int>& cnt_t, vector< vector<int> >& encoder,
	int len_capacity, int len_h_encoded_labelmap, int len_ref, int header1, int header2);
void reportD_(vector<unsigned char>& bits_embed, int len_h_topology, int len_h_value, int len_h_labmap, int idx_prepare);
void reportD2_(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed, int offset);
void reportD3_(vector<unsigned char>& byte_image, int idx_prepare, int len_embed, int cnt_ref);
void reportD4_(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed, int offset);
void reportD5_(vector<unsigned char>& byte_image, int idx_embed, int len_sideinfo);
void reportF_(vector<unsigned char>& byte_image, vector<unsigned char>& bit_extract, int idx_ex);
void reportF2_(Node* tree);
// similar to A
void reportF4_(vector<unsigned char>& val_ex_labelmap, int idx_ex, int idx_ex_proceed);
void reportF6_(vector<unsigned char>& byte_image, int idx_ex, int idx_ex_proceed);
void reportF7_(vector<unsigned char>& bit_secret_extra, int idx_ex, int idx_ex_proceed);

