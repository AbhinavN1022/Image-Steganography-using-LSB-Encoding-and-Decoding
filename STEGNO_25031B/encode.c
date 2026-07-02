#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    YELLOW("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    YELLOW("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    fseek(fptr, 0, SEEK_END); // Move pointer to end of the file
    uint len = ftell(fptr);   // get file size
    return len;               // return file size
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char *fname1 = strstr(argv[2], ".bmp"); // Check if source image has .bmp extension
    if (fname1 != NULL && strcmp(fname1, ".bmp") == 0)
    {
        // printf("Enter the correct format(.bmp)\n");
        encInfo->src_image_fname = argv[2]; // Store source image name
    }
    else
    {
        return e_failure;
    }

    char *ptr = strrchr(argv[3], '.');   //Get last occurence of . in secret file
   //validate allowed secret file extension
    if (ptr != NULL &&                 
        (strcmp(ptr, ".txt") == 0) ||
        (strcmp(ptr, ".c") == 0) ||
        (strcmp(ptr, ".h") == 0) ||
        (strcmp(ptr, ".sh") == 0))                
    {
        encInfo->secret_fname = argv[3];  //store secret file name
    }
    else
    {
        return e_failure;
    }

    if (argv[4] != NULL) // Check the output image name is given or not
    {
        char *fname2 = strstr(argv[4], ".bmp"); // Check if output image has .bmp extension
        if (fname2 != NULL && strcmp(fname2, ".bmp") == 0)
        {
            encInfo->stego_image_fname = argv[4];       //Store stego image name
        }  
        else
        {

            return e_failure;            //return failure if not bmp
        }
    }
    else
    {
        YELLOW("Output file not mentioned. Creating destination.bmp as default\n");
        encInfo->stego_image_fname = "destination.bmp";  //Default output image
    }
    return e_success;      //Return success after validation
}
//open required files for encoding
Status open_files(EncodeInfo *encInfo)
{
    YELLOW("INFO: Opening required files\n");

    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
        return e_failure;
    }

    // No failure return e_success
    YELLOW("INFO: Done\n");
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);   //get image capacity
    YELLOW("INFO: Checking for %s size",encInfo->secret_fname);

    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);     //get secret file size

    if (encInfo->image_capacity > (16 + 32 + 32 + 32 + (encInfo->size_secret_file) * 8) && encInfo->size_secret_file !=0)
    {
        YELLOW("INFO: Done. Not empty\n");
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    YELLOW("INFO: Copying Image header\n");

    char imagebuffer[54];                      // creating a character array
    rewind(fptr_src_image);                    // rewind the file pointer to 0th position
    fread(imagebuffer, 54, 1, fptr_src_image); // Read the 54 bytes from the src image and store into the imagebuffer
    fwrite(imagebuffer, 54, 1, fptr_dest_image);//write header to output image

    if (ftell(fptr_src_image) == ftell(fptr_dest_image))
    {
        YELLOW("INFO: Done\n");
        return e_success;
    }
    else
    {
        YELLOW("INFO: Failed\n");
        return e_failure;
    }
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    YELLOW("INFO: Encoding Magic String\n");

    char imagebuffer[8];
    int len = strlen(magic_string); // Find the length of the magic string

    for (int i = 0; i < len; i++)
    {
        fread(imagebuffer, 8, 1, encInfo->fptr_src_image);    // Read the 8 bytes from the src image and store into image buffer
        encode_byte_to_lsb(magic_string[i], imagebuffer);     // Encode character
        fwrite(imagebuffer, 8, 1, encInfo->fptr_stego_image); // write imagebuffer into the destination image
    }
    YELLOW("INFO: Done\n");
    return e_success;
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    YELLOW("INFO: Encoding File extension size\n");

    char imagebuffer[32];
    fread(imagebuffer, 32, 1, encInfo->fptr_src_image);    // Read the 32 bytes from the src image and store into image buffer
    encode_size_to_lsb(size, imagebuffer);                 // Encode size
    fwrite(imagebuffer, 32, 1, encInfo->fptr_stego_image); // write imagebuffer into the destination image

    return e_success;
}
// Encode secret file extension
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    YELLOW("INFO: Encoding File extension\n");

    char imagebuffer[8];
    int len = strlen(file_extn); // Find extension length

    for (int i = 0; i < len; i++)
    {
        fread(imagebuffer, 8, 1, encInfo->fptr_src_image); // Read the 8 bytes from the src image and store into image buffer
        encode_byte_to_lsb(file_extn[i], imagebuffer);     // Encode character
        fwrite(imagebuffer, 8, 1, encInfo->fptr_stego_image);  //Write to output image
    }
    return e_success;
}

// Encode Secret file size
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{

    YELLOW("INFO: Encoding file size\n");

    char imagebuffer[32];
    fread(imagebuffer, 32, 1, encInfo->fptr_src_image); // Read the 32 bytes from the src image and store into image buffer
    encode_size_to_lsb(file_size, imagebuffer);         // Encode size
    fwrite(imagebuffer, 32, 1, encInfo->fptr_stego_image); //Write to output image

    return e_success;
}
// Encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    YELLOW("INFO: Encoding file data\n");

    char imagebuffer[8];
    char ch;

    rewind(encInfo->fptr_secret);                    //Move to start of secret file

    while(fread(&ch,1,1,encInfo->fptr_secret)>0)    //Read each byte
    {
        fread(imagebuffer, 8, 1, encInfo->fptr_src_image); // Read the 8 bytes from the src image and store into image buffer
        encode_byte_to_lsb(ch, imagebuffer);
        fwrite(imagebuffer, 8, 1, encInfo->fptr_stego_image); // write imagebuffer into the destination image
    }

    return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{

    YELLOW("INFO: Copying remaining data\n");

    char ch;
    while (fread(&ch, 1, 1, fptr_src) > 0)    //Read remaining bytes
    {
        fwrite(&ch, 1, 1, fptr_dest);     //write to output image
    }
    return e_success;
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        char get_bit = (data >> (7 - i)) & 0x01;        // get the bit from data
        image_buffer[i] = image_buffer[i] & 0xFE;    // clear the lsb bit of imagebuffer[i];
        image_buffer[i] = image_buffer[i] | get_bit; // set the get bit on lsb of imagebuffer[i];
    }
    return e_success;
}
Status encode_size_to_lsb(int size, char *imageBuffer)
{
    for (int i = 0; i < 32; i++)
    {
        char get_bit = (size >> (31 - i)) & 0x01;  // get the bit from data
        imageBuffer[i] = imageBuffer[i] & 0xFE; // clear the lsb bit of imagebuffer[i];
        imageBuffer[i] = imageBuffer[i] | get_bit; //Set LSB
    }
    return e_success;
}
Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) != e_success) // Open required files
        return e_failure;

    YELLOW("INFO: ## Encoding Procedure Started ##\n");

    if (check_capacity(encInfo) != e_success) // Check image capacity
        return e_failure;

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
        return e_failure;

    if (encode_magic_string(MAGIC_STRING, encInfo) != e_success)
        return e_failure;

       char *extn=strrchr(encInfo->secret_fname,'.');   //Extract file extension
       int extn_size=strlen(extn);                      //find the extension size

    if (encode_secret_file_extn_size(extn_size, encInfo) != e_success)
        return e_failure;

    if (encode_secret_file_extn(extn, encInfo) != e_success)
        return e_failure;
        
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) != e_success)
        return e_failure;

    if (encode_secret_file_data(encInfo) != e_success)
        return e_failure;

    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
        return e_failure;

    return e_success;
}
