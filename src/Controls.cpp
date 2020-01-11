#include <thread>

#include "graphics.h"

#include "Constants.h"
#include "Controls.h"
#include "StringUtils.h"
#include "Theme.h"
//#include "InfoBox.h"

void DrawBorder(int left, int top, int right, int bottom)
{
    setcolor(Theme::BorderColor);
    rectangle(left - 1, top - 1, right + 1, bottom + 1);
}

void Draw3DBorder(int left, int top, int right, int bottom)
{
	setcolor(Theme::BorderColor);
	line(left, bottom, right, bottom);
	line(right, bottom, right, top);
	setcolor(Theme::PassiveControlBackgroundColor);
	line(right, top, left, top);
	line(left, top, left, bottom);
}

void CreateControl(int left, int top, int right, int bottom, int color)
{
    setfillstyle(SOLID_FILL, color);
    bar(left, top, right, bottom);
    DrawBorder(left, top, right, bottom);
}

void CreateForm()
{
	setcolor(Theme::ForegroundColor);
	setfillstyle(SOLID_FILL, Theme::BackgroundColor);
	floodfill(10, 10, 15);
	settextstyle(FONT, 0, FONT_SIZE);
	settextjustify(LEFT_TEXT, TOP_TEXT);
}

void CreateLabel(int left, int top, String text)
{
    setcolor(Theme::ForegroundColor);
    setbkcolor(Theme::BackgroundColor);
    outtextxy(left, top, text);
}

void CreateButton(int left, int top, int right, int bottom, String text, std::function<void(void)> callback)
{
    CreateLabel(left + HALF_OF_EXTRA_WIDTH, top + MARGIN, text);
    Draw3DBorder(left, top, right, bottom);
    std::thread clickHandlerThread(callback);
    clickHandlerThread.detach();
}

void CreatePanel(int left, int top, int right, int bottom)
{
    CreateControl(left, top, right, bottom, Theme::PassiveControlBackgroundColor);
}

void CreateTextBox(int left, int top, int right, int bottom)
{
	CreateControl(left, top, right, bottom, Theme::ActiveControlBackgroundColor);
}

void CreateMainMenu(int left, int top, MenuItem* mainMenu)
{
    CreateBox(0, top, WIDTH - 1, MENU_HEIGHT);
    int numberOfMainMenuItems = sizeof(mainMenu);
	for (int i = 0; i < numberOfMainMenuItems; i++)
    {
        CreateLabel(left, top + MARGIN, mainMenu[i].Text);
        left += StringUtils::GetTextWidth(mainMenu[i].Text);
	}
}

void CreateStatusBar(int left, int top, ...)
{
    va_list args;
    String current;

    va_start(args, top);
    while (!StringUtils::IsNullOrEmpty(current = va_arg(args, String)))
    {
        int width = StringUtils::GetTextWidth(current);
        CreateButton(left, top, left + width, top + 30, current, [] () { /*InfoBox((char*)"Hi");*/ });
        left += width + HALF_OF_EXTRA_WIDTH;
    }
    va_end(args);
}

void CreateBox(int left, int top, int right, int bottom)
{
	setfillstyle(SOLID_FILL, Theme::BackgroundColor);
	bar(left, top, right, bottom);
	Draw3DBorder(left, top, right, bottom);
}

void CreateUserInterface()
{
    MenuItem MainMenu[] =
    {
        { (String)"File", NULL, 0 },
        { (String)"Edit", NULL, 0 },
        { (String)"Help", NULL, 0 },
        { (String)"Exit", NULL, 9 }
    };

	CreateForm();
    CreateMainMenu(50, 10, MainMenu);

    int top = TITLE_HEIGHT + MENU_HEIGHT + MARGIN;
    int bottom = HEIGHT - 2 * MENU_HEIGHT - MARGIN;
	CreatePanel(MARGIN, top, MIDDLE - MARGIN, bottom);
    CreatePanel(MIDDLE + MARGIN, top, WIDTH - MARGIN - 1, bottom);
	CreateTextBox(75, HEIGHT - 2 * MENU_HEIGHT + 1, WIDTH - 5, HEIGHT - MENU_HEIGHT - 5);
    CreateStatusBar(50, HEIGHT - MENU_HEIGHT, (String)"F1 Help", (String)"F2 Menu", (String)"F3 View",
                (String)"F4 Edit", (String)"F5 Copy", (String)"F6 Move", (String)"F7 New folder",
                (String)"F8 Delete", (String)"F9 Rename", (String)"F10 Exit", NULL);
}
