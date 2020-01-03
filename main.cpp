#include <iostream>

#include <dos.h>
#include <stdio.h>

#include "graphics.h"

#include "Constants.h"
#include "Controls.h"
#include "InfoBox.h"

using namespace std;

int main()
{
    int statusCode = initwindow(WIDTH, HEIGHT, "Conqueror Commander");
    if (statusCode != -1)
    {
        CreateUserInterface();

        while (!kbhit())
        {
        }

        cleardevice();
        closegraph();
        restorecrtmode();
    }
    return 0;
}
