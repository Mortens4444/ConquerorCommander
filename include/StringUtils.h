#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#include "Constants.h"

class StringUtils
{
    public:
        static String Reverse(String);

        static size_t ExtendedStrLen(int extraLength, String, va_list);

        static String ExtendedStrCat(String, ...);

        static bool IsNullOrEmpty(const String);

        static int GetTextWidth(String);

    protected:

    private:
        StringUtils();
};

#endif
