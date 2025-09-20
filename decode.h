#ifndef DECODE_H
#define DECODE_H

#include <stdio.h> // Required for FILE type
#include <ctype.h>
#include "types.h" // Contains user-defined types like Status (e_success, e_failure)

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX1 5 // e.g., ".txt", ".sh" — 4 chars + '\0'

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;

    /* Decoding metadata */
    int decode_size_secret_file;
    int size_secret_file_extn;
    char extn_secret_file[MAX_FILE_SUFFIX1]; // Stores ".txt", ".c", etc.


    char magic_string[20];     
    int magic_str_len; 
  
} DecodeInfo;

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_image_file(DecodeInfo *decInfo);

/* Store Magic String length*/
Status decode_magic_string_length(int *magic_str_len, DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/*decode the secret file extension size */
Status decode_secret_file_extn_size(int *file_size, DecodeInfo *decInfo);

/*decode the secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);



/*Decode a LSB into byte image data array*/
Status decode_byte_from_lsb(char *data, char *image_buffer);

/*Decode secret file extension size*/
Status decode_size_from_lsb(int *data, char *buffer);



#endif
