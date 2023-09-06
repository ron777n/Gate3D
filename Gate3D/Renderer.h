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
    Point _cameraPose{0, 0, 0};
    Matrix<float, 3> _cameraAngle;
public:
    Renderer();
    Color** GetPixelsArrayPointer();
    void reSize(int width, int height);
    void resetFrame();
    void drawLine(const Line& line);
    void drawPolygon(const Face& face);
    Color& getPixel(int row, int column);
    void moveCameraPos(const Point& diff);
    Color& operator[](const PixelCoordinate& cord);
    Point& getCameraPose();
};

extern Renderer View;
