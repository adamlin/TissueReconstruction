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
 ** along with BananaCam.  If not, see <http://www.gnu.org/licenses/>.
 **
 */

#include "max_contrast.h"

char* otsu_th(int x_size1, int y_size1, char *image1, c_threshold *c)
{
    int     x_size2, y_size2;
    char    *image2 = malloc(x_size1 * y_size1 * sizeof(*image2));
    int     hist[GRAYLEVEL];
    double  prob[GRAYLEVEL], omega[GRAYLEVEL]; /* prob of graylevels */
    double  myu[GRAYLEVEL];   /* mean value for separation */
    double  max_sigma, sigma[GRAYLEVEL]; /* inter-class variance */
    int     i, x, y; /* Loop variable */
    int     threshold; /* threshold for binarization */
    
    /* Histogram generation */
    for (i = 0; i < GRAYLEVEL; i++) hist[i] = 0;
    for (y = 0; y < y_size1; y++)
        for (x = 0; x < x_size1; x++) {
            hist[image1[y * x_size1 + x]]++;
        }
    /* calculation of probability density */
    for ( i = 0; i < GRAYLEVEL; i ++ ) {
        prob[i] = (double)hist[i] / (x_size1 * y_size1);
    }
    
    /* omega & myu generation */
    omega[0] = prob[0];
    myu[0] = 0.0;       /* 0.0 times prob[0] equals zero */
    for (i = 1; i < GRAYLEVEL; i++) {
        omega[i] = omega[i-1] + prob[i];
        myu[i] = myu[i-1] + i*prob[i];
    }
    
    /* sigma maximization
     sigma stands for inter-class variance
     and determines optimal threshold value */
    threshold = 0;
    max_sigma = 0.0;
    for (i = 0; i < GRAYLEVEL - 1; i++) {
        if (omega[i] != 0.0 && omega[i] != 1.0){
            sigma[i] = pow(myu[GRAYLEVEL-1]*omega[i] - myu[i], 2) /
            (omega[i]*(1.0 - omega[i]));
        }
        else
            sigma[i] = 0.0;
        
        if (sigma[i] > max_sigma) {
            max_sigma = sigma[i];
            threshold = i;
        }
    }
    
    if (c->max_threshold < threshold) {
        c->max_threshold = threshold;
    }
    
    /* binarization output into image2 */
    x_size2 = x_size1;
    y_size2 = y_size1;
    
    for (y = 0; y < y_size2; y++){
        for (x = 0; x < x_size2; x++){
            if (image1[(y * x_size1) + x] < c->max_threshold - 20){
                image2[(y * x_size2) + x] = MAX_BRIGHTNESS;
            }
            else
                image2[(y * x_size2) +x] = 0;
        }
    }
    return (image2);
}
