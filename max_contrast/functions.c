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

void restocking(char *path)
{
    Image           *img, *yz_img, *xz_img;
    int             f = 700;
    DIR             *FD;
    struct dirent   *in_file;
    struct dirent   **namelist;
    char            *imagePath;
    char            *name;
    
    if (NULL == (FD = opendir(FILE_PATH)))
    {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        closedir(FD);
        return;
    }
    c_histogram     *c = malloc(sizeof(*c));
    c_image_args    *a = malloc(sizeof(*a));
    c_threshold     *t = malloc(sizeof(*t));
    c_image         *e = malloc(sizeof(*e));
    c_zdimension    *z = malloc(sizeof(*z));

    c->first_his_val  = NULL;
    c->sec_his_val    = NULL;
    c->final_his_val  = NULL;
    t->max_threshold  = 0;
    
    while (f < TOTAL_WIDTH_RESTOCKING) {
        z->z_dec                        = scandir(FILE_PATH, &namelist, 0, alphasort);
        z->count_slide                  = 0;
        z->pixel_count_slide_start      = 1;
        z->pixel_count_slide_end        = 0;
        z->xz_count_slide               = 0;
        z->xz_pixel_count_slide_start   = 1;
        z->xz_pixel_count_slide_end     = 0;
        z->yz_pixel_map                 = NULL;
        z->xz_pixel_map                 = NULL;
        
        FD = opendir(FILE_PATH);
        printf("restocking..............\n");
        while ((in_file = readdir(FD)))
        {
            if (!strcmp (in_file->d_name, "."))
                continue;
            if (!strcmp (in_file->d_name, ".."))
                continue;
            //__APPLE__ ISSUE
            if (!strcmp (in_file->d_name, ".DS_Store"))
                continue;
            
            asprintf(&imagePath, "%s%s", path, in_file->d_name);
            img = get_image_from_path(imagePath);
            img = get_grayscale_image(img);

            z->yz_pixel_map = yz_reconstruction(img, z, f, 1);
            z->xz_pixel_map = xz_reconstruction(img, z, f, 1);
            
            DestroyImage(img);
            free(imagePath);
        }
        yz_img = yz_final_construct(z);
        yz_img = resize_image(yz_img, z->z_dec * 6, z->y_dec, BoxFilter, 0.5);
        yz_img = blur_image(yz_img, 2, 2);
        
        xz_img = xz_final_construct(z);
        xz_img = resize_image(xz_img, z->x_dec, z->z_dec * 6, BoxFilter, 0.5);
        xz_img = blur_image(xz_img, 2, 2);
        
        asprintf(&name, "%s%i", "yz_image_", f);
        dump_image(yz_img, FILE_CON_OUTPUT,name,"jpg");
        printf("restocked slide: %s\n", name);
        
        asprintf(&name, "%s%i", "xz_image_", f);
        dump_image(xz_img, FILE_CON_OUTPUT,name,"jpg");
        printf("restocked slide: %s\n", name);
        
        closedir(FD);
        z->yz_pixel_map = NULL;
        z->xz_pixel_map = NULL;
        f++;
    }
    free(a);
    free(t);
    free(c);
    free(e);
    free(z);
    return;
}

void image_processing(char *path)
{
    
    
    Image           *img, *org_img, *second_img, *th_img;
    int             *histogram;
    DIR             *FD;
    struct dirent   *in_file;
    int             numberOfFiles;
    struct dirent   **namelist;
    char            *imagePath;
    ExceptionInfo	exception;
    
    if (NULL == (FD = opendir(FILE_PATH)))
    {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        return;
    }
    
    c_histogram *c  = malloc(sizeof(*c));
    c_image_args *a = malloc(sizeof(*a));
    c_threshold *t  = malloc(sizeof(*t));
    c_image *e      = malloc(sizeof(*e));
    c_zdimension *z = malloc(sizeof(*z));
    
    c->first_his_val    = NULL;
    c->sec_his_val      = NULL;
    c->final_his_val    = NULL;
    t->max_threshold    = 128;
    
    char *save_first_path   = NULL;
    char *save_second_path  = NULL;
    char *save_third_path   = NULL;
    
    while ((in_file = readdir(FD)))
    {
        if (!strcmp (in_file->d_name, ".")){
            continue;
        }
        if (!strcmp (in_file->d_name, "..")){
            continue;
        }
        //APPLE Dir ISSUE
        if (!strcmp (in_file->d_name, ".DS_Store")){
            continue;
        }
        
        asprintf(&imagePath, "%s%s", path, in_file->d_name);
        img = get_image_from_path(imagePath);
        org_img = get_image_from_path(imagePath);
        img = get_grayscale_image(img);  // put image to grayscale
        
        printf("File name: %s\n", in_file->d_name);
        
        
        if (save_first_path == NULL) {
            save_first_path = imagePath;
        }
        else if (save_second_path == NULL)
            save_second_path = imagePath;
        else
            save_third_path = imagePath;
            
        if (save_second_path != NULL && save_first_path != NULL && save_third_path != NULL ) {
            second_img = get_image_from_path(save_second_path);
            second_img = get_grayscale_image(second_img);
            th_img = get_image_from_path(save_third_path);
            th_img = get_grayscale_image(th_img);
            
            img = get_avg_pixel(img, second_img, th_img);
            save_first_path = save_second_path;
            save_second_path = save_third_path;
            save_third_path = NULL;
        }
        
        //img = subtract_image(img, img);
        //img = contrast_image(img, 100); // increase contrast to 100
        //img = get_threshold_image(img, t); // thresholding image
        
        
        //img = crop_image(img,imagePath);
        //img = transparent_image(img);
        histogram = create_histogram(img, a);
        //look_through_histogram(histogram, c);
        img = dump_his_image(img, histogram, a);
        //histogram = create_histogram(img, a);
        //dump_histogram(histogram, FILE_HISTOGRAM_OUTPUT, c, in_file->d_name, "txt");
        
        //img = blur_image(img, 2,2);
        //img = get_blur_maked(img, e);
        //img = close_area(img);
        
        //img = get_area(img);
        //img = slant_line_detection(img);
        
        //img = crop_area_selected(img);
        //img = blur_image (img, 4, 4);
        
        //img = crop_masked_final_image(org_img, img);
        //img = maximum_contrast_image (img);
        //img = unsharp_mask_image(img,5,5); //deconvolution method using graphicmagic
        /*
        if (second_img != (Image *) NULL){
            DestroyImage(second_img);
        }
        if (th_img != (Image *) NULL){
            DestroyImage(th_img);
        }
        */
        dump_image(img, FILE_GRAY_OUTPUT, in_file->d_name , "jpeg");
        DestroyImage(org_img);
        //free(imagePath);
    }
    
    closedir(FD);
    free(a);
    free(t);
    free(c);
    free(e);
    free(z);
    return;
}

