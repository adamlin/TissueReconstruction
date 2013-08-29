//
//  raw_files.h
//  max_contrast
//
//  Created by Adam Lin on 27/08/13.
//  Copyright (c) 2013 Adam Lin. All rights reserved.
//

#ifndef max_contrast_raw_files_h
#define max_contrast_raw_files_h

#include "max_contrast.h"

struct		s_raw_header
{
    int                     dim_nb;
    unsigned int            *sizes;
    double                  *start;
    double                  *steps;
    char                    **dim_name;
    unsigned long long int	*dim_offset;
    unsigned int            *slice_size;
    unsigned int            slice_max;
    int                     type;

};

/*      rawfiles.c      */
void    appendImageBytesToRaw(int fd, char * file);
void    dumpDirectoryContentsIntoRawFile(char *dir, int fd);
void    dumpStackIntoRawFile(char *file, char *out_file);
int		check_raw(char *path);

#endif
