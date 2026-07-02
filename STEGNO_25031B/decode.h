
#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _DEcodeInfo
{
    

    /* Stego Image Info */
    char *stego_image_fname; // To store the dest file name
    FILE *fptr_stego_image;  // To store the address of stego image
    
    char * output_fname;    //To store output file name
    FILE *fptr_output;     // To store the address of output image

    int extn_size;        //To store secret file extension size
    char extn[10];        //To store secret file extension
    long secret_file_size; //To store decoded secret file size

} DEcodeInfo;

/* Encoding function prototype */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DEcodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DEcodeInfo *decInfo);


/* Get File pointers for i/p and o/p files */
Status open_files_decode(DEcodeInfo *decInfo);


/* Store Magic String */
Status decode_magic_string(const char *magic_string, DEcodeInfo *decInfo);

/*Encode extension size*/
Status decode_secret_file_extn_size(int *size, DEcodeInfo *decInfo);

/* Encode secret file extenstion */
Status decode_secret_file_extn( char *file_extn,int size, DEcodeInfo *decInfo);

/* Encode secret file size */
Status decode_secret_file_size(long *file_size, DEcodeInfo *decInfo);

/* Encode secret file data*/
Status decode_secret_file_data(DEcodeInfo *decInfo);

/* Encode a byte into LSB of image data array */
Status decode_byte_from_lsb(char *data, char *image_buffer);

// Encode a size to lsb
Status decode_size_from_lsb(int *size, char *imageBuffer);

/* Copy remaining image bytes from src to stego image after encoding */
//Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
