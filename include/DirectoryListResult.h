#ifndef DIRECTORYLISTRESULT_H
#define DIRECTORYLISTRESULT_H

#include <vector>

#include "Constants.h"

typedef std::vector<String> DirectoryListItemType;

class DirectoryListResult
{
    public:
        DirectoryListResult(String statusMessage);
        DirectoryListResult(DirectoryListItemType items);
        virtual ~DirectoryListResult();

        String StatusMessage;
        DirectoryListItemType Items[];

    protected:

    private:
};

#endif
