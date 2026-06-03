#include <stdio.h>      // This header file is used for input output functions
#include <string.h>     // This header file is used for string handling functions
#include "decode.h"     // This header file contains decode related declarations
#define RED     "\033[1;91m"    // Bright Red
#define GREEN   "\033[1;92m"    // Bright Green
#define YELLOW  "\033[1;93m"    // Bright Yellow
#define BLUE "\033[1;94m"
#define RESET   "\033[0m"       // Reset colour

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)   // This function validates decode arguments
{
    if (strstr(argv[2], ".bmp") != NULL)   // Check whether input file is a BMP image
    {
        decInfo->stego_image_fname = argv[2];   // Store stego image file name
    }
    else
    {
        printf(RED "Invalid stego image file\n" RESET);   // Print error if file is not BMP
        return e_failure;   // Return failure
    }

    if (argv[3] != NULL)   // Check if output file name is provided
    {
        strcpy(decInfo->output_fname, argv[3]);   // Copy given output file name
    }
    else
    {
        strcpy(decInfo->output_fname, "decoded");   // Assign default output file name
    }

    return e_success;   // Return success after validation
}

Status open_decode_files(DecodeInfo *decInfo)   // This function opens the stego image file in binary mode for decoding
{
    /* BUG FIX: open in binary mode */
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");   // Open stego image in read binary mode
    if (decInfo->fptr_stego_image == NULL)   // Check if file opening failed
    {
        printf(RED "Unable to open stego image\n" RESET);   // Print error message
        return e_failure;   // Return failure
    }
    return e_success;   // Return success if file opened
}

void remove_extn(char *fname)   // This function removes file extension from file name
{
    char *dot = strrchr(fname, '.');   // Find position of '.' in file name
    if (dot != NULL) *dot = '\0';   // Replace '.' with null character to remove extension
    if (fname[0] == '\0') strcpy(fname, "decoded");   // If name becomes empty, assign default name
}

Status do_decoding(DecodeInfo *decInfo)   // This function performs the complete decoding process
{
printf(YELLOW "\n============================================================\n" RESET);
printf(YELLOW "                 DECODING PROCESS STARTED\n" RESET);
printf(YELLOW "============================================================\n\n" RESET);
    printf(YELLOW "Opening stego image... " RESET);
    if (open_decode_files(decInfo) == e_failure)   // Open required files
    {
     printf(RED "FAILED\n" RESET);
        return e_failure;   // Return failure if file open fails
    }
    printf(GREEN "SUCCESS\n" RESET);
    printf(YELLOW "Preparing output file name... " RESET);
    remove_extn(decInfo->output_fname);   // Remove extension from output file name
    printf(GREEN "DONE\n" RESET);
printf(YELLOW "Skipping BMP header... "RESET );
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);   // Skip BMP header (54 bytes)
    printf(GREEN "DONE\n" RESET);

    printf(BLUE "Enter magic string: " RESET);   // Ask user to enter magic string
    scanf(" %[^\n]", decInfo->magic);   // Read magic string
   printf(YELLOW "Decoding magic string... "RESET);
    char decoded_magic[MAX_MAGIC_LEN];   // Buffer to store decoded magic string
    for (int i = 0; i < (int)strlen(decInfo->magic); i++)   // Loop through each character of magic string
    {
        decode_byte_from_lsb(&decoded_magic[i], decInfo->fptr_stego_image);   // Decode one byte from image
    }
    decoded_magic[strlen(decInfo->magic)] = '\0';   // Add null terminator

    if (strcmp(decoded_magic, decInfo->magic) != 0)   // Compare decoded magic with user input
    {
        printf(RED "Magic string mismatch\n" RESET);   // Print mismatch message
        fclose(decInfo->fptr_stego_image);   // Close file
        return e_failure;   // Return failure
    }
    printf(GREEN "Magic string verified\n" RESET);   // Print success message

    decode_int_from_lsb_int(&decInfo->extn_size, decInfo->fptr_stego_image);   // Decode extension size
    if (decInfo->extn_size <= 0 || decInfo->extn_size > MAX_FILE_SUFFIX)   // Validate extension size
    {
        printf(RED "Invalid extension size: %d\n" RESET, decInfo->extn_size);   // Print error
        fclose(decInfo->fptr_stego_image);   // Close file
        return e_failure;   // Return failure
    }

    for (int i = 0; i < decInfo->extn_size; i++)   // Loop to decode extension characters
        decode_byte_from_lsb(&decInfo->extn[i], decInfo->fptr_stego_image);   // Decode each byte
    decInfo->extn[decInfo->extn_size] = '\0';   // Add null terminator

    strcat(decInfo->output_fname, decInfo->extn);   // Append extension to output file name
    printf(BLUE "Output file: %s\n" RESET, decInfo->output_fname);   // Print output file name

    /* BUG FIX: open output in binary mode */
    decInfo->fptr_output = fopen(decInfo->output_fname, "wb");   // Open output file in write binary mode
    if (decInfo->fptr_output == NULL)   // Check if file creation failed
    {
        printf(RED "Output file creation failed\n" RESET);   // Print error
        fclose(decInfo->fptr_stego_image);   // Close input file
        return e_failure;   // Return failure
    }

    decode_int_from_lsb_long(&decInfo->file_size, decInfo->fptr_stego_image);   // Decode file size
    if (decInfo->file_size <= 0 || decInfo->file_size > 1000000)   // Validate file size
    {
        printf(RED "Invalid file size: %ld\n" RESET, decInfo->file_size);   // Print error
        fclose(decInfo->fptr_stego_image);   // Close files
        fclose(decInfo->fptr_output);
        return e_failure;   // Return failure
    }

    char ch;   // Variable to store one character
    for (long i = 0; i < decInfo->file_size; i++)   // Loop through file size
    {
        decode_byte_from_lsb(&ch, decInfo->fptr_stego_image);   // Decode one byte
        fputc(ch, decInfo->fptr_output);   // Write byte to output file
    }

    printf(GREEN "Decoding completed successfully\n" RESET);   // Print success message

    fclose(decInfo->fptr_stego_image);   // Close input file
    fclose(decInfo->fptr_output);   // Close output file

    return e_success;   // Return success
}
void decode_byte_from_lsb(char *data, FILE *fptr)   // This function decodes one character from image using LSB technique
{
    char buffer[8];   // Buffer to store 8 bytes from image
    fread(buffer, 8, 1, fptr);   // Read 8 bytes from image file
    char ch = 0;   // Variable to store reconstructed character
    for (int i = 0; i < 8; i++)   // Loop through 8 bits
        ch |= (buffer[i] & 1) << (7 - i);   // Extract LSB from each byte and rebuild character
    *data = ch;   // Store decoded character in given pointer
}

void decode_int_from_lsb_int(int *data, FILE *fptr)   // This function decodes a 32-bit integer from image using LSB technique
{
    char buffer[32];   // Buffer to store 32 bytes from image
    fread(buffer, 32, 1, fptr);   // Read 32 bytes from image file
    int value = 0;   // Variable to store reconstructed integer
    for (int i = 0; i < 32; i++)   // Loop through 32 bits
        value |= (buffer[i] & 1) << (31 - i);   // Extract LSB bits and rebuild integer value
    *data = value;   // Store decoded integer in given pointer
}

void decode_int_from_lsb_long(long *data, FILE *fptr)   // This function decodes a 32-bit long value from image using LSB technique
{
    char buffer[32];   // Buffer to store 32 bytes from image
    fread(buffer, 32, 1, fptr);   // Read 32 bytes from image file
    long value = 0;   // Variable to store reconstructed long value
    for (int i = 0; i < 32; i++)   // Loop through 32 bits
        value |= (long)(buffer[i] & 1) << (31 - i);   // Extract LSB bits and rebuild long value
    *data = value;   // Store decoded long value in given pointer
}