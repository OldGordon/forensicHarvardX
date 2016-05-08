/****************************************************************************
 * resize.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 *Toni Juliá - GrinuX
 *
 * Resizes a BMP by factor n.
 ***************************************************************************/
       
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"

#define INFILE_MAXSIZE 4294967296

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: resize n(integer between 1 to 100) infile outfile\n");
        return 1;
    }
    
    // n from string to integer
    int n = atoi(argv[1]);
    if ((n < 1) || (n > 100))
    {
        printf("Factor for resizing have to be between 1 and 100" );
        return 6;
    }
    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    
    
    
    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    if ((n * bf.bfSize) > INFILE_MAXSIZE)
    {  
        fclose(outptr);
        fclose(inptr); 
        printf("%s too large.\n", infile);
        return 5;
    }
           
    
    //I keep the original dimensions to use them later
    int old_width = bi.biWidth;
    int old_height = bi.biHeight;
    
    
    // write outfile's BITMAPINFOHEADER writing new values for new header
    bi.biWidth = bi.biWidth * n ;
    bi.biHeight = bi.biHeight * n ;
    
    // Saving old padding and getting the new padding
    int padding =  (4 - (old_width * sizeof(RGBTRIPLE)) % 4) % 4;
    int new_padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE))% 4) % 4 ;
    
    bi.biSizeImage = abs(bi.biHeight) * ((bi.biWidth * sizeof (RGBTRIPLE)) + new_padding); 
    bf.bfSize = bi.biSizeImage + sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER);
   
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
  

    // determine padding for scanlines
    
        
    RGBTRIPLE line_width[bi.biWidth];
    
    // iterate over infile's scanlines
    for (int i = 0; i < abs(old_height);  i++)
    {
          int punt = 0 ;
         
         // iterate over pixels in scanline 
         for( int pix = 0; pix < old_width ; pix++)
         {
              // temporary storage
              RGBTRIPLE triple;
                
              // read RGB triple from infile
              fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
              
               for(int k = 0; k < n ; k++) 
               {     
                   line_width[punt].rgbtBlue = triple.rgbtBlue;
                   line_width[punt].rgbtGreen = triple.rgbtGreen;
                   line_width[punt].rgbtRed = triple.rgbtRed;
                   punt = punt + 1 ;
               } 
          }
       //----------------------------------------------                       
          for (int x = 0; x < n; x++)
          {
        
               //again we iterate over scanlines
               for (punt = 0; punt < bi.biWidth; punt++)
               {
                    // write every pixel to the new file
                    fwrite(&line_width[punt], sizeof (RGBTRIPLE), 1, outptr);
               }
 
                 // writing the padding to  the new file
                 for (int y = 0; y < new_padding; y++)
                     fputc(0x00, outptr);
          }                                
            
          // skip over padding, if any
          fseek(inptr, padding, SEEK_CUR);  
      } 
          
      
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
