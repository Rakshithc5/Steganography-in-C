#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

typedef struct _DecodeInfo
{
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char output_fname[100];
    FILE *fptr_output;

    int  extn_size;
    char extn[MAX_FILE_SUFFIX];

    long file_size;

    char magic[MAX_MAGIC_LEN];

} DecodeInfo;

/* Functions */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status open_decode_files(DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);

/* Helpers */
void decode_byte_from_lsb(char *data, FILE *fptr);
void decode_int_from_lsb_int(int *data, FILE *fptr);
void decode_int_from_lsb_long(long *data, FILE *fptr);
void remove_extn(char *fname);

#endif