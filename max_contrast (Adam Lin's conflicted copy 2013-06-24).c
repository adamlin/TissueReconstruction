//
//  max_contrast.c
//  max_contrast
//
//  Created by Adam Lin on 14/06/13.
//  Copyright (c) 2013 Adam Lin. All rights reserved.
//

#include <stdio.h>
#include "max_contrast.h"

Image		*get_image_from_path(char *path)
{
    ImageInfo	*image_info;
    Image		*img;
    ExceptionInfo	exception;
    
    GetExceptionInfo(&exception);
    if ((image_info = CloneImageInfo((ImageInfo *)NULL)) == NULL) {
        CatchException(&exception);
        DestroyImageInfo(image_info);
        return (NULL);
    }
    
    strcpy(image_info->filename, path);
    
    if ((img = ReadImage(image_info, &exception)) == NULL)
    {
        CatchException(&exception);
        DestroyImageInfo(image_info);
        return (NULL);
    }
    DestroyImageInfo(image_info);
    return (img);
}

int   		dump_image(Image *img, char *root_path, char *name, char *image_type)
{
    char		*final_path;
    ImageInfo	*image_info;
    ExceptionInfo	exception;
    
    GetExceptionInfo(&exception);
    if ((image_info = CloneImageInfo((ImageInfo *)NULL)) == NULL) {
        CatchException(&exception);
        DestroyImageInfo(image_info);
        return -1;
    }
    
    asprintf(&final_path, "%s/%s.%s", root_path, name, image_type);
    strcpy(img->filename, final_path);
    WriteImage(image_info, img);
    free(final_path);
    DestroyImageInfo(image_info);
    return (0);
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
    close(fd);
}

Image *transparent_image(Image *img)
{   
    int             i = 0, j = 0, size_x, size_y;
    char            *string_img = malloc(img->rows * img->columns * sizeof(*string_img));
    PixelPacket     *px;
    char            *pixel_value;
    ExceptionInfo   exception;
    
    GetExceptionInfo(&exception);
    
    px = GetImagePixels(img, 0, 0, img->columns, img->rows);
    size_x = (int)img->columns;
    size_y = (int)img->rows;
    
    while (i < size_y) {
        j = 0 ;
        while (j < size_x) {
            pixel_value = (char *)px[(size_x * i) + j].green;
            printf("%c",px[i].green);
        }
    }
    return (img);
}

Image *crop_image(Image *img, char *path)
{
    ImageInfo       *image_info;
    Image           *tmp;
    RectangleInfo   *portion;
    ExceptionInfo   exception;


    portion = malloc(sizeof(*portion));
    portion->width = IMAGE_WIDTH;
    portion->height = IMAGE_HEIGHT;
    portion->x = IMAGE_WIDTH_OFFSET;
    portion->y = IMAGE_HEIGHT_OFFSET;
    
    
    GetExceptionInfo(&exception);
    if ((image_info = CloneImageInfo((ImageInfo *)NULL)) == NULL) {
        CatchException(&exception);
        DestroyImageInfo(image_info);
        return (NULL);
    }
    
    strcpy(image_info->filename, path);
    
    if ((img = ReadImage(image_info, &exception)) == NULL)
    {
        CatchException(&exception);
        DestroyImageInfo(image_info);
        return (NULL);
    }
    tmp = img;
    
    if ((img = CropImage(img, portion, &exception)) == NULL) {
        CatchException(&exception);
        DestroyImage(tmp);
        DestroyImageInfo(image_info);
        return (NULL);
    }
    
    DestroyImage(tmp);
    DestroyImageInfo(image_info);
    return (img);
}

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

Image		*get_grayscale_image(Image *img)
{
    QuantizeInfo	info;
    
    GetQuantizeInfo(&info);
    info.colorspace = GRAYColorspace;
    info.number_colors = 256;
    QuantizeImage(&info, img);
    return (img);
}

Image   *get_threshold_image(Image *img, c_threshold *c)
{
    int size_x, size_y;
    int i = 0;
    int j = 0;
    char *string_img = malloc(img->rows * img->columns * sizeof(*string_img));
    char *temp_string_img;
    PixelPacket	*px;
    ExceptionInfo	exception;
    
    GetExceptionInfo(&exception);

    
    px = GetImagePixels(img, 0, 0, img->columns, img->rows);
    size_x = (int)img->columns;
    size_y = (int)img->rows;
    
    while (i < size_y) {
        j=0;
        while (j < size_x) {
            string_img[(size_x * i) + j] = (char)px[(size_x * i) + j].green;
            j++;
        }
        i++;
    }
    temp_string_img = otsu_th(size_x, size_y, string_img, c);
    
    img = ConstituteImage(size_x, size_y, "I", CharPixel, temp_string_img, &exception);
    return (img);
    
}

Image *get_mask_image(Image *img)
{
    
    
    
    return (img);
}

void		apply_histogram(PixelPacket *px, int width, int height, unsigned long quantum_depth)
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
                pixel_value = mapUnsignedValue(quantum_depth, 8, pixel_value);
            }
            pixel_value = (pixel_value >= 255 ? 254 : pixel_value);
            
            if (QuantumDepth != 8 && quantum_depth == QuantumDepth){
                pixel_value = mapUnsignedValue(8, quantum_depth, pixel_value);
            }
            px[(width * i) + j].red = px[(width * i) + j].green = px[(width * i) + j].blue = (unsigned char) pixel_value;
            j++;
        }
        i++;
    }
}

Image       *dump_his_image(Image *img, int *histogram, c_image_args *a)
{
    PixelPacket     *px;
    
    if ((px = GetImagePixels(img, 0, 0, a->width, a->height)) == NULL)	{
		DestroyImage(img);
		return 0;
	}
    apply_histogram(px, a->width, a->height, img->depth);
    SyncImagePixels(img);
    return img;
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

void readfiles(char *path)
{
    c_histogram *c;
    c_image_args *a;
    c_threshold *t;
    
    Image           *img;
    int             *histogram;
    DIR             *FD;
    struct dirent   *in_file;
    char            *imagePath;
    
    c = malloc(sizeof(*c));
    a = malloc(sizeof(*a));
    t = malloc(sizeof(*t));
    c->first_his_val = NULL;
    c->sec_his_val = NULL;
    c->final_his_val = NULL;
    t->max_threshold = 0;

    if (NULL == (FD = opendir(path)))
    {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        return;
    }
    
    while ((in_file = readdir(FD)))
    {
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))
            continue;
        //APPLE Dir ISSUE
        if (!strcmp (in_file->d_name, ".DS_Store"))
            continue;
        
        asprintf(&imagePath, "%s%s", path, in_file->d_name);
        
        img = get_image_from_path(imagePath);
        //img = get_grayscale_image(img);
        
        printf("%s\n", in_file->d_name);
        
        img = get_threshold_image(img, t);
        //img = crop_image(img,imagePath);
        //img = transparent_image(img);
        //histogram = create_histogram(img, a);
        //dump_histogram(histogram, FILE_HISTOGRAM_OUTPUT, c, in_file->d_name, "txt");
        //look_through_histogram(histogram, c);
        //img = dump_his_image(img, histogram, a);
        //img = get_area(img);
        dump_image(img, FILE_THRESHOD_OUTPUT, in_file->d_name , "jpg");
        
    }
    closedir(FD);
    return;
}
