/*
 **
 ** This file is part of TissueReconstruction.
 **
 ** TissueReconstruction is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** TissueReconstruction is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with BananaCam.  If not, see <http://www.gnu.org/licenses/>.
 **
 */

/*
 FILE: edgeLap.c 
 AUTH: P.Oh
 DESC: 5x5 Laplace mask for edge detection
 DATE: 05/02/02 23:30
 REFS: edgedeta.c
 */

#include "max_contrast.h"

/*
int bt_threshold(int histogram) {
    i_m = (int)((i_s + i_e) / 2.0f); // center of the weighing scale I_m
    w_l = get_weight(i_s, i_m + 1, histogram); // weight on the left W_l
    w_r = get_weight(i_m + 1, i_e + 1, histogram); // weight on the right W_r
    while (i_s <= i_e) {
        if (w_r > w_l) { // right side is heavier
            w_r -= histogram[i_e--];
            if (((i_s + i_e) / 2) < i_m) {
                w_r += histogram[i_m];
                w_l -= histogram[i_m--];
            }
        } else if (w_l >= w_r) { // left side is heavier
            w_l -= histogram[i_s++];
            if (((i_s + i_e) / 2) > i_m) {
                w_l += histogram[i_m + 1];
                w_r -= histogram[i_m + 1];
                i_m++;
            }
        }
    }
    return i_m;
}
*/

long getImageInfo(FILE* inputFile, long offset, int numberOfChars)
{
    unsigned char			*ptrC;
    long				value = 0L;
    unsigned char			dummy;
    int				i;
    
    dummy = '0';
    ptrC = &dummy;
    
    fseek(inputFile, offset, SEEK_SET);
    
    for(i=1; i<=numberOfChars; i++)
    {
        fread(ptrC, sizeof(char), 1, inputFile);
        /* calculate value based on adding bytes */
        value = (long)(value + (*ptrC)*(pow(256, (i-1))));
    }
    return(value);
}

void copyImageInfo(FILE* inputFile, FILE* outputFile)
{
    unsigned char		*ptrC;
    unsigned char		dummy;
    int				i;
    
    dummy = '0';
    ptrC = &dummy;
    
    fseek(inputFile, 0L, SEEK_SET);
    fseek(outputFile, 0L, SEEK_SET);
    
    for(i=0; i<=50; i++)
    {
        fread(ptrC, sizeof(char), 1, inputFile);
        fwrite(ptrC, sizeof(char), 1, outputFile);
    }
}

void copyColorTable(FILE* inputFile, FILE* outputFile, int nColors)
{
    unsigned char		*ptrC;
    unsigned char		dummy;
    int				i;
    
    dummy = '0';
    ptrC = &dummy;
    
    fseek(inputFile, 54L, SEEK_SET);
    fseek(outputFile, 54L, SEEK_SET);
    
    for(i=0; i<=(4*nColors); i++)  /* there are (4*nColors) bytesin color table */
    {
        fread(ptrC, sizeof(char), 1, inputFile);
        fwrite(ptrC, sizeof(char), 1, outputFile);
    }
}


Image *edgelap(Image *img, char *path)
{
    FILE			*bmpInput, *bmpOutput;
    c_image          originalImage;
    c_image          edgeImage;
    unsigned int	X, Y;
    int             I, J;
    long			SUM;
    int             nColors;
    unsigned long	vectorSize;
    unsigned long	fileSize;
    int             MASK[5][5];
    unsigned char 	*pChar, someChar;
    unsigned int	row, col;
    char            *string_img = malloc(img->rows * img->columns * sizeof(*string_img));
    ExceptionInfo	exception;
    
    GetExceptionInfo(&exception);
    
    someChar = '0'; pChar = &someChar;
    
    /* 5x5 Laplace mask.  Ref: Myler Handbook p. 135 */
    MASK[0][0] = -1; MASK[0][1] = -1; MASK[0][2] = -1; MASK[0][3] = -1; MASK[0][4] = -1;
    MASK[1][0] = -1; MASK[1][1] = -1; MASK[1][2] = -1; MASK[1][3] = -1; MASK[1][4] = -1;
    MASK[2][0] = -1; MASK[2][1] = -1; MASK[2][2] = 24; MASK[2][3] = -1; MASK[2][4] = -1;
    MASK[3][0] = -1; MASK[3][1] = -1; MASK[3][2] = -1; MASK[3][3] = -1; MASK[3][4] = -1;
    MASK[4][0] = -1; MASK[4][1] = -1; MASK[4][2] = -1; MASK[4][3] = -1; MASK[4][4] = -1;
    
    printf("%s\n", path);
    /* open files for reading and writing to */
    bmpInput = fopen(path, "rb");
    bmpOutput = fopen(FILE_OUTPUT, "wb");
    
    /* start pointer at beginning of file */
    fseek(bmpInput, 0L, SEEK_END);
    
    /* retrieve and print filesize and number of cols and rows */
    fileSize = getImageInfo(bmpInput, 2, 4);
    originalImage.cols = (int)img->columns;
    originalImage.rows = (int)img->rows;
    edgeImage.rows = originalImage.rows;
    edgeImage.cols = originalImage.cols;
    
    printf("Width: %d\n", originalImage.cols);
    printf("Height: %d\n", originalImage.rows);
    printf("File size: %lu\n", fileSize);
    
    /* retrieve and print Number of colors */
    nColors = (int)getImageInfo(bmpInput, 46, 4);
    printf("nColors: %d\n", nColors);
    
    vectorSize = fileSize - (14+40+4*nColors);
    printf("vectorSize: %lu\n", vectorSize);
    edgeImage.data = malloc(vectorSize*sizeof(unsigned char));
    if(edgeImage.data == NULL) {
        exit(0);
    }
    printf("%lu bytes malloc'ed for edgeImage.data\n", vectorSize);
    
    originalImage.data = malloc(vectorSize*sizeof(unsigned char));
    if(originalImage.data == NULL) {
        exit(0);
    }
    printf("%lu bytes malloc'ed for originalImage.data\n", vectorSize);
    
    //copyImageInfo(bmpInput, bmpOutput);
    //copyColorTable(bmpInput, bmpOutput, nColors);
    fseek(bmpInput, (14+40+4*nColors), SEEK_SET);
    fseek(bmpOutput, (14+40+4*nColors), SEEK_SET);
    
    /* Read input.jpg and store it's raster data into originalImage.data */
    for(row=0; row<=originalImage.rows-1; row++) {
        for(col=0; col<=originalImage.cols-1; col++) {
            fread(pChar, sizeof(char), 1, bmpInput);
            *(originalImage.data + row*originalImage.cols + col) = *pChar;
        }
    }
    
    for(Y=0; Y<=(originalImage.rows-1); Y++)  {
        for(X=0; X<=(originalImage.cols-1); X++)  {
            SUM = 0;
            
            /* image boundaries */
            if(Y==0 || Y==1 || Y==originalImage.rows-2 || Y==originalImage.rows-1)
                SUM = 0;
            else if(X==0 || X==1 || X==originalImage.cols-2 || X==originalImage.cols-1)
                SUM = 0;
            
            /* Convolution starts here */
            else   {
                for(I=-2; I<=2; I++)  {
                    for(J=-2; J<=2; J++)  {
                        SUM = SUM + (int)( (*(originalImage.data + X + I + (Y + J)*originalImage.cols)) * MASK[I+2][J+2]);    
                    }
                }
            }
            if(SUM>255)  SUM=255;
            if(SUM<0)    SUM=0;
            
            
            *(edgeImage.data + X + Y*originalImage.cols) = 255 - (unsigned char)(SUM);  /* make edges black and background white */
            fwrite( (edgeImage.data + X + Y*originalImage.cols), sizeof(char), 1, bmpOutput);
        }
    }
    
    fclose(bmpInput);
    fclose(bmpOutput);
    free(edgeImage.data);   
    free(originalImage.data);
    free(string_img);
    return 0;
}