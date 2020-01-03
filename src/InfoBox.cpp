#include "Constants.h"
#include "Controls.h"
#include "InfoBox.h"
#include "StringUtils.h"

InfoBox::InfoBox(String text, int height)
{
    int half_width = StringUtils::GetTextWidth(text) / 2;
	int half_height = height / 2;
	int left = MIDDLE - half_width;
	int top = VERTICAL_MIDDLE - half_height;
	int right = MIDDLE + half_width;
	int bottom = VERTICAL_MIDDLE + half_height;

    String buttonText = (String)"OK";
    half_width = StringUtils::GetTextWidth(buttonText) / 2;
    int buttonLeft = MIDDLE - half_width;
    int buttonTop = VERTICAL_MIDDLE + 10;
    int buttonRight = MIDDLE + half_width;
    int buttonBottom = VERTICAL_MIDDLE + 35;

    int saveLeft = GetLower(left, buttonLeft);
    int saveTop = GetLower(top, buttonTop);
    int saveRight = GetHigher(right, buttonRight);
    int saveBottom = GetHigher(bottom, buttonBottom);

    imageRestorer = new ImageRestorer(saveLeft, saveTop, saveRight, saveBottom);
    CreateBox(saveLeft, saveTop, saveRight, saveBottom);
    CreateLabel(left + HALF_OF_EXTRA_WIDTH, top + HALF_OF_EXTRA_WIDTH, text);

    CreateButton(buttonLeft, buttonTop, buttonRight, buttonBottom, buttonText);
    imageRestorer->WaitForAction();
    //CreateButton(buttonLeft, buttonTop, buttonRight, buttonBottom, buttonText, (ProcedureCallback)&imageRestorer->WaitForAction);
}

InfoBox::~InfoBox()
{
    free(imageRestorer);
}

int InfoBox::GetLower(int value1, int value2)
{
    return value1 < value2 ? value1 : value2 - MARGIN;
}

int InfoBox::GetHigher(int value1, int value2)
{
    return value1 > value2 ? value1 : value2 - MARGIN;
}

