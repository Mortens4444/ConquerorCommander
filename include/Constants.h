#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#define KILOBYTE 1024
#define MEGABYTE KILOBYTE * KILOBYTE
#define GIGABYTE MEGABYTE * KILOBYTE

#define NOT_SELECTED -1

#define WIDTH 1590
#define HEIGHT 840
#define MIDDLE WIDTH / 2
#define VERTICAL_MIDDLE HEIGHT / 2

#define TITLE_HEIGHT 30
#define MENU_HEIGHT 35
#define FRAME 3
#define MARGIN 4

#define FONT SIMPLEX_FONT
#define FONT_SIZE 1

#define EXTRA_WIDTH 30
#define HALF_OF_EXTRA_WIDTH EXTRA_WIDTH / 2

#define MAX_PATH_LENGTH 255

typedef char* String;

typedef void VoidResultNoParams();

typedef int IntResultIntParam(int);

typedef VoidResultNoParams* ProcedureCallback;

typedef IntResultIntParam* FunctionCallback;

#endif
