#include <stdlib.h>

#include "Menu.h"

MenuItem FileMenu[] =
{
    { (char*)"Attributes",	NULL,   1 },
    { (char*)"Print",	    NULL,	2 }
};

MenuItem EditMenu[] =
{
    { (char*)"Copy",		    NULL,   3 },
    { (char*)"Delete",		    NULL,   4 },
    { (char*)"Move/Rename",    NULL,   5 },
    { (char*)"Run",	        NULL,   6 }
};

MenuItem HelpMenu[] =
{
    { (char*)"Help",	NULL,   7 },
    { (char*)"About",  NULL,	8 }
};

Menu menus[] =
{
    /* x	y   xm	ym  NumberOfMenuItems MenuItems LastSelectedMenuItemIndex */
    {  0,   0,  500,  50,    sizeof(FileMenu) / sizeof(MenuItem), FileMenu, NOT_SELECTED },
    {  100,	100,  500,  100,    sizeof(EditMenu) / sizeof(MenuItem), EditMenu, NOT_SELECTED },
    {  200,	200,  500,  150,    sizeof(HelpMenu) / sizeof(MenuItem), HelpMenu, NOT_SELECTED }
};
