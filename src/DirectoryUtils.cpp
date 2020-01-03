//#include <fileapi.h>

#include "DirectoryUtils.h"
#include "Path.h"

#define FindFirst FindFirstFile
#define FindNext FindNextFile

DirectoryListResult DirectoryUtils::DirectoryList(String directory, int from, int maxItemCount)
{
    int itemsCount = 0;
	DirectoryListItemType items = DirectoryListItemType();



	return DirectoryListResult(items);
}
