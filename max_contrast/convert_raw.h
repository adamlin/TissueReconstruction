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

#ifndef max_contrast_convert_raw_h
#define max_contrast_convert_raw_h

#include <sys/stat.h>


typedef struct	s_header	t_header;

struct			s_header
{
    int                 dim_nb;
    unsigned int		*sizes;
    double              *start;
    double              *steps;
    char                **dim_name;
    unsigned long long int	*dim_offset;
    unsigned int            *slice_size;
    unsigned int            slice_max;
};


#endif /* __MINC_TOOL_CORE__ */

