#include "graphics.h"

#include "ClickHelper.h"

bool ClickHelper::TestLeftMouseClickInArea(int left, int top, int right, int bottom)
{
    if (ismouseclick(WM_LBUTTONDOWN))
    {
        int mouseX, mouseY;
        getmouseclick(WM_LBUTTONDOWN, mouseX, mouseY);
        return ((mouseX > left) && (mouseY > top) && (mouseX < right) && (mouseY < bottom));
    }
    return false;
}
