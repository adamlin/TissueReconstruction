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
 ** r = sum((a*b)^2) / (sqrt(sum(a^2)) * sqrt(sum(b^2))
 ** z-score differences = sum( ((a - mean(a)) / stdev(a)) - ((b - mean(b)) / stdev(b)) ) / nvox
 */

#include "max_contrast.h"

/*  2D Pattern Identification using Cross Correlation   - not working well  */
double    pattern_identification(PixelPacket *f_px, PixelPacket *s_px, int width, int height){
    double          r;
    unsigned int    x = 0, y = 0;
    int             jj, ii;
    double          mean_f_px = 0, mean_s_px = 0;
    int             n = width * height;
    /* Calculate the mean of the two series x[], y[] */
    
    while (y < 200) {
        x = 0;
        while (x < width) {
            mean_f_px += f_px[(width * y) + x].red;
            x++;
        }
        y++;
    }
    y = 50;
    while (y < 150) {
        x = 100;
        while (x < 300) {
            mean_s_px += s_px[(width * y) + x].red;
            x++;
        }
        y++;
    }
    mean_f_px /= n;
    mean_s_px = mean_s_px / (200*100);
    
    
    for (jj = -height/2; jj < height/2; jj++) {
        for (ii = - width/2; ii< width/2; ii++) {
            r += ((ii + width/2) * (jj + height/2) - mean_s_px)*((width + ii)*(height + jj) - mean_f_px);
        }
    }
    return r;
}

/*  zscore - z-score differences    */
double    do_int_calcs(double f_px, double s_px, double mean_f_px, double mean_s_px, double sum_f_px, double sum_s_px, double nvox){
    double sd_f, sd_s, zscore;
    
    /* mean */
    mean_f_px = sum_f_px / nvox;
    mean_s_px = sum_s_px / nvox;
    
    /* variance  and sd */
    sd_f = sqrt(((nvox * SQR2(sum_f_px)) - SQR2(f_px)) / (nvox * (nvox - 1)));
    sd_s = sqrt(((nvox * SQR2(sum_s_px)) - SQR2(s_px)) / (nvox * (nvox - 1)));
    
    for (int i = 0; i < nvox; i ++) {
        zscore += fabs((f_px - mean_f_px)/sd_f - (s_px - mean_s_px)/sd_s)/nvox;
    }
    return zscore;
}

double  *cross_correlation(Image *first_img, Image *second_img, c_cross_correlation *b){
    int             width = (int)first_img->columns;
    //int             height = (int)first_img->rows;
    unsigned int    x = 0, y = 0;
    PixelPacket     *f_px = GetImagePixels(first_img, 0, 0, first_img->columns, first_img->rows);
    PixelPacket     *s_px = GetImagePixels(second_img, 0, 0, second_img->columns, second_img->rows);
    
    int             j;
    int             n = width * 200; // --> important!! look at first 200 pixels heigh for auto correlation!
    int             maxdelay = 50, delay = 40;
    double          max_f_px, max_s_px, sum_f_px, sum_s_px, ssum_f_s_px;
    double          denom;
    double          save_f_px, save_s_px;
    
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
    save_f_px = max_f_px;
    save_s_px = max_s_px;
    
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
                /*
                if (j < 0 || j >= n)
                    ssum_f_s_px += (f_px[(width * y) + x].red - max_f_px) * (-max_s_px);
                else
                    ssum_f_s_px += (f_px[(width * y) + x].red - max_f_px) * (s_px[(width * y) + x].red - max_s_px);
                */
                 /*
                 if (j < 0 || j >= n)
                     continue;
                 else
                     ssum_f_s_px += (f_px[(width * y) + x].red - max_f_px) * (s_px[(width * y) + x].red - max_s_px);
                 */
                
                while (j < 0)
                    j += n;
                j %= n;
                ssum_f_s_px += (f_px[(width * y) + x].red - max_f_px) * (s_px[(width * y) + x].red - max_s_px);
                
                x++;
            }
            y++;
        }
        y = 0;
        
        b->ssum = ssum_f_s_px / denom;
    }
    
    if ( b->ssum > 0 && b->ssum < 0.5) {
        //double final_px =  pattern_identification(f_px, s_px, width, 200);
        double final_px = do_int_calcs(save_f_px, save_s_px, max_f_px, max_s_px, sum_f_px, sum_s_px, n);
        //double final_px = 10 + b->ssum;
        printf("r: %f | zscore: %f\n", b->ssum, final_px);

    }
    else if ( b->ssum < 0) {
        printf("r: %f | zscore: %f\n", b->ssum, ((50 - 10)*b->ssum)+10);
    }
    
    
    /* This is going to destroy second and third images */
    DestroyImage(first_img);
    DestroyImage(second_img);
    
    return &(b->ssum);
}