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
 ** This cross correlation is refered to AutoCorrelation - 2D Pattern Identification
 ** Written by Paul Bourke, August 1996 - http://paulbourke.net/miscellaneous/correlate/
 */

#include "max_contrast.h"

double  *cross_correlation(Image *first_img, Image *second_img, c_cross_correlation *b){
    int             width = (int)first_img->columns;
    unsigned int    x = 0;
    unsigned int    y = 0;
    PixelPacket     *f_px = GetImagePixels(first_img, 0, 0, first_img->columns, first_img->rows);
    PixelPacket     *s_px = GetImagePixels(second_img, 0, 0, second_img->columns, second_img->rows);
    
    int             i,j;
    int             maxdelay = 10, delay = 0;
    double          mx,my,sx,sy,sxy,denom,r;
    int             n = width * 10; // look at first 10 pixels heigh for auto correlation!

    
    mx = 0;
    my = 0;
    /* Calculate the mean of the two series x[], y[] */
    
    while (y < 10) {
        x = 0;
        while (x < width) {
            mx += f_px[(width * y) + x].red;
            my += s_px[(width * y) + x].red;
            x++;
        }
        y++;
    }
    y = 0;
    
    mx /= n;
    my /= n;
    
    /* Calculate the denominator */
    sx = 0;
    sy = 0;
    
    while (y < 10) {
        x = 0;
        while (x < width) {
            sx += (f_px[(width * y) + x].red - mx) * (f_px[(width * y) + x].red - mx);
            sy += (s_px[(width * y) + x].red - my) * (s_px[(width * y) + x].red - my);
            x++;
        }
        y++;
    }
    y = 0;
    
    denom = sqrt(sx*sy);
    /* Calculate the correlation series */
    for (delay=-maxdelay;delay<maxdelay;delay++) {
        sxy = 0;

        for (i=0 ; i < n ; i++) {
            j = i + delay;
            while (j < 0)
                j += n;
            j %= n;
            sxy += (f_px[i].red - mx) * (s_px[i].red - my);
        }
        r = sxy / denom;
    }
    b->ssum = r;
    
    if (b->ssum < 0.5) {
        printf("%f\n", b->ssum);
    }
    
    /* Actually it is going to destroy second and third images */
    DestroyImage(first_img);
    DestroyImage(second_img);
    
    return &(b->ssum);
}