#include<stdio.h>              // This header file is used for standard input and output functions
#include<string.h>             // This header file is used for string handling functions
#include "encode.h"            // This file contains function declarations related to encoding
#include "types.h"             // This file contains user defined data types and structures
#define RED     "\033[1;91m"    // Bright Red
#define GREEN   "\033[1;92m"    // Bright Green
#define YELLOW  "\033[1;93m"    // Bright Yellow
#define BLUE "\033[1;94m"
#define RESET   "\033[0m"       // Reset colour

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo) // This function reads and validates command line arguments
{
    // Check required arguments
    if (argv[2] == NULL || argv[3] == NULL) // Check whether input image and secret file arguments are provided
    {
        printf(RED "Error: Missing arguments\n" RESET); // Print error message if arguments are missing
        printf("Usage: ./a.out -e <image.bmp> <secret.txt> [output.bmp]\n"); // Display correct usage format
        return e_failure; // Return failure if arguments are not valid
    }

    // Validate BMP
    char *ext1 = strrchr(argv[2], '.'); // Extract file extension from input image name
    if (ext1 == NULL || strcmp(ext1, ".bmp") != 0) // Check whether the file extension is .bmp
    {
        printf(RED "Error: Input image must be .bmp\n" RESET); // Print error if image is not BMP format
        return e_failure; // Return failure if validation fails
    }
    encInfo->src_image_fname = argv[2]; // Store the valid input image file name

    // Validate TXT
    char *ext2 = strrchr(argv[3], '.'); // Extract file extension from secret file name
    if (ext2 == NULL || strcmp(ext2, ".txt") != 0) // Check whether the file extension is .txt
    {
        printf(RED "Error: Secret file must be .txt\n" RESET); // Print error if file is not TXT format
        return e_failure; // Return failure if validation fails
    }
    encInfo->secret_fname = argv[3]; // Store the valid secret file name

    // Optional output file
    if (argv[4] != NULL) // Check if output file name is provided
    {
        encInfo->stego_image_fname = argv[4]; // Store the given output file name
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp"; // Assign default output file name if not provided
    }

    return e_success; // Return success after successful validation
}
Status do_encoding(EncodeInfo *encInfo){   // This function performs the complete encoding process step by step
   printf(YELLOW "\n============================================================\n" RESET);
printf(YELLOW "                ENCODING PROCESS STARTED\n" RESET);
printf(YELLOW "============================================================\n\n" RESET);
    if( open_files(encInfo) == e_success){   // Check if all required files are opened successfully
        printf(GREEN "All files opened successfully\n" RESET);
    }
    else{
        printf(RED "All files did not opened successfully\n" RESET);
        return e_failure;   // Return failure if file opening fails
    }

    printf(BLUE "Enter magic string: " RESET);   // Ask user to enter a magic string
    scanf(" %[^\n]",encInfo -> magic);   // Read the magic string including spaces

    if(check_capacity(encInfo)== e_success){   // Check whether image has enough capacity to store data
        printf(GREEN "Capacity checked Successfully\n" RESET);
    }
    else{
        printf(RED "Capacity of the source file is less\n" RESET);
        fclose(encInfo->fptr_src_image);   // Close source image file
        fclose(encInfo->fptr_secret);      // Close secret file
        fclose(encInfo->fptr_stego_image); // Close output file
        return e_failure;   // Return failure if capacity is insufficient
    }

    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success){   // Copy first 54 bytes (header) from source to output image
        printf(GREEN "Header copied successfully\n" RESET);
    }
    else{
        printf(RED "Header not copied successfully\n" RESET);
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);
        return e_failure;   // Return failure if header copy fails
    }
     
    if(encode_magic_string(encInfo->magic,encInfo) == e_success){   // Encode the magic string into image using LSB technique
        printf(GREEN "Magic string encoded successfully\n" RESET);
    }
    else{
        printf(RED "Magic string did not encode successfully\n" RESET);
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);
        return e_failure;   // Return failure if magic string encoding fails
    }

    if(encode_secret_file_extn_size(strlen(strchr(encInfo->secret_fname,'.')),encInfo) == e_success){   // Encode the size of file extension into image
        printf(GREEN "Size of the secret extn file encoded successfully\n" RESET);
    }
    else{
        printf(RED "Size of the secret extn file not encoded\n" RESET);
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);
        return e_failure;   // Return failure if extension size encoding fails
    }

    if(encode_secret_file_extn( strchr(encInfo->secret_fname,'.'), encInfo) == e_success){   // Encode the actual file extension (like .txt)
        printf(GREEN "Secret extn encoded successfully\n" RESET);
    }
    else{
        printf(RED "Secret extn encoded unsuccessfully\n" RESET);
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);
        return e_failure;   // Return failure if extension encoding fails
    }

    if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success){   // Encode the size of the secret file
        printf(GREEN "Secret file size encoded success\n" RESET);
    }
    else{
        printf(RED "Secret file size not encoded\n" RESET);
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);
        return e_failure;   // Return failure if file size encoding fails
    }

    if(encode_secret_file_data(encInfo) == e_success){   // Encode the actual content of the secret file into image
        printf(GREEN "Secret file data encoded successfully\n" RESET);
    }
    else{
        printf(RED "Secret file data not encoded\n" RESET);
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);
        return e_failure;   // Return failure if data encoding fails
    }

    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success){   // Copy remaining image data after encoding is done
        printf(GREEN "Remaining data copied successfully\n" RESET);
    }
    else{
        printf(RED "Remaining data does not copied successfully\n" RESET);
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);
        return e_failure;   // Return failure if remaining data copy fails
    }

    /* BUG FIX: Close all files to flush OS buffers to disk */
    fclose(encInfo->fptr_src_image);   // Close source image file
    fclose(encInfo->fptr_secret);      // Close secret file
    fclose(encInfo->fptr_stego_image); // Close output file

    return e_success;   // Return success after completing encoding process
}

Status open_files(EncodeInfo *encInfo)   // This function opens source, secret and output files in binary mode
{
    /* BUG FIX: Open all files in binary mode ("rb"/"wb") */
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");   // Open source image file in read binary mode
    if(encInfo->fptr_src_image == NULL)   // Check if source file failed to open
    {
        printf(RED"Source file is not opened\n"RESET);
        return e_failure;   // Return failure if source file not opened
    }
    else printf(GREEN "Source file is opened\n" RESET);   // Print success message

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");   // Open secret file in read binary mode
    if(encInfo->fptr_secret == NULL)   // Check if secret file failed to open
    {
        printf(RED"Secret file is not opened\n"RESET);
        fclose(encInfo->fptr_src_image);   // Close already opened source file
        return e_failure;   // Return failure
    }
    else printf(GREEN "Secret file is opened\n" RESET);   // Print success message

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");   // Open output file in write binary mode
    if(encInfo->fptr_stego_image == NULL)   // Check if output file failed to open
    {
        printf(RED"Stego file is not opened\n" RESET);
        fclose(encInfo->fptr_src_image);   // Close source file
        fclose(encInfo->fptr_secret);      // Close secret file
        return e_failure;   // Return failure
    }
    else printf(GREEN "Stego file is opened\n" RESET);   // Print success message

    return e_success;   // Return success if all files opened properly
}

Status check_capacity(EncodeInfo *encInfo){   // This function checks whether the image has enough capacity to store secret data
   encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);   // Get total image capacity in bytes
   encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);   // Get size of secret file

   if(encInfo->image_capacity > (strlen(encInfo->magic)*8 + 32 + 32 + 32 + encInfo->size_secret_file*8)){   // Check if image can store all required data bits
        return e_success;   // Return success if capacity is sufficient
   }
   return e_failure;   // Return failure if capacity is not enough
}

uint get_image_size_for_bmp(FILE *fptr_image){   // This function calculates total image data size for BMP file
    int wid, len;   // Variables to store width and height
    fseek(fptr_image, 18, SEEK_SET);   // Move file pointer to width position in BMP header
    fread(&wid, 4, 1, fptr_image);   // Read width of image
    fread(&len, 4, 1, fptr_image);   // Read height of image
    return wid * len * 3;   // Return total size (width × height × 3 bytes for RGB)
}

uint get_file_size(FILE *fptr){   // This function returns size of given file
    fseek(fptr, 0, SEEK_END);   // Move file pointer to end of file
    return ftell(fptr);   // Return current position which is file size
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){   // This function copies the first 54 bytes (header) from source BMP to destination BMP
    rewind(fptr_src_image);   // Move file pointer to beginning of source image
    char buffer[54];   // Buffer to store header bytes
    fread(buffer, 54, 1, fptr_src_image);   // Read 54 bytes of BMP header
    fwrite(buffer, 54, 1, fptr_dest_image);   // Write header to destination image
    return e_success;   // Return success after copying header
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo){   // This function encodes the magic string into the image
    if(encode_data_to_image(magic_string, strlen(magic_string), encInfo) == e_success){   // Encode magic string using LSB method
        printf(GREEN "Magic string encoded\n" RESET);
        return e_success;   // Return success if encoding is done
    }
    printf(RED "Magic string not encoded\n" RESET);   // Print error if encoding fails
    return e_failure;   // Return failure
}

Status encode_data_to_image(char *data, int size, EncodeInfo *encoInfo){   // This function encodes each byte of data into image buffer
    char buffer[8];   // Buffer to store 8 bytes of image data
    for(int i = 0; i < size; i++){   // Loop through each byte of data
        fread(buffer, 8, 1, encoInfo->fptr_src_image);   // Read 8 bytes from source image
        encode_byte_to_lsb(data[i], buffer);   // Encode one byte of data into 8 bytes using LSB
        fwrite(buffer, 8, 1, encoInfo->fptr_stego_image);   // Write modified bytes to output image
    }
    return e_success;   // Return success after encoding data
}

Status encode_byte_to_lsb(char data, char *image_buffer)   // This function encodes one byte of data into 8 bytes using least significant bits
{
    for(int i = 0; i < 8; i++)   // Loop for 8 bits of the character
    {
        int bit = (data >> (7 - i)) & 1;   // Extract each bit from MSB to LSB
        image_buffer[i] = (image_buffer[i] & 0xFE) | bit;   // Replace LSB of image byte with data bit
    }
    return e_success;   // Return success after encoding byte
}

Status encode_secret_file_extn_size(int data, EncodeInfo *encInfo){   // This function encodes the size of secret file extension into image
    return encode_int_lsb(data, encInfo);   // Call function to encode integer using LSB
}
Status encode_int_lsb(int data, EncodeInfo *encInfo)   // This function encodes an integer (32 bits) into image using LSB technique
{
    char buffer[32];   // Buffer to store 32 bytes from image
    fread(buffer, 32, 1, encInfo->fptr_src_image);   // Read 32 bytes from source image
    for(int i = 0; i < 32; i++)   // Loop through 32 bits of integer
    {
        int bit = (data >> (31 - i)) & 1;   // Extract each bit from MSB to LSB
        buffer[i] = (buffer[i] & 0xFE) | bit;   // Replace LSB of each byte with data bit
    }
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);   // Write modified bytes to output image
    return e_success;   // Return success after encoding integer
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo){   // This function encodes the file extension (like .txt) into image
    return encode_data_to_image(file_extn, strlen(file_extn), encInfo);   // Call function to encode extension string
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo){   // This function encodes the size of secret file
    return encode_int_lsb((int)file_size, encInfo);   // Convert size to int and encode using LSB
}

Status encode_secret_file_data(EncodeInfo *encInfo){   // This function encodes the actual secret file content into image
    /* BUG FIX: rewind secret file before reading (get_file_size left it at EOF) */
    rewind(encInfo->fptr_secret);   // Move file pointer to beginning of secret file
    char buffer[encInfo->size_secret_file];   // Buffer to store secret file data
    fread(buffer, encInfo->size_secret_file, 1, encInfo->fptr_secret);   // Read entire secret file into buffer
    return encode_data_to_image(buffer, encInfo->size_secret_file, encInfo);   // Encode file data into image
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest){   // This function copies remaining image data after encoding
    char ch;   // Variable to store one byte
    while(fread(&ch, 1, 1, fptr_src)){   // Read one byte at a time until end of file
        fwrite(&ch, 1, 1, fptr_dest);   // Write that byte to destination image
    }
    return e_success;   // Return success after copying remaining data
}