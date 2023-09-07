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

    struct {
        float fNear;
        float fFar;
        float fFov;
        float aspectRatio;
        float fFovRad;
    } _projectionNeeds;

    Matrix<Matrix<float, 4>, 4> _projectionMatrix;
    Point _cameraPose = Point(0.0, 0.0, 0.0);
    Matrix<float, 3> _cameraAngle;
    void _reSetProjectionMatrix();
public:
    Renderer();
    void reSize(int width, int height);
    Color** GetPixelsArrayPointer();
    void resetFrame();
    void drawLine(const Line& line);
    void drawPolygon(const Face& face);
    Color& getPixel(int row, int column);
    void moveCameraPos(const Point& diff);
    Color& operator[](const PixelCoordinate& cord);
    Point& getCameraPose();
    PixelCoordinate project(Point p);
};

extern Renderer View;
