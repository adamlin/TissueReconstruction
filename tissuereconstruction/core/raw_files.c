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
 ** Remember to make z plane as Horizontal and x, y planes as vertical
 ** so that we will be able to retack images in right direction.
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
    DIR           *FD = NULL;
    struct dirent *in_file;
    char          *imagePath;
    
    // loop over image stack - in order //
    if (NULL == (FD = opendir(dir)))
    {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        free(FD);
        return;
    }
    
    while ((in_file = readdir(FD)))
    {
        printf("Processing %s\n", in_file->d_name);
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))
            continue;
        //APPLE Dir ISSUE
        if (!strcmp (in_file->d_name, ".DS_Store"))
            continue;
        
        asprintf(&imagePath, "%s/%s", dir, in_file->d_name);
        appendImageBytesToRaw(fd, imagePath);
    }
    closedir(FD);
 
    
    // loop over image stack - strange behaviour (revise order) //
    /*
     int n;
     struct dirent **namelist;
    
     n = scandir(dir, &namelist, 0, alphasort);
     // error scanning directory
     if (n < 0) {
        perror("scandir");
        exit(-1);
     }
     */
    /*
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
    */
}

// TODO: assign dimension value to h->sizes
void    dumpStackIntoRawFile(char *file, char *out_file){
    char    *filePath;
    int     fd = -1;     //open || create file for binary append
    
    if ((fd = open(out_file, (O_CREAT | O_TRUNC | O_RDWR), 0666)) == -1) {
        fprintf(stderr, "Open Failed\n");
        close(fd);
        return;
    }
    
    c_raw_header    *h = malloc(sizeof(*h));
    
    h->dim_nb = 3;
    h->type = 1;
    h->sizes        = malloc(h->dim_nb * sizeof(*h->sizes));
    h->start        = malloc(h->dim_nb * sizeof(*h->start));
    h->slice_size   = malloc(h->dim_nb * sizeof(*h->slice_size));
    h->dim_offset   = malloc(h->dim_nb * sizeof(*h->dim_offset));
    h->steps        = malloc(h->dim_nb * sizeof(*h->steps));
    
    h->sizes[0] = 956;
    h->sizes[1] = 550;
    h->sizes[2] = 450;
    h->slice_max = h->sizes[0] * h->sizes[1] * h->sizes[2];
    
    
    for (int i = 0 ; i < h->dim_nb; i ++) {
        h->steps[i] = 1.0;
        h->start[i] = (h->sizes[i] / 2);
        h->start[i] *= -1;
        h->slice_size[i] = 1;
        int j = 0;
        while (j < h->dim_nb)
        {
            if (j != i)
                h->slice_size[i] *= h->sizes[j];
            j++;
        }
        
    }
    
    h->dim_offset[0] = 0;
    
    for (int i = 0; i < h->dim_nb; i++) {
        h->dim_offset[i] = (unsigned long long)(h->dim_offset[i - 1] + (unsigned long long)((unsigned long long)h->slice_size[i - 1] * (unsigned long long)h->sizes[i - 1]));
    }
    
    char head[4096];
    char pre_head[50];
    memset(pre_head, '\0', 50);
    memset(head, '\0', 4096);
    int len = 0;
    
    
    /*  first dump header    */
    sprintf(head, "%i|%i:%i:%i|%g:%g:%g|%g:%g:%g|%s|%s|%s|%c|%c|%c|%i:%i:%i|%i|%llu:%llu:%llu|%i|",
            h->dim_nb,
            h->sizes[0], h->sizes[1], h->sizes[2],
            h->start[0], h->start[1], h->start[2],
            h->steps[0], h->steps[1], h->steps[2],
            "xspace","yspace","zspace", //h->dim_name[i]
            'x','y','z',                //h->dim_name[i][j]
            h->slice_size[0], h->slice_size[1], h->slice_size[2],
            h->slice_max,
            (unsigned long long)h->dim_offset[0], (unsigned long long)h->dim_offset[1],(unsigned long long)h->dim_offset[2], h->type);
    len = (int)strlen(head);
    sprintf(pre_head, "@IaMraW@|%i|", len);
    write(fd, pre_head, strlen(pre_head));
    write(fd, head, len);
    
    // then dump all dimensions
    for (int i = 0 ; i < h->dim_nb; i++) {
        if (i == 0) {
            asprintf(&filePath, "%s%s", file, "x");
            dumpDirectoryContentsIntoRawFile(filePath, fd);
        }
        if (i == 1) {
            asprintf(&filePath, "%s%s", file, "y");
            dumpDirectoryContentsIntoRawFile(filePath, fd);
        }
        if (i == 2) {
            asprintf(&filePath, "%s%s", file, "z");
            dumpDirectoryContentsIntoRawFile(filePath, fd);
        }
    }
    free(filePath);
    free(h);
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


