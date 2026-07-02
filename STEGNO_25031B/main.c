
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include"common.h"

OperationType check_operation_type(char *);   //Check the operation type

int main(int argc, char *argv[])
{
    if (argc < 3)   //Checkl if minimum arguments are provided
    {
        RED("Insufficient arguments\n");   //print error message
        return 0;
    }

    if (check_operation_type(argv[1]) == e_encode)   //Check if the operation is encode
    {
        if (argc < 4)   //Check if enough arguments for encoding
        {
            RED("Insufficient arguments for encoding\n");  //print error message
            return 0;
        }
        EncodeInfo encinfo;    //Declare structure for encoding

        if ((read_and_validate_encode_args(argv, &encinfo)) == e_success)  //Validate encode arguments
        {
            if (do_encoding(&encinfo) == e_success)  //Perform encoding operation
            {
                YELLOW("Secret file successfully encoded into the image\n");  //Success message
            }
            else
            {
                RED("Encoding Failed\n");  //print failure message
                return 0;
            }
        }
        else
        {
            RED("Invalid encoding arguments\n"); //print invalid argumnet message
        }
    }
    else if (check_operation_type(argv[1]) == e_decode)  //Check if the operation is decode
    {
        DEcodeInfo decinfo;  //Declare structure for decoding

        if (read_and_validate_decode_args(argv, &decinfo) == e_success)   //Validate decode arguments
        {
            if (do_decoding(&decinfo) == e_success)  //Perform decoding operation
            {
                YELLOW("INFO: Decoded completed successfully\n");  //Success message
            }
            else
            {
                RED("Decoding Failed\n");   //print failure message
                return 0;
            }
        }
        else
        {
            RED("Invalid decoding arguments\n");  //print invalid argumnet message
            return 0;
        }
    }
    else
    {
        RED("Invalid option\n");  //print invalid option
        return 0;
    }
}

OperationType check_operation_type(char *symbol)
{
    if (strcmp(symbol, "-e") == 0)   //Compare input with encode flag
    {
        return e_encode;    //return encode operation type
    }
    else if (strcmp(symbol, "-d") == 0)   //Compare input with decode flag
    {
        return e_decode;   //return decode operation type
    }
    else
    {
        return e_unsupported;   //return unsupported operation type
    }
}