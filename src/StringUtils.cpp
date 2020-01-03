#include <stdlib.h>
#include <string.h>

#include "graphics.h"

#include "Constants.h"
#include "StringUtils.h"

String StringUtils::Reverse(String s)
{
	int i, j, length = strlen(s);
    String result = (String)malloc(length + 1);

	for (i = length - 1, j = 0; i >= 0; i--, j++)
	{
		result[j] = s[i];
	}
	result[length] = '\0';
	return result;
}

bool StringUtils::IsNullOrEmpty(const String s)
{
    return s == NULL || *s == '\0';
}

size_t StringUtils::ExtendedStrLen(int extraLength, String s, va_list args)
{
    String current;
    size_t length = strlen(s);
    while (!IsNullOrEmpty(current = va_arg(args, String)))
    {
        length += strlen(current) + extraLength;
    }
    va_end(args);
    return length + 1;
}

String StringUtils::ExtendedStrCat(String s, ...)
{
    va_list args;
    String current;

    va_start(args, s);
    size_t length = ExtendedStrLen(0, s, args);
    String result = (String)malloc(length);

    strncpy(result, s, strlen(s) + 1);
    va_start(args, s);
    while (!IsNullOrEmpty(current = va_arg(args, String)))
    {
        strcat(result, current);
    }
    va_end(args);
    return result;
}

int StringUtils::GetTextWidth(String text)
{
    return textwidth(text) + EXTRA_WIDTH;
}
