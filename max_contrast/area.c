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

char        check_neighborhood(unsigned char *pixel_map, int x, int y, int width, int height)
{
    ExceptionInfo	exception;
    static int      count = 0;
    static  unsigned short    color = 10;
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

    
    GetExceptionInfo(&exception);
    if(y < height -1){
        
        if (pixel_map[corner_up_left] != BLACK && pixel_map[corner_up_left] != WHITE)
            return pixel_map[corner_up_left];
        if (pixel_map[corner_up_right] != BLACK && pixel_map[corner_up_right] != WHITE)
            return pixel_map[corner_up_right];
        if (pixel_map[corner_down_left] != BLACK && pixel_map[corner_down_left] != WHITE)
            return pixel_map[corner_down_left];
        if (pixel_map[corner_down_right] != BLACK && pixel_map[corner_down_right] != WHITE)
            return pixel_map[corner_down_right];
        
        if (pixel_map[up] != BLACK && pixel_map[up] != WHITE)
            return pixel_map[up];
        if (pixel_map[down] != BLACK && pixel_map[down] != WHITE)
            return pixel_map[down];
        if (pixel_map[left] != BLACK && pixel_map[left] != WHITE)
            return pixel_map[left];
        if (pixel_map[right] != BLACK && pixel_map[right] != WHITE)
            return pixel_map[right];

    }
    color += 5;
    
    if(color >= WHITE || color == COLORED || color == BLACK || color == OUTLINE){
        color = 10;
    }
    color = (color == 0 ? 5 : (color == WHITE ? 5 : color));
    count++;
    return color;
}

char        check_neighborhoodc(unsigned char *pixel_map, int x, int y, int width, int height)
{
    ExceptionInfo	exception;
    // Image           *img;
    char            *name;
    static int      count = 0;
    static  unsigned short    color = 10;
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
    
    
    GetExceptionInfo(&exception);
    if(y < height -1){
        if (pixel_map[corner_down_left] != BLACK && pixel_map[corner_down_left] != WHITE)
            return pixel_map[corner_down_left];
        if (pixel_map[left] != BLACK && pixel_map[left] != WHITE)
            return pixel_map[left];
        if (pixel_map[corner_up_left] != BLACK && pixel_map[corner_up_left] != WHITE)
            return pixel_map[corner_up_left];
        if (pixel_map[up] != BLACK && pixel_map[up] != WHITE)
            return pixel_map[up];
        if (pixel_map[corner_up_right] != BLACK && pixel_map[corner_up_right] != WHITE)
            return pixel_map[corner_up_right];
        if (pixel_map[right] != BLACK && pixel_map[right] != WHITE)
            return pixel_map[right];
        if (pixel_map[corner_down_right] != BLACK && pixel_map[corner_down_right] != WHITE)
            return pixel_map[corner_down_right];
        if (pixel_map[down] != BLACK && pixel_map[down] != WHITE)
            return pixel_map[down];
    }
    color += 5;
    
    if(color >= WHITE || color == COLORED || color == BLACK || color == OUTLINE){
        color = 10;
    }
    color = (color == 0 ? 5 : (color == WHITE ? 5 : color));
    // asprintf(&name, "%i", count);
    //img = ConstituteImage(width, height, "I", CharPixel, pixel_map, &exception);
    //dump_image(img, FILE_THRESHOD_OUTPUT, name, "png");
    count++;
    // free(name);
    return color;
}


Image   *get_area(Image *img)
{
    unsigned int    x = 0;
    unsigned int    y = 0;
    unsigned char   *pixel_map = malloc(img->columns * img->rows * sizeof(*pixel_map));
    int             width = (int)img->columns;
    int             height = (int)img->rows;
    
    int size = width * height;
    int used = 0;
    int i = 0, j = 1, increasing = 1 ,move = 0;
    
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

    printf("image size: %i:%i\n\n", height, width);
    
    // Spiral Method
    
    y = height/2;
    x = width/2;
    increasing = 1;
    move = 0;
    used = 0;
    pixel_map[(width * y) + x] = 200;
    
    while (i < size)
    {
        if (used == 0) {
            if (move != 2){
                while (j <= increasing) { // GO RIGHT
                    x += 1;
                    if (pixel_map[(width * y) + x] != BLACK){
                        pixel_map[(width * y) + x] = check_neighborhood(pixel_map, x, y, width, height);
                    }
                    j++;
                }
                j = 1;
                move ++;
            }
            if (move != 2){
                while (j <= increasing) { // GO DOWN
                    y += 1;
                    if (pixel_map[(width * y) + x] != BLACK){
                        pixel_map[(width * y) + x] = check_neighborhood(pixel_map, x, y, width, height);
                    }
                    j++;
                }
                j = 1;
                move ++;
            }
            used = 1;
        }
        else if(used == 1){
            if (move != 2){
                while (j <= increasing) { // GO LEFT
                    x -= 1;
                    if (pixel_map[(width * y) + x] != BLACK){
                        pixel_map[(width * y) + x] = check_neighborhood(pixel_map, x, y, width, height);
                    }
                    j++;
                }
                j = 1;
                move ++;
            }
            if (move != 2){
                while (j <= increasing) { // GO UP
                    y -= 1;
                    if (pixel_map[(width * y) + x] != BLACK){
                        pixel_map[(width * y) + x] = check_neighborhood(pixel_map, x, y, width, height);
                    }
                    j++;
                }
                j = 1;
                move ++;
            }
            used = 0;
        }
        
        increasing ++;
        if (increasing >= width - 1 || increasing >= height - 1 ) {
            break;
        }
        
        if (move == 2){
            move = 0;
        }
        i++;
    }

    
    // Reverse Spiral Method
    
    y = height/2;
    x = width/2;
    increasing = 1;
    move = 0;
    used = 0;
    while (i < size)
    {
        if (used == 0) {
            if (move != 2){
                while (j <= increasing) { // GO LEFT
                    x -= 1;
                    if (pixel_map[(width * y) + x] != BLACK){
                        pixel_map[(width * y) + x] = check_neighborhoodc(pixel_map, x, y, width, height);
                    }
                    j++;
                }
                j = 1;
                move ++;
                
                
            }
            if (move != 2){
                while (j <= increasing) { // GO UP
                    y -= 1;
                    if (pixel_map[(width * y) + x] != BLACK){
                        pixel_map[(width * y) + x] = check_neighborhoodc(pixel_map, x, y, width, height);
                    }
                    j++;
                }
                j = 1;
                move ++;
            }
            used = 1;
        }
        else if(used == 1){
            if (move != 2){
                while (j <= increasing) { // GO RIGHT
                    x += 1;
                    if (pixel_map[(width * y) + x] != BLACK){
                        pixel_map[(width * y) + x] = check_neighborhoodc(pixel_map, x, y, width, height);
                    }
                    j++;
                }
                j = 1;
                move ++;
            }
            if (move != 2){
                while (j <= increasing) { // GO DOWN
                    y += 1;
                    if (pixel_map[(width * y) + x] != BLACK){
                        pixel_map[(width * y) + x] = check_neighborhoodc(pixel_map, x, y, width, height);
                    }
                    j++;
                }
                j = 1;
                move ++;
            }
            used = 0;
        }
        
        increasing ++;
        if (increasing >= width - 1 || increasing >= height - 1 ) {
            break;
        }
        
        if (move == 2){
            move = 0;
        }
        i++;
    }
   

    // up --> down | right --> left
    y = 0;
    
    while (y < height)
    {
        x = width - 1;
        while (x < width){
            if (pixel_map[(width * y) + x] != BLACK){
                pixel_map[(width * y) + x] = check_neighborhood(pixel_map, x, y, width, height);
            }
            x--;
        }
        y++;
    }
    
    // up --> down | left --> right
    y = 0;
    
    while (y < height)
    {
        x = 0;
        while (x < width)
        {
            if (pixel_map[(width * y) + x] != BLACK){
                pixel_map[(width * y) + x] = check_neighborhood(pixel_map, x, y, width, height);
            }
            x++;
        }
        y++;
    }
     
    // down --> up | left --> right
    y = height - 1 ;
    
    while (y > 0)
    {
        x = 0;
        while (x < width)
        {
            if (pixel_map[(width * y) + x] != BLACK){
                pixel_map[(width * y) + x] = check_neighborhood(pixel_map, x, y, width, height);
            }
            x++;
        }
        y--;
    }
    
    //down --> up | right --> left
    
    y = height - 1 ;
    
    while (y > 0)
    {
        x = width - 1;
        while (x < width)
        {
            if (pixel_map[(width * y) + x] != BLACK){
                pixel_map[(width * y) + x] = check_neighborhood(pixel_map, x, y, width, height);
            }
            x--;
        }
        y--;
    }

    
    img = ConstituteImage(width, height, "I", CharPixel, pixel_map, &exception);
    free(pixel_map);
    return (img);
}

Image   *slant_line_detection(Image *img)
{
    static  unsigned short    temp_color;
    unsigned int    x = 0;
    unsigned int    y = 0;
    unsigned char   *pixel_map = malloc(img->columns * img->rows * sizeof(*pixel_map));
    int             width = (int)img->columns;
    int             height = (int)img->rows;    
    int             slant_count = 1;

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
    
    while (y < height)
    {
        x = 0;
        while (x < width){
            if (pixel_map[(width * y) + x] == BLACK){
                if (pixel_map[(width * (y+1)) + x] == COLORED || pixel_map[(width * y) + (x+1)] == COLORED ){
                    temp_color = pixel_map[(width * (y+1)) + x + 1];
                    if(temp_color != COLORED || temp_color != BLACK){
                        for (int j = 1 ; j < height - y; j++) {
                            if (pixel_map[(width * (y + j)) + (x + slant_count)] == temp_color){
                                pixel_map[(width * (y + j)) + (x + slant_count)] = COLORED;
                                // printf("%i:%i\n", (y + j), (x + slant_count));
                            }
                            if (pixel_map[(width * (y + j)) + (x + slant_count)] == BLACK ){
                                // printf("==================\n");
                                break;
                            }
                            slant_count++;
                            if (slant_count > width - x) {
                                slant_count = 1;
                                break;
                            }
                        }
                    }
                }
            }
            x++;
        }
        y++;
    }

    img = ConstituteImage(width, height, "I", CharPixel, pixel_map, &exception);
    free(pixel_map);
    return (img);
}
