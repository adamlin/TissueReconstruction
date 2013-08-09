//
//  otsu_th.c
//  max_contrast
//
//  Created by Adam Lin on 17/06/13.
//  Copyright (c) 2013 Adam Lin. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "max_contrast.h"

char* otsu_th(int x_size1, int y_size1, char *image1, c_threshold *c)
{
    int x_size2, y_size2;
    char *image2 = malloc(x_size1 * y_size1 * sizeof(*image2));
    int hist[GRAYLEVEL];
    double prob[GRAYLEVEL], omega[GRAYLEVEL]; /* prob of graylevels */
    double myu[GRAYLEVEL];   /* mean value for separation */
    double max_sigma, sigma[GRAYLEVEL]; /* inter-class variance */
    int i, x, y; /* Loop variable */
    int threshold; /* threshold for binarization */
    
    //printf("Otsu's binarization process starts now.\n");
    
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
    printf("threshold value = %d and max threshold = %i\n\n", threshold, c->max_threshold);
    
    /* binarization output into image2 */
    x_size2 = x_size1;
    y_size2 = y_size1;
    
    for (y = 0; y < y_size2; y++)
        for (x = 0; x < x_size2; x++)
            if (image1[(y * x_size1) + x] < c->max_threshold -1){
                image2[(y * x_size2) + x] = MAX_BRIGHTNESS;
            }
            else
                image2[(y * x_size2) +x] = 0;
    return (image2);
}
