#ifndef CONTROLS_H
#define CONTROLS_H

#include <functional>

#include "Menu.h"

void CreateForm();

void CreateLabel(int, int, String);

void CreateButton(int left, int top, int right, int bottom, String text, std::function<void(void)> callback);

void CreatePanel(int, int, int, int);

void CreateTextBox(int, int, int, int);

void CreateMainMenu(int, int, MenuItem*);

void CreateStatusBar(int, int, ...);

void CreateBox(int, int, int, int);

void CreateUserInterface();

#endif
