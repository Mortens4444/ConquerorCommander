#ifndef INFOBOX_H
#define INFOBOX_H

#include "ImageRestorer.h"

class InfoBox
{
    public:
        InfoBox(String, int = 80);
        virtual ~InfoBox();

    protected:

    private:
        ImageRestorer* imageRestorer;

        int GetLower(int, int);
        int GetHigher(int, int);
};

#endif
