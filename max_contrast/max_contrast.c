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

Image		*get_image_from_path(char *path)
{
    ImageInfo	*image_info;
    Image		*img = NULL;
    ExceptionInfo	exception;
    
    GetExceptionInfo(&exception);
    if ((image_info = CloneImageInfo((ImageInfo *)NULL)) == NULL) {
        CatchException(&exception);
        DestroyImageInfo(image_info);
        DestroyImage(img);
        return (NULL);
    }
    
    strcpy(image_info->filename, path);
    
    if ((img = ReadImage(image_info, &exception)) == NULL)
    {
        CatchException(&exception);
        DestroyImageInfo(image_info);
        DestroyImage(img);
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
    DestroyImage(img);
    DestroyImageInfo(image_info);
    return (0);
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
    free(string_img);
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
        free(portion);
        return (NULL);
    }
    
    strcpy(image_info->filename, path);
    
    if ((img = ReadImage(image_info, &exception)) == NULL)
    {
        CatchException(&exception);
        DestroyImageInfo(image_info);
        free(portion);
        return (NULL);
    }
    tmp = img;
    
    if ((img = CropImage(img, portion, &exception)) == NULL) {
        CatchException(&exception);
        DestroyImage(tmp);
        DestroyImageInfo(image_info);
        free(portion);
        return (NULL);
    }
    
    DestroyImage(tmp);
    DestroyImageInfo(image_info);
    free(portion);
    return (img);
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

Image       *get_threshold_image(Image *img, c_threshold *c)
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
    free(temp_string_img);
    free(string_img);
    free(px);

    SyncImagePixels(img);
    return (img);
}

Image       *maximum_contrast_image(Image *img)
{
    NormalizeImage(img);
    return (img);
}

Image       *blur_image(Image *img, double radius, double range)
{
    Image *new_img;
    ExceptionInfo	exception;
    GetExceptionInfo(&exception);
    
    new_img =BlurImageChannel(img, AllChannels, radius, range, &exception);
    DestroyImage(img);
    SyncImage(new_img);
    return (new_img);
}

Image       *subtract_image(Image *canvas_image, Image *img)
{
    CompositeImage(img, CopyBlackCompositeOp, canvas_image, 0, 0); 

    //SyncImagePixels(img);
    return (img);
}

Image       *contrast_image(Image *img, int contrast)
{   
    ContrastImage(img, contrast);
    SyncImagePixels(img);
    return (img);

}

Image       *edge_image(Image *img)
{
    ExceptionInfo	exception;
    
    GetExceptionInfo(&exception);
    img = EdgeImage(img, 5, &exception);
    return (img);
}

Image       *unsharp_mask_image(Image *img, double sigma, double amount)
{
    //double radius = img->columns > img->rows ? img->rows : img->columns;
    Image *new_img = NULL;
    ExceptionInfo	exception;
    GetExceptionInfo(&exception);
     
    new_img = UnsharpMaskImage(img, 0, sigma, amount, 0.03, &exception);
    return (new_img);
}

Image       *deconstruction(Image *img){
    
    ExceptionInfo	exception;
    GetExceptionInfo(&exception);
    
    DeconstructImages(img, &exception);
    return (img);
}

Image       *resize_image(Image *img, int re_columns, int re_rows, FilterTypes filter, int blur){
    Image *new_img = NULL;
    ExceptionInfo	exception;
    GetExceptionInfo(&exception);
    
    // Filter type : http://www.graphicsmagick.org/api/types.html#filtertypes
    // Blur: The blur factor where > 1 is blurry, < 1 is sharp.
    
    new_img = ResizeImage(img, re_columns, re_rows, filter, blur, &exception);
    DestroyImage(img);
    return (new_img);
}

