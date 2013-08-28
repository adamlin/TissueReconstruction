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
#include "raw_files.h"


void    appendImageBytesToRaw(int fd, char * file) {
    Image *img = get_image_from_path(file);
    
    unsigned char   *pixel_map = NULL;
    size_t pixel_map_size = img->columns * img->rows * sizeof(*pixel_map);
    pixel_map = malloc(pixel_map_size);
    unsigned int    x = 0;
    unsigned int    y = 0;
    int             width = (int)img->columns;
    int             height = (int)img->rows;
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
    write(fd, pixel_map, pixel_map_size);
    free(pixel_map);
    DestroyImage(img);
    
}

void    dumpDirectoryContentsIntoRawFile(char *dir, int fd){
    struct dirent **namelist;
    char          *imagePath;
    int n;
    
    n = scandir(dir, &namelist, 0, alphasort);
    // error scanning directory
    if (n < 0) {
        perror("scandir");
        exit(-1);
    }
    
    // loop over image stack
    while(n--) {
        printf("Processing %s\n", namelist[n]->d_name);
        
        if (!strcmp (namelist[n]->d_name, "."))
            continue;
        if (!strcmp (namelist[n]->d_name, ".."))
            continue;
        //APPLE Dir ISSUE
        if (!strcmp (namelist[n]->d_name, ".DS_Store"))
            continue;
        
        asprintf(&imagePath, "%s/%s", dir, namelist[n]->d_name);
        appendImageBytesToRaw(fd, imagePath);
        free(namelist[n]);
    }
    free(namelist);
}

void    dumpStackIntoRawFile(char *file, char *out_file){
    char    *filePath;
    int     x = 1200, y = 1600, z = 840;
    int     fd = -1;     //open || create file for binary append
    
    if ((fd = open(out_file, (O_CREAT | O_TRUNC | O_RDWR), 0666)) == -1) {
        fprintf(stderr, "Open Failed\n");
        close(fd);
        return;
    }
    
    char head[4096];
    char pre_head[50];
    memset(pre_head, '\0', 50);
    memset(head, '\0', 4096);
    int len = 0;
    
    /*  temp math for slides    */
    int slide_size_x = y * z;
    int slide_size_y = x * z;
    int slide_size_z = x * y;
    double start_x = (x / 2) * (-1);
    double start_y = (y / 2) * (-1);
    double start_z = (z / 2) * (-1);
    unsigned long long slide_all = x * y * z;
    unsigned long long slider_all_n = slide_all * 2;
    
    /*  first dump header    */
    sprintf(head, "%i|%i:%i:%i|%g:%g:%g|%g:%g:%g|%s|%s|%s|%c|%c|%c|%i:%i:%i|%i|%llu:%llu:%llu|%i|",
            3,                          //h->dim_nb,
            z, y, x,                    //h->sizes[0], h->sizes[1], h->sizes[2],
            start_z, start_y, start_x,  //h->start[0], h->start[1], h->start[2],
            1.0,1.0,1.0,                //h->steps[0], h->steps[1], h->steps[2],
            "zspace","yspace","xspace", //h->dim_name[0], h->dim_name[1], h->dim_name[2],
            'z','y','x',                //h->dim_name[0][0], h->dim_name[1][0], h->dim_name[2][0],
            slide_size_z,slide_size_y,slide_size_x,     //h->slice_size[0], h->slice_size[1], h->slice_size[2],
            (int)slide_all,                 //h->slice_max,
            (unsigned long long)0, (unsigned long long)slide_all, (unsigned long long)slider_all_n, 2);
                                        //(unsigned long long)h->dim_offset[0], (unsigned long long)h->dim_offset[1],
                                        //(unsigned long long)h->dim_offset[2], MINC);
    len = (int)strlen(head);
    sprintf(pre_head, "@IaMraW@|%i|", len);
    write(fd, pre_head, strlen(pre_head));
    write(fd, head, len);
    
    // then dump all dimensions
    for (int i = 0 ; i < 3; i++) {
        if (i == 0) {
            asprintf(&filePath, "%s%s", file, "z");
            dumpDirectoryContentsIntoRawFile(filePath, fd);
        }
        if (i == 1) {
            asprintf(&filePath, "%s%s", file, "y");
            dumpDirectoryContentsIntoRawFile(filePath, fd);
        }
        if (i == 2) {
            asprintf(&filePath, "%s%s", file, "x");
            dumpDirectoryContentsIntoRawFile(filePath, fd);
        }
    }
    free(filePath);
    close(fd);
}

int		check_raw(char *path){
    int		fd;
    char	check[10];
    
    memset(check, '\0', 9);
    if ((fd = open(path, O_RDWR)) == -1)
    {
        perror("Open ");
        return (-1);
    }
    if (read(fd, check, 8) == -1)
    {
        perror("Read ");
        return (-1);
    }
    check[9] = '\0';
    if (strcmp(check, "@IaMraW@") != 0)
        return (0);
    
    printf("%i\n", fd);
    return (fd);
}
