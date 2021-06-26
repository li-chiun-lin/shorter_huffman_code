#pragma once



# define FN_COVER_IMG "lena512.raw"

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

