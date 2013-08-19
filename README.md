TissueReconstruction
====================

TissueReconstruction is a new technique for reconstruction of serial histological sections from mouse brains.
TissueReconstruction has been written in C and it wrap graphicsmagick and ITK with new functionalities to reconstruct brain tissue images. 

## Environment
TissueReconstruction is currently developed through Mac Xcode. The main coding directory is in the /max_contrast/. Other directories are system generated for building active architecture.

Building Settings in Xcode:

    Install libraries using macport: graphicmagick, ITK (Insight Segmentation and Registration Toolkit)
    Architectures: 64 bit Intel
    Complier for C: LLVM compiler 4.1
    Header Search Paths: /opt/local/include/GraphicsMagick/
    Library Search Paths: /opt/local/lib/ /opt/local/bin/ /usr/local/opt/

Build Phases in Xcode:

    Include Link Binary With Libraries: libGraphicsMagick.3.dylib

The reconstruction functions are located in /max_contrast/function.c under the name of: 

    restocking (FILE_PATH)  =>  image restocking

    image_processing (FILE_PATH) => max contrast, masking, deconstruction, resizing …….

## Reconstruction Images Example:
<IMG SRC="https://dl.dropboxusercontent.com/u/24447938/xz_image_reduceNoice_black.jpg" ALT="restocking1" WIDTH=400 HEIGHT=200> 
<IMG SRC="https://dl.dropboxusercontent.com/u/24447938/yz_image_reduceNoice_black.jpg" ALT="restocking1" WIDTH=400 HEIGHT=200>
<BR/> left image is x-z dimension on y 700 deep height and right image is y-z dimension on x 850 deep width.
## Compilation

This software can be compiled under Linux or Mac OSX; however, the current development has only been tested in Mac OSX.
To be able to run the program in Linux, please create a Make file.
