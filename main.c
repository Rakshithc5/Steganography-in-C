/* 
===========================================================
                   STEGANOGRAPHY 
===========================================================

Name        : Rakshith c
Date        : 04/05/2026
Batch No    : 26001B

Description :
-------------
This project implements a Steganography system in C
using the Least Significant Bit (LSB) technique to hide and extract secret data inside a BMP image. 
It supports encoding a text file into an image and decoding it back using command-line arguments.
The system ensures data integrity by using a magic string for verification. 
It also handles file extension and file size during the process. 
Proper validation and file handling are implemented to ensure reliable and secure operation.

Features:
---------
1. Encoding (Hiding Data)
   -Reads and validates input arguments for encoding
    -Opens source image, secret file, and output stego file
    -Accepts magic string from user for data verification
    -Checks image capacity 
    -Copies BMP header to preserve image structure
    -Encodes magic string into image using LSB technique
    -Encodes secret file extension and its size
    -Encodes secret file size and actual file data
    -Copies remaining image data to output file
    -Closes all files after successful encoding
2. Decoding (Extracting Data)
    - Extracts hidden data from stego BMP image
    - Uses LSB (Least Significant Bit) technique
    - Verifies magic string for authentication
    - Decodes file extension and file size
    - Reconstructs original secret file from image

3. Command-Line Based Execution
   - Uses arguments like -e for encoding and -d for decoding
   - Example:
     ./a.out -e input.bmp secret.txt
     ./a.out -d stego.bmp

4. File Handling
   - Opens files in binary mode
   - Reads and writes image and secret data properly
   - Generates output file with correct extension

5. Validation
   - Accepts only .bmp files for image input
   - Accepts only .txt file for encoding
   - Validates magic string during decoding
   - Checks for invalid file size and extension

6. Error Handling
   - Handles file opening errors
   - Handles invalid arguments
   - Handles insufficient image capacity
   - Displays clear success and error messages

===========================================================
*/

#include <stdio.h>              // This header file is used for input and output functions
#include "encode.h"             // This header contains encoding related declarations
#include "types.h"              // This header contains user defined types
#include <string.h>             // This header is used for string comparison functions
#include "decode.h"             // This header contains decoding related declarations
#define RED     "\033[1;91m"    // Bright Red
#define GREEN   "\033[1;92m"    // Bright Green
#define YELLOW  "\033[1;93m"    // Bright Yellow
#define RESET   "\033[0m"       // Reset colour

int main(int argc, char *argv[])   // This is the main function where execution starts
{
    if(argc<3)   // Check if minimum required arguments are provided
    {
        printf(RED "invalid arguments "RESET"\nfor encode -> ./a.out -e beautiful.bmp secret.txt\n");   // Print encode usage
        printf("for decoding -> ./a.out -d stego.bmp\n");   // Print decode usage
        return e_failure;   // Return failure if arguments are insufficient
    }

    if(check_operation_type(argv)==e_encode)   // Check if user selected encoding option
    {
        printf(YELLOW "you chosed encoding\n"RESET);   // Print encoding message

        EncodeInfo encInfo;   // Create structure variable for encoding

        if(read_and_validate_encode_args(argv, &encInfo)==e_success)   // Validate encode arguments
        {
            printf(GREEN "Read and validate successfull\n" RESET);   // Print success message

            if(do_encoding(&encInfo)==e_success)   // Perform encoding process
            {
                printf(GREEN "Encoding successfull\n" RESET );   // Print encoding success
                return e_success;   // Return success
            }
        }
        else
        {
            printf(GREEN "Read and validate unsuccessfull\n" RESET);   // Print validation failure
        }
        return e_success;   // Return success even if encoding fails internally
    }
    else if(check_operation_type(argv)==e_decode)   // Check if user selected decoding option
    {
        printf(YELLOW "you chosing decoding\n"RESET);   // Print decoding message

        DecodeInfo decInfo;   // Create structure variable for decoding

        if(read_and_validate_decode_args(argv, &decInfo) == e_success)   // Validate decode arguments
        {
            if(do_decoding(&decInfo) == e_success)   // Perform decoding process
            {
                printf(GREEN "Decoding successful\n" RESET);   // Print decoding success
            }
            else
            {
                printf(RED "Decoding failed\n" RESET);   // Print decoding failure
            }
        }
        else
        {
            printf(RED "Invalid decode arguments\n" RESET);   // Print invalid argument message
        }

        return e_success;   // Return success after decoding flow
    }
    else   // If neither encoding nor decoding option is selected
    {
        printf(RED "invalid arguments\n" RESET);
        printf("for encode -> ./a.out -e beautiful.bmp secret.txt\n");   // Print encode usage
        printf("for decoding -> ./a.out -d stego.bmp\n");   // Print decode usage
        return e_failure;   // Return failure
    }
}

OperationType check_operation_type(char *argv[])   // This function checks whether user selected encode or decode
{
    if(strcmp(argv[1], "-e")==0)   // Compare argument with "-e"
    {
        return e_encode;   // Return encode operation type
    }
    else if(strcmp(argv[1], "-d")==0)   // Compare argument with "-d"
    {
        return e_decode;   // Return decode operation type
    }
    else
    {
        return e_unsupported;   // Return unsupported operation if input is invalid
    }
}