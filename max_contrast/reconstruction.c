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

unsigned char *yz_reconstruction(Image *img, c_zdimension *z, int i_px, int copy_width){
    unsigned int    y = 0;
    int             new_width = z->z_dec;
    int             width = (int)img->columns;
    int             height = (int)img->rows;
    PixelPacket     *px;
    
    if (z->yz_pixel_map == 0){
        z->y_dec = (int)img->rows;
        z->x_dec = (int)img->columns;
        z->z_dec = (z->z_dec * copy_width);
        z->yz_pixel_map         = malloc(z->y_dec *  z->z_dec * sizeof(*z->yz_pixel_map));
    }
    px = GetImagePixels(img, 0, 0, img->columns, img->rows);
    
    z->count_slide ++ ; // count how many slide in brain tissue
    z->pixel_count_slide_end = z->count_slide * copy_width;
    
    while (y < height) {
        for (int i = z->pixel_count_slide_start; i<= z->pixel_count_slide_end; i++) {
            z->yz_pixel_map[(new_width * y) + i] = BLACK;
        }
        y++;
    }
    y = 0;
    
    while (y < height) {
        for (int i = z->pixel_count_slide_start; i<= z->pixel_count_slide_end; i++) {
            z->yz_pixel_map[(new_width * y) + i] = (char)px[(width * y) + i_px].green;
        }
        y++;
    }
    z->pixel_count_slide_start = z->pixel_count_slide_end;
    
    return (z->yz_pixel_map);
}


unsigned char *xz_reconstruction(Image *img, c_zdimension *z, int i_px, int copy_width){
    unsigned int    x = 0;
    int             width = (int)img->columns;
    PixelPacket     *px;
    
    if (z->xz_pixel_map == 0){
        z->p_dec = z->z_dec;
        z->x_dec = (int)img->columns;
        z->xz_pixel_map = malloc(z->x_dec *  z->p_dec * sizeof(*z->xz_pixel_map));
    }
    px = GetImagePixels(img, 0, 0, img->columns, img->rows);
    
    z->xz_count_slide ++ ; // count how many slide in brain tissue
    z->xz_pixel_count_slide_end = z->xz_count_slide * copy_width;
    
    while (x < width) {
        for (int j = z->xz_pixel_count_slide_start; j <= z->xz_pixel_count_slide_end; j++) {
            z->xz_pixel_map[(z->x_dec * j) + x] = BLACK;
        }
        x++;
    }
    x = 0;
    while (x < width) {
        for (int j = z->xz_pixel_count_slide_start; j <= z->xz_pixel_count_slide_end; j++) {
            z->xz_pixel_map[(z->x_dec * j) + x] = (char)px[(width * i_px) + x].green;
        }
        x++;
    }
    
    z->xz_pixel_count_slide_start = z->xz_pixel_count_slide_end;
    return (z->xz_pixel_map);
}

Image   *yz_final_construct(c_zdimension *z){
    Image           *final_images;
    ExceptionInfo	exception;
    
    GetExceptionInfo(&exception);
    final_images = ConstituteImage(z->z_dec, z->y_dec, "I", CharPixel, z->yz_pixel_map, &exception);
    free(z->yz_pixel_map);
    
    return (final_images);
}

Image   *xz_final_construct(c_zdimension *z){
    Image           *final_images;
    ExceptionInfo	exception;
    
    GetExceptionInfo(&exception);
    final_images = ConstituteImage(z->x_dec, z->p_dec, "I", CharPixel, z->xz_pixel_map, &exception);
    free(z->xz_pixel_map);
    
    return (final_images);
}

/* look through 3 images before and after for pixel comparsion! */
Image   *get_avg_pixel(Image *img, Image *first_img, Image *second_img, Image *third_img){
    Image           *new_img;
    unsigned char   *pixel_map = malloc(img->columns * img->rows * sizeof(*pixel_map));
    unsigned int    x = 0, y = 0;
    unsigned int    i = 0;
    int             width = (int)img->columns;
    int             height = (int)img->rows;
    PixelPacket     *f_px = GetImagePixels(first_img, 0, 0, first_img->columns, first_img->rows);
    PixelPacket     *s_px = GetImagePixels(second_img, 0, 0, second_img->columns, second_img->rows);
    PixelPacket     *t_px = GetImagePixels(third_img, 0, 0, third_img->columns, third_img->rows);
    
    ExceptionInfo	exception;
    GetExceptionInfo(&exception);
    
    while (y < height) {
        x = 0;
        while (x < width) {
            i = ((int)f_px[(width * y) + x].green + (int)t_px[(width * y) + x].green)/2;
            
            if(i > (int)t_px[(width * y) + x].green + 5 || i > (int)f_px[(width * y) + x].green + 5)
                pixel_map[(width * y) + x] =  (char)s_px[(width * y) + x].green;
            else if ((int)s_px[(width * y) + x].green > i && i < (int)s_px[(width * y) + x].green)
                pixel_map[(width * y) + x] = i;
            else
                pixel_map[(width * y) + x] = (char)s_px[(width * y) + x].green;
            x++;
        }
        y++;
    }
    
    DestroyImage(img);
    DestroyImage(first_img);
    DestroyImage(second_img);
    DestroyImage(third_img);
    
    
    new_img = ConstituteImage(width, height, "I", CharPixel, pixel_map, &exception);
    free(pixel_map);
    SyncImagePixels(new_img);
    return (new_img);
}
