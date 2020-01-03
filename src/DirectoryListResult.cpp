#include <stdlib.h>

#include "DirectoryListResult.h"

DirectoryListResult::DirectoryListResult(String statusMessage)
{
    this->StatusMessage = statusMessage;
    //this->Items = std::vector<String>();
}

DirectoryListResult::DirectoryListResult(DirectoryListItemType items)
{
    this->StatusMessage = NULL;
    //this->Items = items;
}

DirectoryListResult::~DirectoryListResult()
{
}
