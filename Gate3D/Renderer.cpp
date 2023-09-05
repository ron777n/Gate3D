#include "Renderer.h"
#include <iostream>

Renderer::Renderer()
{
    this->_frame = { 0 };
}

void Renderer::reSize(int width, int height)
{
    this->_frame.width = width;
    this->_frame.height = height;
    this->resetFrame();

}

void Renderer::resetFrame()
{
    for (int i = 0; i < this->_frame.width * this->_frame.height; i++)
    {
        this->_frame.pixels[i] = 0x0000FF00;
    }
}

void Renderer::drawLine(Line line)
{
    PixelCoordinate a;
    PixelCoordinate b;
    a[0] = line.a[0] + this->_cameraPose[0];
    a[1] = line.a[1] - this->_cameraPose[1];
    if ((line.a[2] - this->_cameraPose[2]))
        a *= 1000 / (line.a[2] - this->_cameraPose[2]);
    b[0] = line.b[0] + this->_cameraPose[0];
    b[1] = line.b[1] - this->_cameraPose[1];
    if (line.b[2] - this->_cameraPose[2])
        b *= 1000 / (line.b[2] - this->_cameraPose[2]);

    int sx = a[0] < b[0] ? 1 : -1, sy = a[1] < b[1] ? 1 : -1;
    int dx = abs(b[0] - a[0]), dy = abs(b[1] - a[1]);
    int err = dx - dy;

    while (a[0] != b[0] or a[1] != b[1])
    {
        if (abs(a[0]) < this->_frame.width / 2 and abs(a[1]) < this->_frame.height / 2)
        {
            (*this)[a] = 0x00FF0000;
        }
        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            a[0] += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            a[1] += sy;
        }
    }

    // this->getPixel(line.a[1], line.a[0]) = 0x00FF0000;
    // this->getPixel(line.b[1], line.b[0]) = 0x00FF0000;
}

Color& Renderer::getPixel(int row, int column)
{
    return this->_frame.pixels[(this->_frame.height / 2 - row) * this->_frame.width + column - this->_frame.width / 2];
}

Color& Renderer::operator[](PixelCoordinate cord)
{
    return this->_frame.pixels[(this->_frame.height / 2 - cord[1]) * this->_frame.width + cord[0] - this->_frame.width / 2];
}

Point& Renderer::getCameraPose()
{
    return this->_cameraPose;
}

void Renderer::moveCameraPos(Point diff)
{
    this->_cameraPose += diff;
}

Color** Renderer::GetPixelsArrayPointer()
{
    return &this->_frame.pixels;
}

Renderer View;