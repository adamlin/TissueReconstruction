TissueReconstruction
====================

TissueReconstruction is a new technique for reconstruction of serial histological sections from mouse brains.
TissueReconstruction has been written in C and it wrap graphicsmagick and ITK with new functionalities to reconstruct brain tissue images. 
<BR/>
<IMG SRC="https://dl.dropboxusercontent.com/u/24447938/restacking.png" ALT="restocking1" WIDTH=260 HEIGHT=200>
<IMG SRC="https://dl.dropboxusercontent.com/u/24447938/restackingWithPlanes.png" ALT="restocking1" WIDTH=330 HEIGHT=200>
<BR/>
Fig. 1. Creating a 3D histological volume by registering 2D image slices. The order of slice is related to the reference slice (Sref). The registration for each slice was obtained by concatenating the individual transformations after Si-ref independent alignments where i is the number of slices.
## Environment
TissueReconstruction is currently developed through Mac Xcode. The main coding directory is in the /tissuereconstruction/. Other directories are system generated for building active architecture.
Building Settings in Xcode:

    Install libraries using macport: graphicmagick, ITK (Insight Segmentation and Registration Toolkit)
    Architectures: 64 bit Intel
    Complier for C: LLVM compiler 4.1
    Header Search Paths: /opt/local/include/GraphicsMagick/
    Library Search Paths: /opt/local/lib/ /opt/local/bin/ /usr/local/opt/

Build Phases in Xcode:

    Include Link Binary With Libraries: libGraphicsMagick.3.dylib

The reconstruction functions are located in /tissuereconstruction/src/restacking.c under the name of: 

    restocking (FILE_PATH, OUTPUT_PATH)  =>  image restocking

The image processing functions are located in /tissuereconstruction/src/process_image.c under the name of: 

    image_processing (FILE_PATH, OUTPUT_PATH) => max contrast, masking, deconstruction, resizing …….

**** Please also modify FILE_PATHS in /tissuereconstruction/header/max_contrast.h to locate current brain tissue files

## Reconstruction Images Example:
<IMG SRC="https://dl.dropboxusercontent.com/u/24447938/xz_image_reduceNoice_black.jpg" ALT="restocking1" WIDTH=300 HEIGHT=150> 
<IMG SRC="https://dl.dropboxusercontent.com/u/24447938/yz_image_reduceNoice_black.jpg" ALT="restocking1" WIDTH=300 HEIGHT=150>
<BR/> 
Fig. 2. Alignment of histological slices. To illustrate the alignment of slices through the image processing, 
a 3D stack of unaligned and aligned tissue images are presented in the horizontal and sagittal plane. The 
geometry and smoothness of the brain shape was improved. First image is x-z dimension on y 700 deep height and second 
image is y-z dimension on x 850 deep width.
## Compilation

This software can be compiled under Linux or Mac OSX. 

## Installation (Linux)

To be able to run the program in Linux, please make sure you have installed "GraphicsMagick" lib and add-on library "libudev" in your system. 
More information on installation instructions can be found in the following files: (http://www.graphicsmagick.org/INSTALL-unix.html)

After completed installation, build the Makefile in (TissueReconstruction -> tissueReconstruction -> make). The built program is called "TissueReconstruction". It required two parameters to do the job. Example:  

    ./TissueReconstruction restacking blockface_file_dic restocked_file_dic
    
(available commands: restacking, processing and raw)

## Extension

This project is an extension of TissueStack (http://tissuestack.com) research for the image processing. To abe able to show the final reconstructed 3D image results, it is possible to build a RAW file for Tissuestack uses. 
The RAW file compilation is located in /tissuereconstruction/src/raw_files.c under the name of:

    dumpStackIntoRawFile(RESTOCKED_FILE_PATH, FILE_RAW_PATH) => dump reconstructed file to a RAW format

PS: reconstructed image file must contents x, y, z planes
