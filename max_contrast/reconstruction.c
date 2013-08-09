//
//  reconstruction.c
//  max_contrast
//
//  Created by Adam Lin on 23/07/13.
//  Copyright (c) 2013 Adam Lin. All rights reserved.
//

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
Image   *get_avg_pixel(Image *f_img, Image *s_img, Image *t_img){
    Image           *new_images;
    ExceptionInfo	exception;
    unsigned char   *pixel_map = malloc(f_img->columns * f_img->rows * sizeof(*pixel_map));
    unsigned int    x = 0;
    unsigned int    y = 0;
    int             width = (int)f_img->columns;
    int             height = (int)f_img->rows;
    PixelPacket     *f_px, *s_px, *t_px;
    
    GetExceptionInfo(&exception);
    
    f_px = GetImagePixels(f_img, 0, 0, f_img->columns, f_img->rows);
    s_px = GetImagePixels(s_img, 0, 0, s_img->columns, s_img->rows);
    t_px = GetImagePixels(t_img, 0, 0, t_img->columns, t_img->rows);
    
    while (y < height) {
        x = 0;
        while (x < width) {
            int i = (f_px[(width * y) + x].green + t_px[(width * y) + x].green)/2;
            /*
            if (i < (s_px[(width * y) + x].green) && i > (t_px[(width * y) + x].green) ) {
                pixel_map[(width * y) + x] = s_px[(width * y) + x].green;
            }
             */
            if ((s_px[(width * y) + x].green - 5) > i && i < (s_px[(width * y) + x].green + 5) ) {
                //printf("first:%i | second:%i | thrid:%i\n",f_px[(width * y) + x].green , s_px[(width * y) + x].green, t_px[(width * y) + x].green);
                pixel_map[(width * y) + x] = i;
                //printf("final:%i \n",i);
            }
            else
                pixel_map[(width * y) + x] = s_px[(width * y) + x].green;
            x++;
        }
        y++;
    }
    
    new_images = ConstituteImage(width, height, "I", CharPixel, pixel_map, &exception);

    free(pixel_map);
    DestroyImage(f_img);
    DestroyImage(s_img);
    DestroyImage(t_img);
    return new_images;
}
