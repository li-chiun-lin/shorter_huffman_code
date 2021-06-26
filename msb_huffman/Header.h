#pragma once

# define REPORT 1
# define PRINT 1

# define DIR_IMG "../image/"
# define DIR_OUT "../out/"

# define FN_COVER_IMG "lena512.raw"

# define PATH_COVER_IMG DIR_IMG FN_COVER_IMG

# define PATH_ENCRY_IMG DIR_OUT FN_COVER_IMG"_encrypt.raw"
# define PATH_STEGO_IMG DIR_OUT FN_COVER_IMG"_stego.raw"
# define PATH_RESTO_IMG DIR_OUT FN_COVER_IMG"_restore.raw"

# if REPORT
#	define PATH_REPORT DIR_OUT FN_COVER_IMG"_report.txt"
#	define LEN_SAMPLE 4096
#	define ROW_SAMPLE 8
# endif

# if PRINT
#	define PATH_OG_IMG_HD DIR_OUT FN_COVER_IMG"_hex.txt"
#	define PATH_RE_IMG_HD DIR_OUT FN_COVER_IMG"_restore_hex.txt"
# endif


# define MAX_T 8
# define LEN_HC_RULE 32
# define LEN_LEN_H_ENCODED_LABELMAP 20
# define LEN_HCODE_HEADER  (LEN_HC_RULE + LEN_LEN_H_ENCODED_LABELMAP)
# define MAX_HCODE_LEN 5






# if REPORT
extern FILE* fptr_report;

const int width_report = 64;
const int width_report_1 = width_report - 1;

int reportBegin(int width_image, int size_image);
void reportEnd();
void reportA(vector<unsigned char>& val_labelmap, vector<int>& cnt_t, int width_image);
void reportB(vector<unsigned char>& byte_image, vector<unsigned char>& byte_rand_keystream, int width_image);
void reportB3(vector<unsigned char>& byte_image, int width_image);
void reportC(vector<int>& h_encoder, int len_h_encoded_labelmap, int len_secret);
void reportD(vector<unsigned char>& bits_embed, int idx_prepare);
void reportD2(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed);
void reportD3(vector<unsigned char>& bits_embed, int idx_prepare, int len_embed);
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
void reportG(vector<unsigned char>& byte_image, vector<unsigned char>& byte_rand_keystream);
void reportG3(vector<unsigned char>& byte_image);
void reportG4(vector<unsigned char>& byte_image);
# endif
