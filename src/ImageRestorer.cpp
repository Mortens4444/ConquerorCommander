#include "graphics.h"

#include "ClickHelper.h"
#include "ImageRestorer.h"

ImageRestorer::ImageRestorer(int left, int top, int right, int bottom)
{
    this->left = left;
    this->top = top;
    this->right = right;
    this->bottom = bottom;
    ptr = malloc(imagesize(left, top, right, bottom));
    getimage(left, top, right, bottom, ptr);
}

ImageRestorer::~ImageRestorer()
{
    Dispose();
}

void ImageRestorer::Dispose()
{
    if (ptr != NULL)
    {
        free(ptr);
    }
}

void ImageRestorer::WaitForAction()
{
    while (!kbhit())
    {
        if (ClickHelper::TestLeftMouseClickInArea(left, top, right, bottom))
        {
            break;
        }
    }
    while (kbhit()) getch();
    putimage(left, top, ptr, COPY_PUT);
}
