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

#define _GNU_SOURCE

#include "max_contrast.h"
#include "raw_files.h"

int		main()
{
    InitializeMagick("/");
    //restocking(FILE_PATH, 0, 1600, FILE_CON_OUTPUT);
    //image_processing(FILE_PATH, FILE_MAX_OUTPUT);
    //image_correction("/Users/adam/Documents/Brain BlockFace 28.05.2013/zConOutput/restacked_brain_3/x/", FILE_MAX_OUTPUT);
    //check_raw("/Users/adam/Documents/Brain BlockFace 28.05.2013/zConOutput/restacked_brain_3/restack3.raw");
    //dumpStackIntoRawFile("/Users/adam/Documents/Brain BlockFace 28.05.2013/zConOutput/restacked_brain_3/restack16.raw");
    DestroyMagick();
    return (0);
}