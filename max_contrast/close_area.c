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

/*      close_area header         */

struct      pixel {
    struct pixel * previous;
    
    unsigned long int x;
    unsigned long int y;
    
    void (*destroyMe) (struct pixel *);
};

struct      pixel_stack {
    struct pixel * top;
    
    struct pixel * (*pop)(struct pixel_stack *);
    void (*push) (struct pixel_stack *, unsigned long int x, unsigned long int y);
};


struct      pixel *pop(struct pixel_stack * this) {
    if (this == NULL || this->top == NULL) return NULL;
    
    // if we have a previous one, we have to adjust the top when popping
    struct pixel * top = this->top;
    struct pixel * prev = this->top->previous == NULL ? NULL : this->top->previous;
    top->previous = NULL;
    this->top = prev;
    
    return top;
}

void destroyPixel(struct pixel * px) {
    if (px == NULL) return;
    
    px->previous = NULL;
    free(px);
    px = NULL;
}

void initializePixel(struct pixel * px, unsigned long int x, unsigned long int y) {
    if (px == NULL) return;
    
    px->x = x;
    px->y = y;
    px->previous = NULL;
    px->destroyMe = destroyPixel;
}

void push(struct pixel_stack * this, unsigned long int x, unsigned long int y) {
    if (this == NULL) return;
    
    // construct new pixel with new coordinate pair
    struct pixel * newPixel = NULL;
    newPixel = malloc(sizeof(*newPixel));
    initializePixel(newPixel, x, y);
    // check if we have a predecessor on the stack
    
    if (this->top == NULL) // no! this is the start of the stack
        this->top = newPixel;
    else { // yes! we will have to make this the new top element of the stack
        struct pixel * tmp = this->top;
        newPixel->previous = tmp;
        this->top = newPixel;
    }
}

void initializePixelStack(struct pixel_stack * stack) {
    if (stack == NULL)  return;
    
    stack->top = NULL;
    stack->pop = pop;
    stack->push = push;
}

/*          end of close_area header        */
/*          function go under               */
void apply_close_area(PixelPacket * read, PixelPacket * write, unsigned long int width, unsigned long int height) {
    
    // create a new stack and initialize it
    struct pixel_stack * my_stack = NULL;
    my_stack = malloc(sizeof(*my_stack));
    initializePixelStack(my_stack);
    
    // create our first start pixel, the middle of the image and add it to the stack
    my_stack->push(my_stack, width /2 , height / 2);
    
    //short theVeryFirstPixel = 1;
    
    // now enter the fill loop which we only exit once the stack has no more pixels on it
    while (1) {
        struct pixel * top_of_stack = my_stack->pop(my_stack);
        
        if (top_of_stack == NULL) // exit !
            break;
        
        unsigned short value = (unsigned short) read[(width * top_of_stack->y) + top_of_stack->x].red;
        
        // problems with the middle point not being empty
        /*
        if (theVeryFirstPixel == 1 && value != EMPTY_SPACE_COLOR) {
            // move to first white
            if (top_of_stack->x > 0) // pixel LEFT
                for (int i = 0 ; i < width / 2; i++) {
                    value = (unsigned short) read[(width * top_of_stack->y) + top_of_stack->x - i].red;
                    if(value == EMPTY_SPACE_COLOR){
                        theVeryFirstPixel = 0;
                        return;
                    }
                }
        }
        */
        // regular fill        
        if (value == EMPTY_SPACE_COLOR) {
            if (width * top_of_stack->y + top_of_stack->x > width * height) {
                printf("%li:%li\n", top_of_stack->y, top_of_stack->x);
                break;
            }
            read[width * top_of_stack->y + top_of_stack->x].red =
            write[width * top_of_stack->y + top_of_stack->x].red =
            write[width * top_of_stack->y + top_of_stack->x].green =
            write[width * top_of_stack->y + top_of_stack->x].blue = FILL_COLOR;
            
            // now, add the neighborhood onto the stack if we are not out of bounds
            if (top_of_stack->x > 0) // pixel LEFT
                my_stack->push(my_stack, top_of_stack->x - 1, top_of_stack->y);
            if (top_of_stack->x < width) // pixel RIGHT
                my_stack->push(my_stack, top_of_stack->x + 1, top_of_stack->y);
            if (top_of_stack->y < height) // pixel TOP
                my_stack->push(my_stack, top_of_stack->x, top_of_stack->y + 1);
            if (top_of_stack->y > 0) // pixel BOTTOM
                my_stack->push(my_stack, top_of_stack->x, top_of_stack->y - 1);
        }
        
        // destroy the popped pixel
        top_of_stack->destroyMe(top_of_stack);
    }
    
    // destroy the stack
    free(my_stack);
}

Image    *close_area(Image *image)
{
    Image *newImage = NULL;
    ImageInfo *imageInfo, * newImageInfo = NULL;
    ExceptionInfo exception;
    
    InitializeMagick(NULL);
    imageInfo=CloneImageInfo(0);
    newImageInfo=CloneImageInfo(0);
    GetExceptionInfo(&exception);

    if (image == NULL) { // CANNOT READ IMAGE FILE
        CatchException(&exception);
        printf("Error opening image\n");
    } else {
        newImage = AllocateImage(newImageInfo);
        newImage->rows = image->rows;
        newImage->columns = image->columns;
        SetImagePixels(newImage, 0, 0, newImage->columns, newImage->rows);
        
        PixelPacket * pixelsRead = NULL;
        PixelPacket * pixelsWritten = NULL;
        unsigned long int width = image->columns;
        unsigned long int height = image->rows;
        
        // read source image
        if ((pixelsRead = GetImagePixels(image, 0, 0, width, height)) == NULL)
            printf("Error getting pixels\n");
        
        // create destination pixels
        if ((pixelsWritten = GetImagePixels(newImage, 0, 0, width, height)) == NULL)
            printf("Error getting pixels\n");
        
        // call the flood fill algorithm
        apply_close_area(pixelsRead, pixelsWritten, width, height);
        SyncImagePixels(newImage);
        
        // CLEAN UP
        DestroyImage(image);
        DestroyImageInfo(imageInfo);
    }
    return (newImage);
}