#include "pch.h"
#include "framework.h"

#include "lib.h"

FILE* fptr_report = NULL;

int reportBegin(int width_image, int size_image, const char* fn_report, const char* fn_cover_img)
{
	int ren = fopen_s(&fptr_report, fn_report, "w");

	if (ren || fptr_report == NULL)
		return -2;

	fprintf(fptr_report, "cover image: %s, %d x %d = %d bytes\n\n", fn_cover_img, width_image, width_image, size_image);
	return 0;
}

void reportEnd()
{
	fclose(fptr_report);
}

void reportB(vector<unsigned char>& byte_image, vector<unsigned char>& byte_rand_keystream)
{
	fprintf(fptr_report, "B. Image Encryption with random key stream\n\n");
	fprintf(fptr_report, "B1. original pixels, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_image[i], WRAP(i));
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "B2. random key stream, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_rand_keystream[i], WRAP(i));
	fprintf(fptr_report, "\n");
}

void reportB3(vector<unsigned char>& byte_image, const char* fn_enc_img)
{
	if (*fn_enc_img)
		fprintf(fptr_report, "B3. cipher bytes, only show first few, write to %s\n\t", fn_enc_img);
	else
		fprintf(fptr_report, "B3. cipher bytes, only show first few\n\t");

	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_image[i], WRAP(i));
	fprintf(fptr_report, "\n");
}

void reportG(vector<unsigned char>& byte_image, vector<unsigned char>& byte_rand_keystream)
{
	fprintf(fptr_report, "G. Image Recovery with key stream, only show first few\n\n");
	fprintf(fptr_report, "G1. cipher bytes, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_image[i], WRAP(i));
	fprintf(fptr_report, "\n");
	fprintf(fptr_report, "G2. key stream, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_rand_keystream[i], WRAP(i));
	fprintf(fptr_report, "\n");
}

void reportG3(vector<unsigned char>& byte_image)
{
	fprintf(fptr_report, "G3. decrypted bytes, only show first few\n\t");
	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_image[i], WRAP(i));
	fprintf(fptr_report, "\n");
}

void reportG4(vector<unsigned char>& byte_image, const char* fn_restore)
{
	if (*fn_restore)
		fprintf(fptr_report, "G4. revoered pixels, only show first few, write stego image to %s\n\t", fn_restore);
	else
		fprintf(fptr_report, "G4. revoered pixels, only show first few\n\t");

	for (int i = 0; i < LEN_SAMPLE; ++i)
		fprintf(fptr_report, "%02X %s", byte_image[i], WRAP(i));
	fprintf(fptr_report, "\n");
}

void printResult(Result res)
{
	printf("--------------------------------------\n");
	printf("size_image                 %8d\n", res.size_image);
	printf("width_image                %8d\n", res.width_image);
	printf("\n");
	printf("len_h_topology             %8d = 2 * %d - 1\n", res.header_len_h_topology, 
											res.header_len_h_topology ? (res.header_len_h_topology + 1) / 2 : 0);
	printf("len_len_h_values           %8d\n", res.header_len_h_values);
	printf("log2_len_hencoded_labelmap %8d\n", res.header_log2_len_h_encoded_labelmap);

	printf("\n");
	printf("len_h_encoded_labelmap     %8d\n", res.len_h_encoded_labelmap);
	printf("\n");
	printf("len_ref                    %8d = %d * 8\n", res.len_ref, res.len_ref / 8);
	printf("\n");
	printf("len_secret                 %8d = %.3f bpp\n", res.len_secret, res.size_image ? (float)res.len_secret / res.size_image : 0);
	printf("\n");
	printf("capacity                   %8d = %d + %d\n", res.size_capacity_with_header, res.len_h_encoded_labelmap, res.len_secret);
	printf("\n");
	printf("capacity without header    %8d = %d + %d + %d\n", res.size_capacity, res.len_h_encoded_labelmap, res.len_secret, res.header_len);

	for (int i = 0; i < 9; ++i)
		printf("%d %8d %2x %2d\n", i, res.freq[i], res.code_hex[i], res.len_code[i]);
}
