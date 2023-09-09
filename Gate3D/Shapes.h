#pragma once
#include "Definitions.hpp"
#include <vector>


class Shape
{
private:
    Point _center;
    std::vector<Point> _vertices;
    std::vector<Face> _faces;
    void _addFace(Face face);

public:
    Shape(Point& center, ShapeData& faces);
    std::vector<Point> getVertices() { return this->_vertices; };
    const std::vector<Face>& getFaces() { return this->_faces; };
};

Shape makeCube();
