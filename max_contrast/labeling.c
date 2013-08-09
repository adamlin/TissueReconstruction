//
//  labeling.c
//  max_contrast
//
//  Created by Adam Lin on 28/06/13.
//  Copyright (c) 2013 Adam Lin. All rights reserved.
//

#include "max_contrast.h"

char        get_neighborpixcel(unsigned char *pixel_map, int x, int y, int width, int height, int color, c_image *c)
{
    int     y_plus = (y + 1 > height ? height : y + 1);
    int     x_plus = (x + 1 > width ? width : x + 1);
    int     y_minus = (y - 1 < 0 ? 0 : y - 1);
    int     x_minus = (x - 1 <  0 ? 0 : x - 1);
    
    int     corner_up_left = (y_minus * width) + x_minus;
    int     corner_up_right = (y_minus * width) + x_plus;
    int     corner_down_left = (y_plus * width) + x_minus;
    int     corner_down_right = (y_plus * width) + x_plus;
    int     up = (y_minus * width) + x;
    int     down = (y_plus * width) + x;
    int     left = (y * width) + x_minus;
    int     right = (y *  width) + x_plus;
    
    if(y < height - 1){
        if (pixel_map[corner_up_left] == color && c->label_map[corner_up_left] == 1){
            return pixel_map[corner_up_left];
        }
        else if (pixel_map[corner_up_right] == color && c->label_map[corner_up_left] == 1){
            return pixel_map[corner_up_right];
        }
        else if (pixel_map[corner_down_left] == color && c->label_map[corner_up_left] == 1){
            return pixel_map[corner_down_left];
        }
        else if (pixel_map[corner_down_right] == color && c->label_map[corner_up_left] == 1){
            return pixel_map[corner_down_right];
        }
        else if (pixel_map[up] == color && c->label_map[corner_up_left] == 1){
            return pixel_map[up];
        }
        else if (pixel_map[down] == color && c->label_map[corner_up_left] == 1){
            return pixel_map[down];
        }
        else if (pixel_map[left] == color && c->label_map[corner_up_left] == 1){
            return pixel_map[left];
        }
        else if (pixel_map[right] == color && c->label_map[corner_up_left] == 1){
            return pixel_map[right];
        }
    }
    return color;
}

Image   *get_blur_maked(Image *img, c_image *c)
{
    Image           *new_img;
    static  unsigned short    color = 0;
    unsigned int    x = 0;
    unsigned int    y = 0;
    unsigned char   *pixel_map = malloc(img->columns * img->rows * sizeof(*pixel_map));
    
    c->label_map = malloc(img->columns * img->rows * sizeof(*c->label_map));
    
    int             width = (int)img->columns;
    int             height = (int)img->rows;
    
    PixelPacket     *px;
    ExceptionInfo	exception;
    
    GetExceptionInfo(&exception);
    
    px = GetImagePixels(img, 0, 0, img->columns, img->rows);
      
    while (y < height) {
        x = 0;
        while (x < width) {
            pixel_map[(width * y) + x] = (char)px[(width * y) + x].green;
            c->label_map[(width * y) + x] = 0;
            x++;
        }
        y++;
    }
    y = 0;
    
    while (y < height){
        x = 0;
        while (x < width){
            if (pixel_map[(width * y) + x] != WHITE){
                c->label_map[(width * y) + x] = 1;
                pixel_map[(width * y) + x] = get_neighborpixcel(pixel_map, x, y, width, height, color, c);
            }
            else{
                c->label_map[(width * y) + x] = 0;
            }
            if(color >= 255){
                color = 0;
            }
            x++;
        }
        y++;
     }
    new_img = ConstituteImage(width, height, "I", CharPixel, pixel_map, &exception);
    free(c->label_map);
    free(pixel_map);
    DestroyImage(img);
    SyncImage(new_img);
    return (new_img);
}


int     check_srounding(unsigned char *pixel_map, int x, int y, int width, int height)
{
    // CHECK SROUNDING COLORED PIXELS (10)
    int     y_plus = (y + 10 > height ? height : y + 10);
    int     x_plus = (x + 10 > width ? width : x + 10);
    int     y_minus = (y - 10 < 0 ? 0 : y - 10);
    int     x_minus = (x - 10 <  0 ? 0 : x - 10);
    
    int     up = (y_minus * width) + x;
    int     down = (y_plus * width) + x;
    int     left = (y * width) + x_minus;
    int     right = (y *  width) + x_plus;
    
    if(y < height - 10 && x < width - 10){
         if (pixel_map[up] == FILL_COLOR || pixel_map[down] == FILL_COLOR || pixel_map[left] == FILL_COLOR || pixel_map[right] == FILL_COLOR)
            return TRUE;
    }
    return FALSE;
}

Image   *crop_area_selected(Image *img)
{
    Image           *final_images;
    unsigned char   *pixel_map = malloc(img->columns * img->rows * sizeof(*pixel_map));
    unsigned int    x = 0;
    unsigned int    y = 0;
    int             width = (int)img->columns;
    int             height = (int)img->rows;
    PixelPacket     *px;
    ExceptionInfo	exception;
    
    GetExceptionInfo(&exception);
    
    px = GetImagePixels(img, 0, 0, img->columns, img->rows);
    
    while (y < height) {
        x = 0;
        while (x < width) {
            pixel_map[(width * y) + x] = (char)px[(width * y) + x].green;
            x++;
        }
        y++;
    }
    y = 0;
    
    while (y < height){
        x = 0;
        while (x < width){
            if (pixel_map[(width * y) + x] == BLACK){
                if (check_srounding(pixel_map, x, y, width, height) == TRUE){
                    pixel_map[(width * y) + x] = OUTLINE;
                }
            }
            if (pixel_map[(width * y) + x] !=FILL_COLOR && pixel_map[(width * y) + x] != OUTLINE)
                pixel_map[(width * y) + x] = WHITE;
            x++;
        }
        y++;
    }
    final_images = ConstituteImage(width, height, "I", CharPixel, pixel_map, &exception);
    free(pixel_map);
    return (final_images);
}


Image   *crop_masked_final_image(Image *org_img, Image *mask_img)
{
    Image           *final_image;
    unsigned int    x = 0;
    unsigned int    y = 0;
    int             width = (int)org_img->columns;
    int             height = (int)org_img->rows;
    PixelPacket     *px;
    PixelPacket     *mask_px;
    ExceptionInfo	exception;
    
    GetExceptionInfo(&exception);
    
    px = GetImagePixels(org_img, 0, 0, org_img->columns, org_img->rows);
    mask_px = GetImagePixels(mask_img, 0, 0, mask_img->columns, mask_img->rows);
    
    while (y < height) {
        x = 0;
        while (x < width) {
            if (mask_px[(width * y) + x].red == WHITE) {
                px[(width * y) + x].green = px[(width * y) + x].blue = px[(width * y) + x].red = BLACK;
            }
            x++;
        }
        y++;
    }
    return (org_img);
}