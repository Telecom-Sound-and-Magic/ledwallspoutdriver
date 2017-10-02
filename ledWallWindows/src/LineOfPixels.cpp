#include "../include/LineOfPixels.h"

LineOfPixels::LineOfPixels(int x0, int y0, bool leftToRight) : m_x0(x0), m_y0(y0), m_leftToRight(leftToRight)
{

}

void LineOfPixels::startCounting()
{
    m_index = -1;
}

void LineOfPixels::nextPixel(int & x, int & y)
{
    m_index += 1;

    if(m_leftToRight)
    {
        x = m_x0 + m_index;
        y = m_y0;
    }
    else
    {
        x = m_x0 + slabLenght - m_index - 1;
        y = m_y0;
    }
}

