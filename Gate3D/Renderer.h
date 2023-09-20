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
    bool drawPolygon(const Face& face);
    bool drawTriangle(const Face& face);
    void moveCameraPos(const Point& diff);
    Color& operator[](const PixelCoordinate& cord);
    Point& getCameraPose();
    PixelCoordinate projectCamera(const Point& p) const;
    Point applyCameraOffset(const Point& vertex) const;
    Matrix<float, 2> getWindowSize() { return Matrix<float, 2>(this->_frame.width, this->_frame.height); };
};

extern Renderer View;
