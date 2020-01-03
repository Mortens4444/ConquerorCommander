#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "Path.h"
#include "StringUtils.h"

String Path::Combine(String s, ...)
{
    va_list args;
    String current;

    va_start(args, s);
    size_t length = StringUtils::ExtendedStrLen(1, s, args);
    String result = (String)malloc(length);

    strncpy(result, s, strlen(s) + 1);
    va_start(args, s);
    while (!StringUtils::IsNullOrEmpty(current = va_arg(args, String)))
    {
        if (result[strlen(result) - 1] != '\\')
        {
            strcat(result, "\\");
        }
        strcat(result, current);
    }
    va_end(args);
    return result;
}
