#include "Renderer.h"
#include <iostream>

Renderer::Renderer()
{
    this->_frame = { 0 };
    this->_projectionNeeds =
    {
        0.1,
        1000.0,
        90.0,
        0,
        0,
    };
}

void Renderer::reSize(int width, int height)
{
    this->_frame.width = width;
    this->_frame.height = height;
    if (!height || !width)
        return;

    this->resetFrame();
    this->_reSetProjectionMatrix();

}

void Renderer::_reSetProjectionMatrix()
{
    this->_projectionNeeds.aspectRatio = (float)this->_frame.height / this->_frame.width;
    this->_projectionNeeds.fFovRad = 1.0 / tanf(this->_projectionNeeds.fFov * 0.5 / 180 * 3.14159);

    this->_projectionMatrix[0][0] = this->_projectionNeeds.aspectRatio * this->_projectionNeeds.fFovRad;
    this->_projectionMatrix[1][1] = this->_projectionNeeds.fFovRad;
    this->_projectionMatrix[2][2] = this->_projectionNeeds.fFar / (this->_projectionNeeds.fFar - this->_projectionNeeds.fNear);
    this->_projectionMatrix[3][2] = (-this->_projectionNeeds.fFar * this->_projectionNeeds.fNear) / (this->_projectionNeeds.fFar - this->_projectionNeeds.fNear);
    this->_projectionMatrix[2][3] = 1.0;
    this->_projectionMatrix[3][3] = 0;
}


void Renderer::resetFrame()
{
    for (int i = 0; i < this->_frame.width * this->_frame.height; i++)
    {
        this->_frame.pixels[i] = 0x0000FF00;
    }
}

void Renderer::drawLine(const Line& line)
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
        b *= 1000.0 / (line.b[2] - this->_cameraPose[2]);

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

void Renderer::drawPolygon(const Face& face)
{
    std::vector<PixelCoordinate> vertices;
    int maxY = INT_MAX;
    int minY = -INT_MAX;
    for (const Point& vertex : face.getVertices())
    {
        Matrix<float, 4> p = Matrix<float, 4>(vertex[0], vertex[1], vertex[2], 1);
        p = p * this->_projectionMatrix;
        Point projectedVertex = Point(p[0], p[1], p[2]) * (1.0 / p[3]);
        int x = vertex[0] + this->_cameraPose[0],
            y = vertex[1] - this->_cameraPose[1];
        int diff = vertex[2] / (this->_cameraPose[2] + 11);
        if (diff < 0)
            continue;
        if (diff)
        {
            x /= diff;
            y /= diff;

        }
        minY = min(minY, y);
        maxY = max(maxY, y + 1);
        vertices.push_back(PixelCoordinate(x, y));
    }
    minY = max(minY, -this->_frame.height / 2);
    maxY = min(maxY, this->_frame.height / 2);
    for (int y = minY; y < maxY; y++)
    {
        std::vector<int> intersections;

        // Find intersections with each edge
        for (size_t i = 0; i < vertices.size(); i++) {
            const PixelCoordinate& p1 = vertices[i];
            const PixelCoordinate& p2 = vertices[(i + 1) % vertices.size()];

            if ((p1[1] < y && p2[1] >= y) || (p2[1] < y && p1[1] >= y)) {
                // Calculate x-coordinate of intersection
                int x = p1[0] + (y - p1[0]) * (p2[0] - p1[0]) / (p2[1] - p1[1]);
                intersections.push_back(x);
            }
        }

        // Sort intersections in ascending order
        std::sort(intersections.begin(), intersections.end());

        // Fill between pairs of intersections
        for (size_t i = 0; i < intersections.size(); i += 2) {
            int x1 = min(intersections[i], this->_frame.width / 2);
            int x2 = max(intersections[i + 1], -this->_frame.width / 2);
            for (int x = x1; x <= x2; x++) {
                // if (x > this->_frame.width / 2)
                (*this)[PixelCoordinate(x, y)] = 0x00FF0000;
            }
        }
    }
}

Color& Renderer::getPixel(int row, int column)
{
    return this->_frame.pixels[(this->_frame.height / 2 - row) * this->_frame.width + column - this->_frame.width / 2];
}

Color& Renderer::operator[](const PixelCoordinate& cord)
{
    return this->_frame.pixels[(this->_frame.height / 2 - (int)cord[1]) * this->_frame.width + (int)cord[0] - this->_frame.width / 2];
}

Point& Renderer::getCameraPose()
{
    return this->_cameraPose;
}

void Renderer::moveCameraPos(const Point& diff)
{
    this->_cameraPose += diff;
}

Color** Renderer::GetPixelsArrayPointer()
{
    return &this->_frame.pixels;
}

Renderer View;