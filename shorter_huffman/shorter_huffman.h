#pragma once

//# define FN_COVER_IMG "lena512.raw"
# define FN_COVER_IMG "lena.tiff.raw"

# define PATH_COVER_IMG DIR_IMG FN_COVER_IMG
# define PATH_ENCRY_IMG DIR_OUT FN_COVER_IMG"_encrypt_sh.raw"
# define PATH_STEGO_IMG DIR_OUT FN_COVER_IMG"_stego_sh.raw"
# define PATH_RESTO_IMG DIR_OUT FN_COVER_IMG"_restore_sh.raw"


# define PATH_REPORT DIR_OUT FN_COVER_IMG"_report_sh.txt"


# define PATH_OG_IMG_HD DIR_OUT FN_COVER_IMG"_hex.txt"
# define PATH_RE_IMG_HD DIR_OUT FN_COVER_IMG"_restore_hex_sh.txt"


# define LEN_HC_RULE 22
# define LEN_LEN_H_ENCODED_LABELMAP 20
# define LEN_HCODE_HEADER  (LEN_HC_RULE + LEN_LEN_H_ENCODED_LABELMAP)
