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
 ** Cross Correlation Algorithm: 
 ** = sum((a*b)^2) / (sqrt(sum(a^2)) * sqrt(sum(b^2))
 */

#include "max_contrast.h"

double  *cross_correlation(Image *first_img, Image *second_img, c_cross_correlation *b){
    int             width = (int)first_img->columns;
    int             height = (int)first_img->rows;
    unsigned int    x = 0, y = 0;
    PixelPacket     *f_px = GetImagePixels(first_img, 0, 0, first_img->columns, first_img->rows);
    PixelPacket     *s_px = GetImagePixels(second_img, 0, 0, second_img->columns, second_img->rows);
    
    int             i,j;
    int             n = width * 200; // --> important!! look at first 30 pixels heigh for auto correlation!
    int             maxdelay = 50, delay = 40;
    double          max_f_px, max_s_px, sum_f_px, sum_s_px, ssum_f_s_px;
    double          denom;
    
    max_f_px = 0;
    max_s_px = 0;
    /* Calculate the mean of the two series x[], y[] */
    
    while (y < 200) {
        x = 0;
        while (x < width) {
            max_f_px += f_px[(width * y) + x].red;
            max_s_px += s_px[(width * y) + x].red;
            x++;
        }
        y++;
    }
    y = 0;
    
    /* means of the corresponding series */
    max_f_px /= n;
    max_s_px /= n;
    
    /* Calculate the denominator */
    sum_f_px = 0;
    sum_s_px = 0;
    
    while (y < 200) {
        x = 0;
        while (x < width) {
            sum_f_px += (f_px[(width * y) + x].red - max_f_px) * (f_px[(width * y) + x].red - max_f_px);
            sum_s_px += (s_px[(width * y) + x].red - max_s_px) * (s_px[(width * y) + x].red - max_s_px);
            x++;
        }
        y++;
    }
    y = 0;
    
    denom = sqrt(sum_f_px*sum_s_px);
    /* Calculate the correlation series */
    for (delay =-maxdelay; delay < maxdelay; delay++) {
        ssum_f_s_px = 0;
        
        while (y < 200) {
            x = 0;
            while (x < width) {
                j = ((width * y) + x) + delay;
                
                if (j < 0 || j >= n)
                    ssum_f_s_px += (f_px[(width * y) + x].red - max_f_px) * (-max_s_px);
                else
                    ssum_f_s_px += (f_px[(width * y) + x].red - max_f_px) * (s_px[(width * y) + x].red - max_s_px);
                
                 /*
                 if (j < 0 || j >= n)
                     continue;
                 else
                     ssum_f_s_px += (f_px[(width * y) + x].red - m_f_px) * (s_px[(width * y) + x].red - m_s_px);
                 */
                
                /*
                while (j < 0)
                    j += n;
                j %= n;
                ssum_f_s_px += (f_px[(width * y) + x].red - m_f_px) * (s_px[(width * y) + x].red - m_s_px);
                 */
                
                x++;
            }
            y++;
        }
        y = 0;
        
        b->ssum = ssum_f_s_px / denom;
    }
    
    if ( b->ssum > 0 && b->ssum < 0.5) {
        //printf("m_f: %f | m_s: %f\n", max_f_px, max_s_px);
        //printf("s_f: %f | s_s: %f\n", sum_f_px/n, sum_s_px/n);
        printf("r: %f | move right or left pixel: %f\n", b->ssum, ((50 - 10)*b->ssum)+10);

    }
    else if ( b->ssum < 0) {
        printf("r: %f | move up or down pixel: %f\n", b->ssum, ((50 - 10)*b->ssum)+10);
    }
    
    
    /* Actually it is going to destroy second and third images */
    DestroyImage(first_img);
    DestroyImage(second_img);
    
    return &(b->ssum);
}