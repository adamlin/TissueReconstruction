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

void restacking(char *path, int start_slide, int end_slide, char *output_path)
{
    Image           *img, *yz_img, *xz_img; // *angel_img;
    int             f = start_slide;
    DIR             *FD;
    struct dirent   *in_file;
    struct dirent   **namelist;
    char            *imagePath, *x_outputPath, *y_outputPath, *z_outputPath;
    char            *name;
    int             count = 1;
    
    if (NULL == (FD = opendir(path)))
    {
        fprintf(stderr, "Error : Failed to open SOURCE directory - %s\n", strerror(errno));
        free(FD);
        return;
    }
    
    if (NULL == (FD = opendir(output_path)))
    {
        fprintf(stderr, "Error : Failed to open OUTPUT directory - %s\n", strerror(errno));
        free(FD);
        return;
    }
    
    create_three_dimension_folder(output_path);
    asprintf(&x_outputPath, "%sx", output_path);
    asprintf(&y_outputPath, "%sy", output_path);
    asprintf(&z_outputPath, "%sz", output_path);
    
    
    /*
     ** dupicate z plane images to meet slides requirement!
     ** important! need to compare how many slides in order to math pixels (z->z->dec)
    */
    
    FD = opendir(path);
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
        img = get_image_from_path(imagePath);
        
        for (int i = 0 ; i < 4 ; i ++) {
            asprintf(&name, "%.4i%s", count, "_xy_image");
            dump_multi_image(img, z_outputPath, name, "jpg");
            count ++;
            printf("New Images: %s\n", name);
        }
        DestroyImage(img);
    }
    closedir(FD);
    
    
    /*
     ** restacking image start from here!
     ** f is the start point of dimension and end_slide is the end point of dimension
    */
    
    printf("..... start restacking yz and xy plane images .....\n");
    
    c_histogram     *c = malloc(sizeof(*c));
    c_image_args    *a = malloc(sizeof(*a));
    c_threshold     *t = malloc(sizeof(*t));
    c_image         *e = malloc(sizeof(*e));
    c_zdimension    *z = malloc(sizeof(*z));
    
    c->first_his_val  = NULL;
    c->sec_his_val    = NULL;
    c->final_his_val  = NULL;
    t->max_threshold  = 128;
    
    while (f < end_slide) {
        z->z_dec                            = scandir(path, &namelist, 0, alphasort);
        z->count_slide                      = 0;
        z->pixel_count_slide_start          = 1;
        z->pixel_count_slide_end            = 0;
        z->xz_count_slide                   = 0;
        z->xz_pixel_count_slide_start       = 1;
        z->xz_pixel_count_slide_end         = 0;
        
        /*  angel restacking     */
        /*
        z->angel_count_slide                = 0;
        z->angel_pixel_count_slide_end      = 0;
        z->angel_pixel_count_slide_start    = 1;
        */
        
        z->yz_pixel_map                     = NULL;
        z->xz_pixel_map                     = NULL;
        //z->angel_pixel_map                  = NULL;
        
        FD = opendir(path);
        printf("restocking..............\n");
        while ((in_file = readdir(FD)))
        {
            if (!strcmp (in_file->d_name, ".")){
                z->z_dec -= 1;
                continue;
            }
            if (!strcmp (in_file->d_name, "..")){
                z->z_dec -= 1;
                continue;
            }
            //__APPLE__ ISSUE
            if (!strcmp (in_file->d_name, ".DS_Store")){
                z->z_dec -= 1;
                continue;
            }
            
            asprintf(&imagePath, "%s%s", path, in_file->d_name);
            img = get_image_from_path(imagePath);
            img = get_grayscale_image(img);
            
            z->yz_pixel_map     = yz_reconstruction(img, z, f);
            z->xz_pixel_map     = xz_reconstruction(img, z, f);
            //z->angel_pixel_map  = angel_reconstruction(img, z, f, 45);

            DestroyImage(img);
            free(imagePath);
        }
        yz_img = yz_final_construct(z);
        yz_img = resize_image(yz_img, z->z_dec * 4, z->y_dec, BoxFilter, 0.5);
        yz_img = reduce_noice(yz_img);
        
        xz_img = xz_final_construct(z);
        xz_img = resize_image(xz_img, z->x_dec, z->z_dec * 4, BoxFilter, 0.5);
        xz_img = reduce_noice(xz_img);
        
        /*  angel restacking     */
        /*
         angel_img = angel_final_construct(z);
         angel_img = resize_image(angel_img, z->ax_dec, z->z_dec * 6, BoxFilter, 0.5);
         angel_img = reduce_noice(angel_img);
         */
        
        asprintf(&name, "%.4i%s", f, "_yz_image");
        dump_image(yz_img, x_outputPath,name,"jpg");
        printf("restocked slide: %s\n", name);
        
        asprintf(&name, "%.4i%s", f, "_xz_image");
        dump_image(xz_img, y_outputPath,name,"jpg");
        printf("restocked slide: %s\n", name);
        
        /*  angel restacking     */
        /*
         asprintf(&name, "%s%i", "angel_image_r", f);
         dump_image(angel_img, output_path,name,"jpg");
         printf("restocked slide: %s\n", name);
        */
        
        closedir(FD);
        z->yz_pixel_map     = NULL;
        z->xz_pixel_map     = NULL;
        
        //z->angel_pixel_map  = NULL;
        
        f++;
    }
    
    free(a);
    free(t);
    free(c);
    free(e);
    free(z);
    return;
}
