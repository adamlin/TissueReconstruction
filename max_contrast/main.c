#define _GNU_SOURCE

#include "max_contrast.h"

int		main(int argc, char *argv[])
{
    InitializeMagick("/");
    restocking(FILE_PATH);
    //image_processing(FILE_PATH);
    DestroyMagick();
    return (0);
}


