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

#include "convert_raw.h"

/*
void		write_header_into_file(int fd, t_header *h)
{
    char		head[4096];
    char		lenhead[200];
    int         len;
    
    memset(head, '\0', 4096);
    sprintf(head, "%i|%i:%i:%i|%g:%g:%g|%g:%g:%g|%s|%s|%s|%c|%c|%c|%i:%i:%i|%i|%llu:%llu:%llu|%i|",
            h->dim_nb,
            h->sizes[0], h->sizes[1], h->sizes[2],
            h->start[0], h->start[1], h->start[2],
            h->steps[0], h->steps[1], h->steps[2],
            h->dim_name[0], h->dim_name[1], h->dim_name[2],
            h->dim_name[0][0], h->dim_name[1][0], h->dim_name[2][0],
            h->slice_size[0], h->slice_size[1], h->slice_size[2],
            h->slice_max,
            (unsigned long long)h->dim_offset[0], (unsigned long long)h->dim_offset[1], (unsigned long long)h->dim_offset[2], MINC); // change it later
    len = strlen(head);
    memset(lenhead, '\0', 200);
    sprintf(lenhead, "@IaMraW@|%i|", len);
    write(fd, lenhead, strlen(lenhead));
    write(fd, head, len);
}

void    *start(){
    
    
}
*/