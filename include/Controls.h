#ifndef CONTROLS_H
#define CONTROLS_H

#include <functional>

#include "Menu.h"

void CreateForm();

void CreateLabel(int, int, String);

//std::function<void(const ImageRestorer&, void)>
//void CreateButton(int left, int top, int right, int bottom, String text, std::function<void(ImageRestorer const &, void)> callback);
//void CreateButton(int left, int top, int right, int bottom, String text, std::function<void()> callback);
void CreateButton(int, int, int, int, String, ProcedureCallback = []() { });

void CreatePanel(int, int, int, int);

void CreateTextBox(int, int, int, int);

void CreateMainMenu(int, int, MenuItem*);

void CreateStatusBar(int, int, ...);

void CreateBox(int, int, int, int);

void CreateUserInterface();

#endif
