TissueReconstruction
====================

TissueReconstruction is a new technique for reconstruction of serial histological sections from mouse brains.
TissueReconstruction has been written in C and it wrap graphicsmagick and ITK with new functionalities to reconstruct brain tissue images. 
<BR/>
<IMG SRC="https://dl.dropboxusercontent.com/u/24447938/slides.jpg" ALT="restocking1" WIDTH=260 HEIGHT=200 ALIGN="center">
<BR/>
Fig. 1. Creating a 3D image volume by registering 2D histological slices (x, y, z). The slice order relative to the fixed 
reference slice.
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

The reconstruction functions are located in /tissuereconstruction/function.c under the name of: 

    restocking (FILE_PATH, OUTPUT_PATH)  =>  image restocking

    image_processing (FILE_PATH, OUTPUT_PATH) => max contrast, masking, deconstruction, resizing …….

    Please also modify FILE_PATHS in /tissuereconstruction/header/max_contrast.h to locate current brain tissue files
## Reconstruction Images Example:
<IMG SRC="https://dl.dropboxusercontent.com/u/24447938/xz_image_reduceNoice_black.jpg" ALT="restocking1" WIDTH=300 HEIGHT=150> 
<IMG SRC="https://dl.dropboxusercontent.com/u/24447938/yz_image_reduceNoice_black.jpg" ALT="restocking1" WIDTH=300 HEIGHT=150>
<BR/> 
Fig. 2. Alignment of histological slices. To illustrate the alignment of slices through the image processing, 
a 3D stack of unaligned and aligned tissue images are presented in the horizontal and sagittal plane. The 
geometry and smoothness of the brain shape was improved. First image is x-z dimension on y 700 deep height and second 
image is y-z dimension on x 850 deep width.
## Compilation

This software can be compiled under Linux or Mac OSX; however, the current development has only been tested in Mac OSX.
To be able to run the program in Linux, please create a Make file.
