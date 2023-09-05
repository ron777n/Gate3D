#pragma once
#include "Definitions.hpp"


class Renderer
{
private:
    struct {
        int width;
        int height;
        Color* pixels;
    } _frame;
    Point _cameraPose;
    Matrix<float, 3> _cameraAngle;
public:
    Renderer();
    Color** GetPixelsArrayPointer();
    void reSize(int width, int height);
    void resetFrame();
    void drawLine(Line line);
    Color& getPixel(int row, int column);
    void moveCameraPos(Point diff);
    Color& operator[](PixelCoordinate cord);
    Point& getCameraPose();
};

extern Renderer View;
