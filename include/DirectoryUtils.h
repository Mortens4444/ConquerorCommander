#ifndef DIRECTORYUTILS_H
#define DIRECTORYUTILS_H

#include "DirectoryListResult.h"

class DirectoryUtils
{
    public:
        static DirectoryListResult DirectoryList(String directory, int from, int maxItemCount);

    protected:

    private:
        DirectoryUtils();
};

#endif
