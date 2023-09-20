#include "Renderer.h"
#include <iostream>

Renderer::Renderer()
{
    this->_frame = { 0 };
    this->_projectionNeeds =
    {
        0.1,
        1000.0,
        80.0,
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

    this->_projectionMatrix[0][0] = this->_projectionNeeds.aspectRatio *
        this->_projectionNeeds.fFovRad * 0.5 * this->_frame.width;
    this->_projectionMatrix[1][1] = this->_projectionNeeds.fFovRad
        * 0.5 * this->_frame.height;
    this->_projectionMatrix[2][2] = this->_projectionNeeds.fFar /
        (this->_projectionNeeds.fFar - this->_projectionNeeds.fNear);
    this->_projectionMatrix[2][3] = (-this->_projectionNeeds.fFar *
        this->_projectionNeeds.fNear)
        / (this->_projectionNeeds.fFar - this->_projectionNeeds.fNear);
    this->_projectionMatrix[3][2] = 1.0;
    this->_projectionMatrix[3][3] = 0.0;
}


void Renderer::resetFrame()
{
    for (int i = 0; i < this->_frame.width * this->_frame.height; i++)
    {
        this->_frame.pixels[i] = 0x00FFFFFF;
    }
}

void Renderer::drawLine(const Line& line)
{
    PixelCoordinate a = this->projectCamera(this->applyCameraOffset(line.a));
    PixelCoordinate b = this->projectCamera(this->applyCameraOffset(line.b));

    int sx = a[0] < b[0] ? 1 : -1, sy = a[1] < b[1] ? 1 : -1;
    int dx = abs(b[0] - a[0]), dy = abs(b[1] - a[1]);
    int err = dx - dy;

    while (a[0] != b[0] or a[1] != b[1])
    {
        if (abs(a[0]) < this->_frame.width / 2 and abs(a[1]) < this->_frame.height / 2)
        {
            (*this)[a] = 0x00000000;
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

bool Renderer::drawTriangle(const Face& face)
{
    std::vector<PixelCoordinate> vertices; // = face.getVertices();
    for (const Point& vertex : face.getVertices())
    {
        Point offsetVertex = this->applyCameraOffset(vertex);
        if (offsetVertex.dotProduct(face.getNormal()) >= 0)
            return false;
        PixelCoordinate projectedVertex = this->projectCamera(offsetVertex);
        vertices.push_back(projectedVertex);
    }
    if (vertices.size() != 3)
        throw std::exception("Invalid vertices count of triangle.");
    if (vertices[1][1] < vertices[0][1])
        std::swap(vertices[0], vertices[1]);
    if (vertices[2][1] < vertices[0][1])
        std::swap(vertices[0], vertices[2]);
    if (vertices[2][1] < vertices[1][1])
        std::swap(vertices[1], vertices[2]);

    int dx1 = vertices[1][0] - vertices[0][0];
    int dy1 = vertices[1][1] - vertices[0][1];
    int dx2 = vertices[2][0] - vertices[0][0];
    int dy2 = vertices[2][1] - vertices[0][1];
    float daxStep = 0, dbxStep = 0;
    if (dy1) daxStep = dx1 / (float)abs(dy1);
    if (dy2) dbxStep = dx2 / (float)abs(dy2);
    int startX = vertices[0][0], endX = vertices[1][0];
    int startY = vertices[0][1], endY = vertices[1][1];
    for (int i = startY; i <= endY; i++)
    {
        int ax = startX + (float)(i - startY) * daxStep;
        int bx = startX + (float)(i - startY) * dbxStep;
        if (ax > bx)
        {
            std::swap(ax, bx);
        }
        float tstep = 1.0f / (bx - ax);
        float t = 0;
        for (int j = ax; j < bx; j++)
        {
            std::stringstream egg;
            // float xLoc = , yLoc;
            // egg << t << "\n";
            // egg << "relative coordinate: " << i << ", " << j << "\n";
            // Debug(egg.str().c_str());
            face.getPixelColor((*this)[PixelCoordinate(i, j)], Matrix<float, 2>(i, j));
            t += tstep;
        }
    }
}

bool Renderer::drawPolygon(const Face& face)
{
    std::vector<PixelCoordinate> vertices;
    int maxY = -INT_MAX;
    int minY = INT_MAX;
    for (const Point& vertex : face.getVertices())
    {
        Point offsetVertex = this->applyCameraOffset(vertex);
        if (offsetVertex.dotProduct(face.getNormal()) >= 0)
            return false;
        PixelCoordinate projectedVertex = this->projectCamera(offsetVertex);
        minY = min(minY, projectedVertex[1]);
        maxY = max(maxY, projectedVertex[1] + 1);
        vertices.push_back(projectedVertex);
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
                int x = p1[0] + (y - p1[1]) * (p2[0] - p1[0]) / (p2[1] - p1[1]);
                intersections.push_back(x);
            }
        }

        // Sort intersections in ascending order
        std::sort(intersections.begin(), intersections.end());

        // Fill between pairs of intersections
        for (size_t i = 0; i < intersections.size(); i += 2) {
            int x1 = intersections[i],
                x2 = intersections[i + 1];
            x1 = max(x1, -this->_frame.width / 2),
                x2 = min(x2, this->_frame.width / 2);
            Matrix<float, 2> screenSize = Matrix<float, 2>(this->_frame.height, this->_frame.width);
            for (int x = x1; x <= x2; x++) {
                PixelCoordinate cord = PixelCoordinate(x, y);
                face.getPixelColor(
                    (*this)[cord],
                    Matrix<float, 2>(cord[0], cord[1]) / screenSize * 2 - 1.0
                );
            }
        }
    }
    return true;
}

Color& Renderer::operator[](const PixelCoordinate& cord)
{
    return this->_frame.pixels[(this->_frame.height / 2 - (int)cord[1]) * this->_frame.width + (int)cord[0] - this->_frame.width / 2];
}

Point& Renderer::getCameraPose()
{
    return this->_cameraPose;
}

Point Renderer::applyCameraOffset(const Point& vertex) const
{
    return Point(vertex[0] + this->_cameraPose[0],
        vertex[1] + this->_cameraPose[1], vertex[2] + this->_cameraPose[2]);
}

PixelCoordinate Renderer::projectCamera(const Point& vertex) const
{
    // Point offsetPoint = this->applyCameraOffset(vertex);
    Matrix<float, 4> p = Matrix<float, 4>(vertex[0], vertex[1], vertex[2], 1);
    p = p * this->_projectionMatrix;
    if (p[3] == 0)
        return PixelCoordinate(p[0], p[1]);
    return PixelCoordinate(p[0] / p[3], p[1] / p[3]);
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