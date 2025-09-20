#include <stdio.h>
#include "encode.h"
#include <string.h>
#include "types.h"


OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    { 
        return e_unsupported;
    }
}


Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char arr[5];
    if(strstr(argv[2], ".bmp") != NULL)
    {
        printf("Source image file validated : %s\n", argv[2]);
         printf("#* Done  *#\n\n");
        encInfo -> src_image_fname = argv[2];
    }
    else
    {
        printf("Source image file validated : beautiful.bmp is not prsent \n");
        return e_failure;
    }

    if(argv[3] != 0)
    {
        if(strstr(argv[3], ".txt") || strstr(argv[3], ".csv") != NULL )
        {
            printf("Secret file validated : %s12\n", argv[3]);
            printf("#* Done  *#\n\n");
            encInfo -> secret_fname = argv[3];
           

        }
        else
        {
            printf("Secret file validated : secret.txt not present\n");
  
            return e_failure;
        }
    }
    else
    {
        printf("Please pass screte file\n");
        return e_failure;

    }
    if(argv[4] != 0)
	{
		strcpy(arr,strstr(argv[4],"."));
		if(strcmp(arr,".bmp") == 0)
		{
			encInfo->stego_image_fname = argv[4];  //Copying output file name if passed
		}
		else
		{
			printf("Extension should be .bmp\n");  //Prompt user to give extension
			return e_failure;
		}
	}
	else
	{
		encInfo->stego_image_fname = "stego_img.bmp";  //Giving default name 
	}
    return e_success;
}


Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    // Get length of magic string
    int magic_str_len = strlen(encInfo->magic_string);

    // Get length of secret file extension
    int extn_len = strlen(encInfo->extn_secret_file);
   
   

    // Calculate total bytes to encode (header + data)
    
        long int no_bytes_to_encode =  (sizeof(int) +  magic_str_len +  sizeof(int) +  extn_len + sizeof(int) +    encInfo->size_secret_file)  * 8;        
    

    // Check capacity (exclude BMP header = 54 bytes)
    if ((encInfo->image_capacity - 54) >= no_bytes_to_encode)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}


// uint get_file_size(	FILE *fptr)
// {
// 	fseek(fptr,0,SEEK_END);           //Getting file size using fseek function
// 	int len = ftell(fptr);
// 	return len;
// }

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("Copying BMP Header from source to destination...\n");

    // Ensure reading from start of the source image
    rewind(fptr_src_image);

    char str[54]; // BMP header is 54 bytes

    if (fread(str, 54, 1, fptr_src_image) != 1)
    {
        perror("Error reading BMP header");
        return e_failure;
    }

    // Write BMP header to destination (stego) image
    if (fwrite(str, 54, 1, fptr_dest_image) != 1)
    {
        perror("Error writing BMP header");
        return e_failure;
    }

    return e_success;

}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int n=7;
    for (int i = 0; i < 8; i++) // Loop over 7 bits
    {
        int mask = 1 << n;
        int bit = data & mask; 
        image_buffer[i] = image_buffer[i] & 0xFE;
        bit = bit >> n;
        image_buffer[i] = image_buffer[i] | bit;
        n--;
    }

    return e_success;
}

Status encode_size_to_lsb(int data, char *image_buffer)
{
    int n=31;
    for (int i = 0; i < 32; i++) // Loop over 32 bits
    {
        int mask = 1 << n;
        int bit = data & mask; 
        image_buffer[i] = image_buffer[i] & 0xFE;
        bit = bit >> n;
        image_buffer[i] = image_buffer[i] | bit;
        n--;
    }

    return e_success;
}


Status encode_magic_string_length(int magic_str_len, EncodeInfo *encInfo)
{
    printf("Encoding Magic String Length into image...\n");

    char buffer[32]; // 32 bytes buffer 

    // Read 32 bytes from source image where magic string length will be encoded
    if (fread(buffer, 32, 1, encInfo->fptr_src_image) != 1)
    {
        printf("Error reading from source image while encoding magic string length");
        return e_failure;
    }

    //call encode_size_to_lsb function to encode 
    if (encode_size_to_lsb(magic_str_len, buffer) != e_success)
    {
        printf("Failed to encode magic string length into image buffer\n");
        return e_failure;
    }

    // Write the modified 32 bytes back into stego image file
    if (fwrite(buffer, 32, 1, encInfo->fptr_stego_image) != 1)
    {
        printf("Error writing encoded magic string length to stego image");
        return e_failure;
    }

    return e_success;
}


Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
     printf("Encoding Magic String into image...\n");

     for (int i = 0; i < strlen(magic_string); i++)
     {
        char buffer[8];
        if (fread(buffer, 8, 1, encInfo->fptr_src_image) != 1)
        {
            printf("Error: Failed to read 8 bytes from source image");
            return e_failure;
        }
        if(encode_byte_to_lsb(magic_string[i], buffer) != e_success) 
        {
            printf("Error: Failed to encode byte");
            return e_failure;
        }

        if (fwrite(buffer, 8, 1, encInfo->fptr_stego_image) != 1)
        {
            printf("Error: Failed to write encoded bytes to stego image");
            return e_failure;
        }
        
     }
    return e_success;

    
}

Status encode_secret_file_extn_size(int len, EncodeInfo *encInfo)
{
    char buffer[32];

    printf("Encoding secret file extension size into image...\n");

    // Read 32 bytes from the source image
    if (fread(buffer, 32, 1, encInfo->fptr_src_image) != 1)
    {
        printf("Error reading from source image during file size encoding");
        return e_failure;
    }

    // Encode the 32-bit file size into LSBs
    if (encode_size_to_lsb(len, buffer) != e_success)
    {
        printf("Failed to encode file size into image buffer\n");
        return e_failure;
    }

    // Write 32 bytes to the stego image
    if (fwrite(buffer, 32, 1, encInfo->fptr_stego_image) != 1)
    {
        printf("Error writing encoded file size to stego image");
        return e_failure;
    }

    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("Encoding secret file extension into image...\n");

    for (int i = 0; i < strlen(file_extn); i++)
     {
        char buffer[8];
        if (fread(buffer, 8, 1, encInfo->fptr_src_image) != 1)
        {
            printf("Error: Failed to read 8 bytes from source image");
            return e_failure;
        }
        if(encode_byte_to_lsb(file_extn[i], buffer) != e_success) 
        {
            printf("Error: Failed to encode byte");
            return e_failure;
        }

        if (fwrite(buffer, 8, 1, encInfo->fptr_stego_image) != 1)
        {
            printf("Error: Failed to write encoded bytes to stego image");
            return e_failure;
        }
        
     }

    return e_success;
}


Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];

      printf("Encoding secret file size into image...\n");

    // Read 32 bytes from the source image
    if (fread(buffer, 32, 1, encInfo->fptr_src_image) != 1)
    {
        printf("Error reading from source image while encoding secret file size");
        return e_failure;
    }

    // Encode the 32-bit file size into LSBs
    if (encode_size_to_lsb(file_size, buffer) != e_success)
    {
        printf("Failed to encode secret file size into image buffer\n");
        return e_failure;
    }

    // Write 32 bytes to the stego image
   if (fwrite(buffer, 32, 1, encInfo->fptr_stego_image) != 1)
    {
        printf("Error writing encoded secret file size to stego image");
        return e_failure;
    }

    return e_success;
}


Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("Encoding secret file data into image...\n");

    char ch;
    while (fread(&ch, 1, 1, encInfo->fptr_secret) == 1)
    {
        char buffer[8];
        if (fread(buffer, 8, 1, encInfo->fptr_src_image) != 1)
        {
            printf("Error reading from secret file");
            return e_failure;
        }
        if(encode_byte_to_lsb(ch, buffer) != e_success) 
        {
            printf("Error: Failed to encode byte");
            return e_failure;
        }
        if (fwrite(buffer, 8, 1, encInfo->fptr_stego_image) != 1)
        {
            printf("Error writing encoded secret file  to stego image");
            return e_failure;
        }

    }
    return e_success;

   
}


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("Copying remaining image data to output image...\n");

    char ch;
   
    while (fread(&ch, 1, 1,fptr_src) == 1) // fread returns number of items read
    {
        if (fwrite(&ch, 1, 1, fptr_dest) != 1)
        {
            printf("Error: Failed to write remaining byte to stego image");
            return e_failure;
        }
    }

    return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{

   printf("\n\n## ENCODING OPERATION STARTED ##\n\n");

   // Open source and destination files
   if(open_files(encInfo) != e_success) // to open files
   {
        printf("Error: Failed to open files\n");
        return e_failure;
   }
    printf("All Files opened successfully\n");
    printf("#* Done  *#\n\n");

    //  Check image capacity
    int ret =  check_capacity(encInfo);
    if(ret != e_success)
    {
        printf("Error: %s does not have enough capacity to store %s\n", encInfo->src_image_fname, encInfo->secret_fname);
        return e_failure;
    }
    
    printf("Sufficient Capacity in the image for Encoding\n\n");  //Printing success message
    
   // Copy BMP header

   if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
   {
        printf("Error: Failed to copy BMP header\n");
        return e_failure;
   }

    printf("BMP header copied successfully\n");
    printf("#* Done  *#\n\n");

    //  Encode magic string length
    if (encode_magic_string_length(encInfo-> magic_str_len, encInfo) != e_success)
    {
        printf(" Error: Failed to encode magic string length\n");
        return e_failure;
    }
    printf("Magic string length encoded successfully\n");
    printf("#* Done  *#\n\n"); 


     //  Encode magic string
    if (encode_magic_string(encInfo->magic_string, encInfo) != e_success)
    {
        printf("Error: Failed to encode magic string\n");
        return e_failure;
    }
    printf("Magic string encoded successfully\n");
    printf("#* Done  *#\n\n");

    // getting file extension length
        char *ret_ptr = strstr(encInfo->secret_fname, ".");
        if (ret_ptr == NULL)
        {
            printf("ERROR: Secret file has no extension\n");
            return e_failure;
        }
        int extn_len = strlen(ret_ptr);

    //  Encode secret file extension size
    if (encode_secret_file_extn_size(extn_len, encInfo) != e_success)
    {
        printf("Error: Failed to encode secret file extension size\n");
        return e_failure;
    }
    printf("Secret file extension size encoded successfully\n");
    printf("#* Done  *#\n\n");
   

    // Encode secret file extension
    if (encode_secret_file_extn(ret_ptr, encInfo) != e_success)
    {
        printf("Error: Failed to encode secret file extension\n");
        return e_failure;
    }
    printf("Secret file extension encoded successfully\n");
    printf("#* Done  *#\n\n");

    // 10. Calculate secret file size
    fseek(encInfo->fptr_secret, 0, SEEK_END);
    long file_size = ftell(encInfo->fptr_secret);
    rewind(encInfo->fptr_secret);


    // Encode secret file size
    if (encode_secret_file_size(file_size, encInfo) != e_success)
    {
        printf("Error: Failed to encode secret file size\n");
        return e_failure;
    }
    printf("Secret file size encoded successfully\n");
    printf("#* Done  *#\n\n");



    // Encode secret file data
    if (encode_secret_file_data(encInfo) != e_success)
    {
        printf("Error: Failed to encode secret file data\n");
        return e_failure;
    }
    printf("Secret file data encoded successfully\n");
    printf("#* Done  *#\n\n");



    //  Copy remaining image data
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        printf("Error: Failed to copy remaining image data\n");
        return e_failure;
    }
    printf("Remaining image data copied successfully\n");
    printf("#* Done  *#\n\n");


    return e_success;

 
   
}


