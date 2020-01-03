#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "Constants.h"

typedef struct
{
    String Text;
	FunctionCallback Callback;
	int Parameter;
} MenuItem;

typedef struct
{
	int X;
	int Y;
	int xm;
	int ym;
	int NumberOfMenuItems;
	MenuItem *MenuItems;
	int LastSelectedMenuItemIndex;
} Menu;

int CallbackImplementation(int);

#endif
