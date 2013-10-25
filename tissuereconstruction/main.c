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
 ** along with TissueReconstruction.  If not, see <http://www.gnu.org/licenses/>.
 **
 */

/*
 ** -- need following libraries to make this reconstruction works --
 ** "graphicmagick"
 ** "ITK" (Insight Segmentation and Registration Toolkit)
 ** "Dcraw"
 */

#include "max_contrast.h"
#include "raw_files.h"

int		main(int argc, char **argv)
{
    InitializeMagick("/");
    #ifndef DEBUG
    if (argc != 4) {
        printf("\n************************************************************************\n");
        printf("Please provide paramter and TWO file locations for image reconstruction!\n");
        printf("For example: ./TissueReconstruction restacking file_name file_output    \n");
        printf("************************************************************************\n");
        //return 0;
    }
    else{
        char *f_imagePath = strdup(argv[2]);
        char *s_imagePath = strdup(argv[3]);
        char *f_ptr, *s_ptr;
        int  ch = '/';
        
        f_ptr = strrchr(f_imagePath, ch);
        s_ptr = strrchr(s_imagePath, ch);
        
        char *cht = "/";
        
        if (f_ptr != cht || s_ptr != cht ) {
            if(f_ptr != cht)
                asprintf(&f_imagePath, "%s/", f_imagePath);
            if(s_ptr != cht){
                struct stat st = {0};
                asprintf(&s_imagePath, "%s/Restacking/", s_imagePath);
                
                if (stat(s_imagePath, &st) == -1)
                    mkdir(s_imagePath, 0700);
            }
        }

        if (strcmp(argv[1], "processing") == 0)
            image_processing(f_imagePath, s_imagePath);
        else if (strcmp(argv[1], "restacking") == 0)
            restacking(f_imagePath, 0, 1000, s_imagePath);
        else if (strcmp(argv[1], "raw") == 0)
            dumpStackIntoRawFile(f_imagePath, s_imagePath);
        
        free(f_imagePath);
        free(s_imagePath);
    }
    

    #endif
    
    #ifdef DEBUG

        /*
         ** image processing before restacking - max contrast, otsu-throsholding, cross correlation, image deconvolution
         */
        
        //image_processing(FILE_PATH, FILE_MAX_OUTPUT);
        //image_correction("/Users/adam/Documents/blockface_allbrains/3DOutput/y/", FILE_MAX_OUTPUT);
        //image_processing("/Users/adam/Desktop/image/JPG_CROP/", "/Users/adam/Desktop/image/JPG_CROP_GRAY");
        //check_raw(FILE_OUT_RAW);  
        
        /*
         ** restack images - Parameters:(source dir, start slide, end slide, output dir)
         */
    
        //restacking(FILE_PATH,0, 1000, FILE_MAX_OUTPUT);
        
        /*
        ** final - dump three planes to RAW file for Tissuestack used!
        */

        //dumpStackIntoRawFile("/Users/adam/Desktop/image/Reconstruction/", FILE_OUT_RAW);
    
     #endif
    DestroyMagick();
    return (0);
}