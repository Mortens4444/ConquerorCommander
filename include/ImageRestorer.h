#ifndef IMAGERESTORER_H
#define IMAGERESTORER_H

class ImageRestorer
{
    public:
        ImageRestorer(int, int, int, int);
        virtual ~ImageRestorer();

        void WaitForAction();

    protected:

    private:
        void *ptr;

        int left;
        int top;
        int right;
        int bottom;

        void Dispose();
};

#endif
