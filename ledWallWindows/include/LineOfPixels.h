#ifndef LINEOFPIXELS_H
#define LINEOFPIXELS_H


/***

This class represents a line of pixels on a slab

***/

const static int slabLenght = 18;


class LineOfPixels
{
    public:
        LineOfPixels(int x0, int y0, bool leftToRight);     ///constructs the line
        void startCounting();                       ///index is initialized with -1
        void nextPixel(int & x, int & y);           ///puts the position of the next pixel in x and y
        bool isAtTheEnd() const { return m_index == slabLenght - 1; }

    private:


        int m_index;                                ///While running throw the line, index indicates what number of pixels we have already been through
        int m_x0;                                   ///The x position of the top left pixel
        int m_y0;                                   ///The y position of the top left pixel
        bool m_leftToRight;                         ///Indicates whether the line is traveled from left to right or from right to left
};

#endif // LINEOFPIXELS_H
