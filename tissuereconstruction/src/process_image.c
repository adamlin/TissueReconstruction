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

void image_processing(char *path, char *output_path)
{
    Image           *final_img = NULL;
    Image           *first_img = NULL, *second_img = NULL, *third_img = NULL;
    //int             *histogram;
    DIR             *FD;
    struct dirent   *in_file;
    
    char            *imagePath;
    char            *save_first_path   = NULL;
    char            *save_second_path  = NULL;
    char            *save_third_path   = NULL;
    double          *ssum;
    
    if (NULL == (FD = opendir(path)))
    {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        free(FD);
        return;
    }
    
    c_histogram         *c  = malloc(sizeof(*c));
    c_image_args        *a  = malloc(sizeof(*a));
    c_threshold         *t  = malloc(sizeof(*t));
    c_image             *e  = malloc(sizeof(*e));
    c_zdimension        *z  = malloc(sizeof(*z));
    c_cross_correlation *b  = malloc(sizeof(*b));
    
    c->first_his_val    = NULL;
    c->sec_his_val      = NULL;
    c->final_his_val    = NULL;
    t->max_threshold    = 128;
    
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
        final_img   = get_image_from_path(imagePath);
        //org_img     = get_image_from_path(imagePath);
        final_img   = get_grayscale_image(final_img);         // put image to grayscale
        
        printf("File name: %s\n", in_file->d_name);
        
        if (save_first_path == NULL)
            save_first_path = imagePath;
        else if (save_second_path == NULL)
            save_second_path = imagePath;
        else
            save_third_path = imagePath;
        
        if (save_second_path != NULL && save_first_path != NULL && save_third_path != NULL) {
            first_img = get_image_from_path(save_first_path);
            second_img = get_image_from_path(save_second_path);
            third_img = get_image_from_path(save_third_path);
            
            final_img = get_avg_pixel(final_img, first_img, second_img, third_img);
            //DestroyImage(first_img);
            ssum = cross_correlation(second_img, third_img, b); // cross correlation  - TODO: move pixel to right position.
            
            save_first_path = save_second_path;
            save_second_path = save_third_path;
            save_third_path = NULL;
        }
        
        //img = crop_image(img,imagePath);
        //img = transparent_image(img);
        
        //histogram = create_histogram(img, a);
        //look_through_histogram(histogram, c);
        //img = dump_his_image(img, histogram, a);
        //histogram = create_histogram(img, a);
        //dump_histogram(histogram, FILE_HISTOGRAM_OUTPUT, c, in_file->d_name, "txt");
        
        
         //final_img = subtract_image(final_img, final_img);   // subtraction 2 images
         //final_img = contrast_image(final_img, 100);         // increase contrast to 100
         //final_img = get_threshold_image(final_img, t);      // thresholding image
         //final_img = blur_image(final_img, 2,2);             // blur images by 2*2
         //final_img = get_blur_maked(final_img, e);           // make all blur (black) to join together
         //final_img = close_area(final_img);                  // draw close areas
        
        
        /*
         img = get_area(img);
         img = slant_line_detection(img);
         
         img = crop_area_selected(img);
         img = blur_image (img, 4, 4);
         */
        
        //final_img = crop_masked_final_image(org_img, final_img);
        //final_img = maximum_contrast_image (final_img);
        //final_img = unsharp_mask_image(final_img, 3, 3); //deconvolution method using graphicmagic
        //final_img = reduce_noice(final_img);
        
        dump_image(final_img, output_path, in_file->d_name , "jpeg");
        
        //DestroyImage(org_img);
        //free(imagePath);
    }
    
    closedir(FD);
    free(a);
    free(t);
    free(c);
    free(e);
    free(z);
    free(b);
    return;
}
