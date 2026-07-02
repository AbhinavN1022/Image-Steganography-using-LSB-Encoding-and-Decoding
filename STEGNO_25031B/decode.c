#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status read_and_validate_decode_args(char *argv[], DEcodeInfo *decInfo)
{
    char *ptr = strrchr(argv[2], '.');           // Find the last occurence of '.' in file name
    if (ptr == NULL || strcmp(ptr, ".bmp") != 0) // Check if input file is a .bmp
    {
        return e_failure;
    }
    else
    {
        decInfo->stego_image_fname = argv[2]; // store stego image file name
    }

    if (argv[3] != NULL) // Check if output file name is given
    {
        decInfo->output_fname = argv[3]; // Store user gievn output file name
    }
    else
    {
        decInfo->output_fname = NULL; // Set NULL to generate default file name
    }
    return e_success;
}

Status open_files_decode(DEcodeInfo *decInfo)
{
    YELLOW("INFO: Opening required files for decoding\n"); // Print the INFO message

    // Src Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb"); // Open stego image in read mode
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

        return e_failure;
    }
    return e_success;
}

Status decode_magic_string(const char *magic_string, DEcodeInfo *decInfo)
{
    YELLOW("INFO: Decoding magic string\n");

    char decode_char;               // variable to store decoded character
    char imagebuffer[8];            // create an imagebuffer of 8 bytes
    int len = strlen(magic_string); // Find the length of the magic string
    for (int i = 0; i < len; i++)
    {
        fread(imagebuffer, 8, 1, decInfo->fptr_stego_image); // read 8 bytes from image
        decode_byte_from_lsb(&decode_char, imagebuffer);     // Decode one character from LSB
        if (decode_char != magic_string[i])                  // Compare decoded character with expected one
        {
            return e_failure;
        }
    }
    YELLOW("INFO: Magic string matched\n");
    return e_success;
}
// Decode secret file extension size
Status decode_secret_file_extn_size(int *size, DEcodeInfo *decInfo)
{
    YELLOW("INFO: Decoding secret file extension size\n"); // print info message

    char imagebuffer[32];                                 // create an imagebuffer of 32 bytes
    *size = 0;                                            // Initialize size to zero
    fread(imagebuffer, 32, 1, decInfo->fptr_stego_image); // read 8 bytes from image
    decode_size_from_lsb(size, imagebuffer);              // Decode extension size from lsb

    YELLOW("INFO: Extension size = %d\n", *size); // print decode extension size
    return e_success;
}
Status decode_secret_file_extn(char *file_extn, int size, DEcodeInfo *decInfo)
{

    YELLOW("INFO: Decoding secret file extension\n");

    char imagebuffer[8]; // create an imagebuffer of 8 bytes
    char ch;             // variable to store decoded character
    for (int i = 0; i < size; i++)
    {
        fread(imagebuffer, 8, 1, decInfo->fptr_stego_image); // Read the 8 bytes
        decode_byte_from_lsb(&ch, imagebuffer);              // decode one character
        file_extn[i] = ch;                                   // Store character in extension array
    }
    file_extn[size] = '\0';                                   // NULL terminate the extension string
    YELLOW("INFO: Decoded file extension = %s\n", file_extn); // print extension
    return e_success;
}
Status decode_secret_file_size(long *file_size, DEcodeInfo *decInfo)
{
    YELLOW("INFO: Decoding secret file size\n"); // print info message

    char imagebuffer[32]; // create an imagebuffer of 32 bytes
    int size = 0;         // temp variable to store size

    fread(imagebuffer, 32, 1, decInfo->fptr_stego_image); // Read 32 bytes
    decode_size_from_lsb(&size, imagebuffer);             // Decode file size from lab

    *file_size = size;                                         // store decoded size
    YELLOW("INFO: Secret file size= %ld bytes\n", *file_size); // print file size
    return e_success;
}
// decode secret file data
Status decode_secret_file_data(DEcodeInfo *decInfo) // print info message
{

    YELLOW("INFO: Decoding secret file data\n");

    char imagebuffer[8]; // create an imagebuffer of 8 bytes
    char ch;             // variable to store decoded byte
    for (long i = 0; i < decInfo->secret_file_size; i++)
    {
        fread(imagebuffer, 8, 1, decInfo->fptr_stego_image); // Read the 8 bytes
        decode_byte_from_lsb(&ch, imagebuffer);              // decode character
        fwrite(&ch, 1, 1, decInfo->fptr_output);             // write to output file
    }

    YELLOW("INFO: Secret file data decoded successfully\n");
    return e_success;
}
//decode a byte from LSB
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    *data= 0;   //initalize data to zero
    for (int i = 0; i < 8; i++)   //run the loop for 8 times
    {
        *data = (*data << 1) | (image_buffer[i] & 1);  //Extract lsb and build byte
    }
    return e_success;
}
//Decode size from LSB
Status decode_size_from_lsb(int *size, char *imageBuffer)
{
    *size = 0;   //initalize size to zero
    for (int i = 0; i < 32; i++)  //run the loop for 32 times
    {

        *size = (*size << 1) | (imageBuffer[i] & 1);   //Extract lsb and build integer
    }

    return e_success;
}

//Perform decoding
Status do_decoding(DEcodeInfo *decInfo)
{
    if (open_files_decode(decInfo) != e_success)   //Open required files
    {
        printf("Error opening files\n");  //print error message
        return e_failure;
    }
    else
    {
        YELLOW("INFO: Skipping BMP header (54 bytes)\n");   //print info message

        fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    }

    if (decode_magic_string(MAGIC_STRING, decInfo) != e_success)  //Decode magic string
    {
        printf("ERROR Decoding magic string\n");
        return e_failure;
    }

    if (decode_secret_file_extn_size(&decInfo->extn_size, decInfo) != e_success)  //Decode extension size
    {
        printf("ERROR Decoding secret file extension size\n");
        return e_failure;
    }

    if (decInfo->extn_size >= sizeof(decInfo->extn))   //Check extension size limit
    {
        printf("ERROR Extension size too large\n");
        return e_failure;
    }

    if (decode_secret_file_extn(decInfo->extn, decInfo->extn_size, decInfo) != e_success)  //Decode extension
    {
        printf("ERROR Decoding secret file extension\n");
        return e_failure;
    }
    if (decInfo->output_fname == NULL)  //Check if output file name not given
    {
        static char output_name[50];   //static buffer for output file name
        sprintf(output_name, "decoded%s", decInfo->extn);  //Create default file name
        decInfo->output_fname = output_name;   //Assign the file name
    }

    YELLOW("INFO: Creating output file %s\n", decInfo->output_fname);  //print info message

    decInfo->fptr_output = fopen(decInfo->output_fname, "wb");  //open output file
    if (decInfo->fptr_output == NULL)  //check if open properly
    {
        printf("ERROR Opening output file\n");
        return e_failure;
    }

    YELLOW("INFO: Output file opened successfully\n");

    if (decode_secret_file_size(&decInfo->secret_file_size, decInfo) != e_success)  //Decode file size
    {
        printf("ERROR Decoding secret file size\n");
        return e_failure;
    }

    if (decode_secret_file_data(decInfo) != e_success)  //Decode file data
    {
        printf("ERROR Decoding secret file data\n");
        return e_failure;
    }

    fclose(decInfo->fptr_stego_image);   //close stego image file
    fclose(decInfo->fptr_output);  //close output file
 
    return e_success;
}
