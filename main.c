#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"


int main(int argc, char *argv[])
{
    int ret;
    EncodeInfo encInfo; 
    DecodeInfo decInfo; 
    if(argc == 1)
    {
        printf("$ Usage:\n");
        printf("   To encode : %s -e <input.bmp> <secret.txt|.c|.sh> [optional output.bmp]\n", argv[0]);
        printf("   To decode : %s -d <stego_image.bmp> [optional output_file]\n", argv[0]);
        return 0; 
        
    }
    if((argc == 1 || argc == 2) && (argv[1][1] == 'e'))
	{
		printf("Error please pass ./a.out -e beautiful.bmp secret.txt\n");//Checking whether option is encoding and printing error message
		return 0;
	}
	if((argc == 1 || argc == 2) && (argv[1][1] == 'd'))
	{
		printf("Error please pass ./a.out -d stego_img.bmp output.txt\n");//Checking whether option is decoding and printing error
		return 0;
	}
   

    if(argc > 2)
    {
        ret = check_operation_type(argv);
        if(ret == e_encode)
        {
             //Creating a variable
          printf("\nValidating encoding arguments\n\n");
        
            if(read_and_validate_encode_args(argv, &encInfo) == e_success)
            {
                printf("All Encode Arguments Validated  successfully\n");
                printf("<-- Done  -->\n\n");

                printf("Enter magic string : ");
                scanf("%49s", encInfo.magic_string);
                encInfo.magic_str_len = strlen(encInfo.magic_string);

                ret = do_encoding(&encInfo);
                if(ret == e_success)
                {
                  
                    printf("\n  ## Hidden data encoded into the image successfully!  ##\n");
                    printf("<---- ENCODING COMPLETED — FILE READY FOR DELIVERY ----->\n");
                    printf("_________________________________________________________\n");
                }
                
                else
                {
                    printf("ERROR: Failed to perform encoding\n");
                    return e_failure;
                }
            }
            else
		    {
			printf("ERROR: Validation of encoding arguments failed\n");
            return e_failure;
		    }
        }
        else if(ret  == e_decode)
        {
            
            if (argc >= 3 && argc <= 4)
            {
                

                if(read_and_validate_decode_args(argv, &decInfo) == e_success)
                {
                    
                    ret = do_decoding(&decInfo);
                    if(ret == e_success)
                    {
                        // close_decode_files(&decodeInfo);
                        printf("\n<-- Hidden data successfully extracted from the image!  -->\n");
                        printf("<---- DECODING COMPLETED — DATA RECOVERED SUCCESSFULLY ----->\n");
                        printf("<*_________________________________________________________*>\n");

                     
                    }

                    
                    else
                    {
                        //close_decode_files(&decodeInfo);
                        printf("ERROR: Failed to perform decoding\n");
                        return e_failure;
                    }
               
                }
                else
                {
                    printf("ERROR: Validation of decoding arguments failed\n");
                    return e_failure;  
                }
            }
            else
            {
                printf("ERROR: Invalid number of arguments for decoding\n");
                printf("📌 Usage: %s -d <stego_image.bmp> [optional output.txt]\n", argv[0]);
            }
       }
      else
       {
            printf("❌ ERROR: Unsupported operation\n");
            printf("➡️  Use -e for encoding or -d for decoding\n");
            printf("📌 Usage:\n");
            printf("   To encode : %s -e <input.bmp> <secret.txt|.c|.sh> [optional output.bmp]\n", argv[0]);
            printf("   To decode : %s -d <stego_image.bmp> [optional output_file]\n", argv[0]);
        }

       
    }
    return 0;
   

}

