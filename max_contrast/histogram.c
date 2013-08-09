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


int		*create_histogram(Image *img, c_image_args *a)
{
    int		i = 0;
    int		*histogram = malloc(256 * sizeof(*histogram));
    PixelPacket	*px;
    
    px = GetImagePixels(img, 0, 0, img->columns, img->rows);
    
    a->height = (int)(img->rows);
    a->width = (int)(img->columns);
    
    //memset(histogram, 0, 255);
    while (i < 255){
        histogram[i++] = 0;
    }
    
    a->size = (int)(img->columns * img->rows);
    i = 0;
    while (i < a->size)
    {
        histogram[(int)px[i].blue] ++;
        i++;
    }
   
    return (histogram);
}

void		dump_histogram(int *histogram, char *path, c_histogram *c, char *file_name, char *file_type)
{
    char		*final_path;
    asprintf(&final_path, "%s%s.%s", path, file_name, file_type);
    
    int         fd = open(final_path, O_CREAT|O_TRUNC|O_RDWR, 0755);
    int         i = 0;
    char		*val;
    
    if(c->first_his_val == NULL){
        while (i < 255)
        {
            asprintf(&val, "%i\n", histogram[i]);
            write(fd, val, strlen(val));
            
            if(c->first_his_val == NULL)
                asprintf(&c->first_his_val, "%s", val);
            asprintf(&c->first_his_val, "%s%s", c->first_his_val, val);
            i++;
        }
    }
    else if (c->sec_his_val == NULL){
        while (i < 255)
        {
            asprintf(&val, "%i\n", histogram[i]);
            write(fd, val, strlen(val));
            
            if(c->sec_his_val == NULL){
                asprintf(&c->sec_his_val, "%s", val);
            }
            asprintf(&c->sec_his_val, "%s%s", c->sec_his_val, val);
            i++;
        }
    }
    else if (c->first_his_val != NULL && c->sec_his_val != NULL ){
        c->first_his_val = c->sec_his_val;
        c->sec_his_val = NULL;
        while (i < 255)
        {
            asprintf(&val, "%i\n", histogram[i]);
            write(fd, val, strlen(val));
            if(c->sec_his_val == NULL){
                asprintf(&c->sec_his_val, "%s", val);
            }
            asprintf(&c->sec_his_val, "%s%s", c->sec_his_val, val);
            i++;
        }
    }
    free(final_path);
    free(histogram);
    close(fd);
}

unsigned char	get_contrasted_value(unsigned char min, unsigned char max,
                                     unsigned char dataset_min, unsigned char dataset_max, unsigned char initial_value)
{
    float	ratio;
    unsigned char	final_value;
    
    ratio = (float)((float)((float)max - (float)min) / (float)((float)dataset_max - (float)dataset_min));
    final_value = min + round(initial_value * ratio);
    return (final_value);
}

void		apply_histogram(PixelPacket *px, unsigned char min, unsigned char max,
                            unsigned char dataset_min, unsigned char dataset_max,
                            int width, int height, unsigned long quantum_depth)
{
    int		i = 0;
    int		j = 0;
    unsigned long long int pixel_value;
    
    while (i < height)
    {
        j = 0;
        while (j < width)
        {
            pixel_value = (unsigned long long int) px[(width * i) + j].red;
            if (QuantumDepth != 8 && quantum_depth == QuantumDepth){
                pixel_value = mapUnsignedValue(quantum_depth, 8, (unsigned short) pixel_value);
            }
            pixel_value = (pixel_value >= 255 ? 254 : pixel_value);
            
            pixel_value = get_contrasted_value(min, max, dataset_min, dataset_max, pixel_value);
            
            if (QuantumDepth != 8 && quantum_depth == QuantumDepth){
                pixel_value = mapUnsignedValue(8, quantum_depth, (unsigned short) pixel_value);
            }
            px[(width * i) + j].red = px[(width * i) + j].green = px[(width * i) + j].blue = (unsigned char) pixel_value;
            j++;
        }
        i++;
    }
}


void    *look_through_histogram(int *histogram, c_histogram *c)
{
    char    *delims = "\n";
    char    *his_first_val;
    char    *his_sec_val;
    int     a, d;
    
    if (c->first_his_val != NULL && c->sec_his_val != NULL) {
        his_first_val = strtok(c->first_his_val,delims);
        his_sec_val = strtok(c->sec_his_val,delims);
        
        while (his_first_val && his_sec_val) {
            d = atoi(his_first_val) - atoi(his_sec_val);
            if (d < 0)
                a = atoi(his_first_val) + d;
            else
                a = atoi(his_first_val) - d;
            
            if(c->final_his_val != NULL)
                asprintf(&c->final_his_val, "%s|%i", c->final_his_val, a);
            if(c->final_his_val == NULL)
                asprintf(&c->final_his_val, "%i", a);
            
            his_first_val = strtok(NULL, delims);
            his_sec_val = strtok(NULL, delims);
        }
    }
    return 0;
}

Image       *dump_his_image(Image *img, int *histogram, c_image_args *a)
{
    PixelPacket     *px;
    
    if ((px = GetImagePixels(img, 0, 0, a->width, a->height)) == NULL)	{
		DestroyImage(img);
		return 0;
	}
    apply_histogram(px, 300, 500, 0, 255, a->width, a->height, img->depth);
    SyncImagePixels(img);
    return img;
}
