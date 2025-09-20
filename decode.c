#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"

#define MAX_MAGIC_LEN 50




// Function to read and validate decode arguments from argv
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Initialize all file pointers to NULL to prevent undefined behavior
    decInfo->fptr_stego_image = NULL;
    decInfo->fptr_secret = NULL;

    printf("\nValidating decoding arguments...\n\n");

    // Validate stego image file extension
    char *bmp_ext = strrchr(argv[2], '.');
    if (bmp_ext != NULL && strcmp(bmp_ext, ".bmp") == 0)
    {
        decInfo->stego_image_fname = argv[2];
        printf("Source image validated: %s\n", argv[2]);
    }
    else
    {
        printf(" Error: Invalid stego image file. Expected a .bmp file\n");
        return e_failure;
    }
    // Handaling OutPut File
    if (argv[3] == NULL)
    {
        // User did not provide output filename, use default
        decInfo->secret_fname = "decode_secret";
    }
     else
    {
        char *ext = strrchr(argv[3], '.');

        if (ext != NULL)
        {
            // Extension found – reject it
            if (strcmp(ext, ".c") == 0 || strcmp(ext, ".sh") == 0 || strcmp(ext, ".txt") == 0)
            {
                printf("Error: Do not include file extension in output filename\n");
                printf("Just enter a file name without extension <output_file_name>\n");
                return e_failure;
            }
            else
            {
                printf("Error: Unknown extension provided. Please avoid using extensions\n");
                return e_failure;
            }
        }
        decInfo->secret_fname = argv[3];
    }
     printf("\nAll decode arguments validated successfully!\n");
     printf("#* Done  *#\n\n");
     return e_success;
}

// open image file
Status open_image_file(DecodeInfo *decInfo)
{
    printf("Opening required files...\n\n");
    // open the stego image file in read mode
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");

    // Check if the file was opened successfully
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        printf("Error: Failed to open the image file: %s\n", decInfo->stego_image_fname);
        return e_failure;
    }
    else
    {
        printf("Opened image file : %s\n", decInfo->stego_image_fname);
    }

    return e_success;
}

// Decode Magic string length
Status decode_magic_string_length(int *magic_str_len, DecodeInfo *decInfo)
{
     // Move the file pointer to the start of pixel data 
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    printf("Decode Magic String Length from image...\n");

    char buffer[32]; // 32 bytes buffer 

    // Read 32 bytes from source image where magic string length will be encoded
    if (fread(buffer, 32, 1, decInfo->fptr_stego_image) != 1)
    {
        printf("Error Unabel to  reading from source image while decodeing magic string length");
        return e_failure;
    }

    //call encode_size_to_lsb function to encode 
    if (decode_size_from_lsb(magic_str_len, buffer) != e_success)
    {
        printf("Failed to decode magic string length into image buffer\n");
        return e_failure;
    }

   

    return e_success;
}

// Decode magic string 
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
     printf("decoding Magic String from image...\n");

     for (int i = 0; i < decInfo->magic_str_len; i++)
     {
        char buffer[8];
        if (fread(buffer, 8, 1, decInfo->fptr_stego_image)!= 1)
        {
            printf("Error: Failed to read 8 bytes from source image");
            return e_failure;
        }

        char decoded_char;
        if (decode_byte_from_lsb(&decoded_char, buffer) != e_success)
        {
            printf("Error: Failed to encode byte");
            return e_failure;
        }

        decInfo->magic_string[i] = decoded_char;    
        
     }
      decInfo->magic_string[decInfo->magic_str_len] = '\0'; 
    return e_success;

    
}


/*decode the secret file extension size */
Status decode_secret_file_extn_size(int *file_size, DecodeInfo *decInfo)
{
    printf("Decoding secret file extension size from image...\n");

    char buffer[32];

    // Read 32 bytes from the source image
    if (fread(buffer, 32, 1, decInfo->fptr_stego_image) != 1)
    {
        printf("Error: Unable to read from source image while encoding file size\n");
        return e_failure;
    }

    // Encode the 32-bit file size into LSBs
    if (decode_size_from_lsb(file_size, buffer) != e_success)
    {
        printf("Error: Failed to encode file size into image buffer\n");
        return e_failure;
    }

    // Print success message
    return e_success;
}

/* Decode the secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    printf("Decoding secret file extension from image...\n");

    for (int i = 0; i < decInfo->size_secret_file_extn; i++)
     {
        char buffer[8];
        if (fread(buffer, 8, 1,decInfo->fptr_stego_image ) != 1)
        {
            printf("Error: Failed to read 8 bytes from source image");
            return e_failure;
        }
        char decoded_char;
        if( decode_byte_from_lsb(&decoded_char, buffer) != e_success) 
        {
            printf("Error: Failed to encode byte");
            return e_failure;
        }
        decInfo->extn_secret_file[i] = decoded_char;
        
     }
    decInfo->extn_secret_file[decInfo->size_secret_file_extn] = '\0';


    return e_success;
}


//Decode secret file size 
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    printf("Decoding secret file size from image...\n");

    char buffer[32];

    if (fread(buffer, 32, 1, decInfo->fptr_stego_image) != 1)
    {
        printf("Failed to read data for decoding file size\n");
        return e_failure;
    }
    if (decode_size_from_lsb(&(decInfo->decode_size_secret_file), buffer) != e_success)
    {
        printf("Error: Failed to decode secret file size from image buffer\n");
        return e_failure;
    }

    return e_success;

}

//Decode secret file  Data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    printf("Decoding secret file data...\n");

    for (int i = 0; i < decInfo->decode_size_secret_file; i++)
    {
        char buffer[8];
        if (fread(buffer, 8, 1, decInfo->fptr_stego_image) != 1)
        {
            printf("Error: Failed to read 8 bytes from source image\n");
            return e_failure;
        }

        char decoded_char;
        if (decode_byte_from_lsb(&decoded_char, buffer) != e_success)
        {
            printf("Error: Failed to decode byte from image\n");
            return e_failure;
        }

        if (fputc(decoded_char, decInfo->fptr_secret) == EOF)
        {
            printf("Error: Failed to write decoded byte to output file\n");
            return e_failure;
        }
    }

   

    return e_success;
}





/* Decode a byte from the LSBs of 8 image bytes */
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    char ch = 0;  // Variable to reconstruct the decoded byte

    for (int i = 0; i < 8; i++)  // Loop through 8 bytes in image_buffer
    {
        // ch = (ch << 1) | (image_buffer[i] & 1);  // Extract LSB and place it at correct bit position
        ch = ch << 1;                        // Make space for the new bit
        int bit = image_buffer[i] & 1;       // Extract LSB
        ch = ch | bit;   
    }

    *data = ch;  // Store decoded byte

    return e_success;
}



/*Decode secret file extension size*/
Status decode_size_from_lsb(int *data, char *image_buffer)
{
    int value = 0;  // Variable to reconstruct the integer

    for (int i = 0; i < 32; i++)  // Loop over 32 bytes holding encoded bits
    {
       
        value = value << 1;                 
        int bit = image_buffer[i] & 1;      
        value = value | bit;       
    }

    *data = value;   // Store the decoded integer value
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    printf("\n## DECODING OPERATION STARTED ##\n\n");

    //  Open stego image file
    if (open_image_file(decInfo) != e_success)
    {
        printf("Error: Failed to open stego image file\n");
        return e_failure;
    }
    printf("Image file opened successfully\n");
    printf("#* Done  *#\n\n");

    //  decode magic string length
    if (decode_magic_string_length(&decInfo-> magic_str_len, decInfo) != e_success)
    {
        printf(" Error: Failed to decode magic string length\n");
        return e_failure;
    }
    printf("Magic string length decode successfully\n");
    printf("Decoded Magic String length is: %d\n", decInfo-> magic_str_len);
    printf("#* Done  *#\n\n"); 

    // Decode magic string
    if (decode_magic_string(decInfo->magic_string, decInfo) != e_success)
    {
        printf("Error: Magic string decoding failed\n");
        return e_failure;
    }
    printf("Magic string decoded successfully\n");
    printf("#* Done  *#\n\n");


   
    char user_magic[MAX_MAGIC_LEN];
    printf("Enter expected magic string to verify: ");
    if (fgets(user_magic, sizeof(user_magic), stdin) == NULL)
    {
        printf("Failed to read user input\n");
        return e_failure;
    }
    // Remove newline if present
    user_magic[strcspn(user_magic, "\n")] = '\0';

    // Compare decoded and user entered magic strings
    if (strcmp(user_magic, decInfo->magic_string) != 0)
    {
        printf("Magic string mismatch! Decoded string and user input do not match.\n");
        return e_failure;
    }
    else
    {
        printf("Magic string verification successful.\n");
        printf("#* Done  *#\n\n");
    }

    //  Decode secret file extension size
    if (decode_secret_file_extn_size(&(decInfo->size_secret_file_extn), decInfo) != e_success)
    {
        printf("Error: Failed to decode secret file extension size\n");
        return e_failure;
    }
    printf("Secret file extension size decoded successfully\n");
    printf("Decoded extension size: %d\n", decInfo->size_secret_file_extn);
    printf("#* Done  *#\n\n");

   

    //Decode secret file extension
    if (decode_secret_file_extn(decInfo) != e_success)
    {
        printf("Error: Failed to decode secret file extension\n");
        return e_failure;
    }
    printf("Secret file extension decoded successfully\n");
    printf("Decoded extension: %s\n", decInfo->extn_secret_file);
    printf("#* Done  *#\n\n");
    

   // Concatenate output filename and decoded extension using strcat
    char final_fname[100];
    strcpy(final_fname, decInfo->secret_fname);
    strcat(final_fname, decInfo->extn_secret_file);

    // printf("Final output filename: %s\n", final_fname);


    // Open output file for writing using concatenated filename
    decInfo->fptr_secret = fopen(final_fname, "w");
    if (decInfo->fptr_secret == NULL)
    {
        perror("Error opening secret output file");
        return e_failure;
    }
    printf("Output file created: %s\n", final_fname);
    printf("#* Done  *#\n\n");


    // Decode secret file size
    if (decode_secret_file_size(decInfo) != e_success)
    {
        printf("Error: Failed to decode secret file size\n");
        return e_failure;
    }
    printf("Secret file size decoded successfully\n");
    printf("Decoded secret file size: %d bytes\n", decInfo->decode_size_secret_file);
    printf("#* Done  *#\n\n");


   // Decode secret file data
    if (decode_secret_file_data(decInfo) != e_success)
    {
        printf("Error: Failed to decode secret file data\n");
        return e_failure;
    }
    printf(" Secret file data decoded successfully...\n");
    printf("#* Done  *#\n\n");

    fclose(decInfo->fptr_secret);
    decInfo->fptr_secret = NULL;


    return e_success;

}