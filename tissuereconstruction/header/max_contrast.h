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
 ** -- need following libraries to make this reconstruction works --
 ** "graphicmagick"
 ** "ITK" (Insight Segmentation and Registration Toolkit)
 ** "Dcraw"
 */

#ifndef max_contrast_max_contrast_h
#define max_contrast_max_contrast_h

#ifndef __APPLE__

#include <sys/prctl.h>
#include <libudev.h>
#include <linux/input.h>

#endif

#include <stdio.h>
#include <stdlib.h>
#include <magick/api.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>

#include "typedef.h"

struct		s_histogram
{
    char    *first_his_val;
    char    *sec_his_val;
    char    *final_his_val;
    
};

struct      s_image_args
{
    int     size;
    int     width;
    int     height;
};

struct      s_threshold {
    int     max_threshold;
};

struct      s_image{
    int rows;
    int cols;
    unsigned char *data;
    unsigned char *label_map;
    int first_position;
    int final_position;
};

struct      s_zdimension{
    int count_slide;
    int pixel_count_slide_end;
    int pixel_count_slide_start;
    
    int xz_count_slide;
    int xz_pixel_count_slide_end;
    int xz_pixel_count_slide_start;
    
    
    int angel_count_slide;
    int angel_pixel_count_slide_end;
    int angel_pixel_count_slide_start;
    
    int pixel_width;
    int pixel_height;
    int x_dec;
    int y_dec;
    int z_dec;
    int p_dec;
    int ax_dec;
    int ay_dec;
    
    unsigned char *yz_pixel_map;    
    unsigned char *xz_pixel_map;
    unsigned char *angel_pixel_map;
};

struct      s_cross_correlation{
    double ssum;
    double dif_sum;
};

/*      main           */

/*
On the nef file format, I have edit
    /opt/local/lib/GraphicsMagick-1.3.18/config/delegates.mgk
for dcraw decoding with command="/opt/local/bin/"
*/

#define FILE_PATH           "/Users/adam/Documents/blockface_allbrains/brain_3_gray/"
#define FILE_GRAY_OUTPUT    "/Users/adam/Documents/blockface_allbrains/gray_his/"
#define FILE_HISTOGRAM      "/Users/adam/Documents/blockface_allbrains/gray_his/"
#define FILE_CON_OUTPUT     "/Users/adam/Documents/blockface_allbrains/zConOutput/brain_3_complele/"
#define FILE_MAX_OUTPUT     "/Users/adam/Documents/blockface_allbrains/zMaxOutput/"
#define FILE_HISTOGRAM_OUTPUT "/Users/adam/Documents/blockface_allbrains/gray_his_txt/"

#define FILE_XYZ            "/Users/adam/Documents/blockface_allbrains/zConOutput/restacked_brain_3/"
#define FILE_OUT_RAW        "/Users/adam/Documents/blockface_allbrains/zConOutput/restacked_brain_3/restack17.raw"

/*      for home used   */
/*
#define FILE_PATH "/Users/AdamLin/Documents/CAI/brain_image/"
#define FILE_GRAY_OUTPUT "/Users/AdamLin/Documents/CAI/brain_image_gray/"
#define FILE_THRESHOD_OUTPUT "/Users/AdamLin/Documents/CAI/brain_image_threshold/"
#define FILE_HISTOGRAM_OUTPUT "/Users/AdamLin/Documents/CAI/brain_image_histogram/"
*/

/*      functions.c     */
void        restocking(char *path, int start_slide, int end_slide, char *output_path);
void        image_processing(char *path, char *output_path);
void        image_correction(char *path, char *output_path);

/*      max_contract.c  */
Image		*get_image_from_path(char *path);
Image       *transparent_image(Image *img);
Image       *crop_image(Image *img, char *path, int image_width, int image_height, int width_offset, int height_offset);
Image		*get_grayscale_image(Image *img);
Image       *get_threshold_image(Image *img, c_threshold *c);
Image       *maximum_contrast_image(Image *img);
Image       *blur_image(Image *img, double radius, double range);
Image       *subtract_image(Image *canvas_image, Image *img);
Image       *contrast_image(Image *img, int contrast);
Image       *edge_image(Image *img);
Image       *unsharp_mask_image(Image *img, double sigma, double amount);
Image       *dump_his_image(Image *img, int *histogram, c_image_args *a);
Image       *deconstruction(Image *img);
Image       *reduce_noice(Image *img);
Image       *resize_image(Image *img, int re_columns, int re_rows, FilterTypes filter, int blur);
Image       *rotate_image(Image *img, int degrees);
int   		dump_image(Image *img, char *root_path, char *name, char *image_type);

/*      histogram.c     */
int		*create_histogram(Image *img, c_image_args *a);
void	dump_histogram(int *histogram, char *path, c_histogram *c, char *file_name, char *file_type);
//void	apply_histogram(PixelPacket *px, int width, int height, unsigned long quantum_depth);
void	apply_histogram(PixelPacket *px, unsigned char min, unsigned char max,
                            unsigned char dataset_min, unsigned char dataset_max,
                            int width, int height, unsigned long quantum_depth);
void    *look_through_histogram(int *histogram, c_histogram *c);

/*      otsu_th.c       */
char    *otsu_th(int x_size1, int y_size1, char *image1, c_threshold *c);
#define GRAYLEVEL 256
#define MAX_BRIGHTNESS 255

/*      area.c          */
Image   *get_area(Image *img);
#define WHITE 255
#define BLACK 0
#define OUTLINE 30
#define COLORED 200

/*      edgelap.c       */
Image   *edgelap(Image *img, char *path);
#define FILE_OUTPUT "/Users/AdamLin/Documents/CAI/brain_image_output/brain.jpg"


/*      area.c          */
Image   *get_blur_maked(Image *img, c_image *c);
Image   *crop_area_selected(Image *img);
Image   *crop_masked_final_image(Image *img, Image *mask_img);
Image   *slant_line_detection(Image *img);


#define TRUE 1
#define FALSE 0

/*      close_area.c    */
Image    *close_area(Image *image);

#define IS_8BIT_DEPTH ((QuantumDepth == 8) ? 1 : 0)
#define EMPTY_SPACE_COLOR ( (IS_8BIT_DEPTH == 1) ? 255 : (int)(((double)255 / 255) * 65535) )
#define FILL_COLOR ( (IS_8BIT_DEPTH == 1) ? 200 : (int)(((double)200 / 255) * 65535) )

/*      reconstruction.c    */
unsigned char   *yz_reconstruction(Image *img, c_zdimension *z, int i_px);
unsigned char   *xz_reconstruction(Image *img, c_zdimension *z, int i_px);
unsigned char   *angel_reconstruction(Image *img, c_zdimension *z, int i_px, double angel);
Image           *get_avg_pixel(Image *img, Image *first_img, Image *second_img, Image *third_img);
Image           *yz_final_construct(c_zdimension *z);
Image           *xz_final_construct(c_zdimension *z);
Image           *angel_final_construct(c_zdimension *z);

#define PI 3.14159265
#define DR 0.0174532925

/*      corss_correlation.c    */
double          *cross_correlation(Image *first_img, Image *second_img, c_cross_correlation *b);
#define SQR2(x) ((x) * (x))



#endif

