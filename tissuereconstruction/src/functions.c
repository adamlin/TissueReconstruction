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

#include "max_contrast.h"

/*     check image processing is working for currection         */
void image_correction(char *path, char *output_path){
    Image           *final_img = NULL;
    DIR             *FD;
    struct dirent   *in_file;
    char            *imagePath;
    //char            *imageName;
    char            *name;
    int             count = 1;

    if (NULL == (FD = opendir(path)))
    {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        free(FD);
        return;
    }
    
    c_threshold *t   = malloc(sizeof(*t));
    
    t->max_threshold = 10;
    
    while ((in_file = readdir(FD)))
    {
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))
            continue;
        //APPLE Dir ISSUE
        if (!strcmp (in_file->d_name, ".DS_Store"))
            continue;
        
        asprintf(&imagePath, "%s%s", path, in_file->d_name);
        final_img = get_image_from_path(imagePath);
        
        printf("File name: %s\n", in_file->d_name);
        
        /*  put your image processing function/measure here !   */
        /*  crop image before reconstruction （IMAGE_WIDTH, IMAGE_HEIGHT, WIDTH_OFFSET, HEIGHT_OFFSET)   */
        //final_img = crop_image(final_img, imagePath, 1300, 1150, 1700, 3150); //brain_2
        //final_img = crop_image(final_img, imageName, 1727, 1575, 2450, 1850); //blockface 08.05.2013
        
        //final_img = rotate_image(final_img, 90);
        
        asprintf(&name, "%.4i%s", count, "_image");
        dump_image(final_img, output_path, name, "jpg");
        count ++;
        printf("Name Changed: %s\n", name);
        
        /*
        for (int i = 0 ; i < 6 ; i ++) {
            asprintf(&name, "%.4i%s", count, "_image");
            dump_multi_image(final_img, output_path, name, "jpg");
            count ++;
            printf("Name Changed: %s\n", name);
        }
        DestroyImage(final_img);
        */
    }
    closedir(FD);
    free(t);
    return;
}