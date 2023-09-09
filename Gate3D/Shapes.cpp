#include "Shapes.h"

void Shape::_addFace(Face face)
{
    for (const Point& vertex : face.getVertices())
    {
        Point* target = nullptr;
        for (Point& _vertex : this->_vertices)
        {
            if (vertex != _vertex)
                continue;
            target = &_vertex; // in case we'll need to make faces by PointsRefrence or something.
            break;
        }
        if (!target)
        {
            this->_vertices.push_back(Point(vertex));
        }
    }
    this->_faces.push_back(face);
}

Shape::Shape(Point& center, ShapeData& faces) : _center(center)
{
    for (const std::vector<Point>& face : faces) // TODO: this ugly
    {
        this->_addFace(Face(face));
    }
}

Shape makeCube()
{
    ShapeData shapeData;
    // each face's points' share one coordinate that is the same.
    float vertices[8][3]
    {
        { 1, 1, 1 },
        { 1, 1, -1 },
        { 1, -1, 1 },
        { 1, -1, -1 },
        { -1, 1, 1 },
        { -1, 1, -1 },
        { -1, -1, 1 },
        { -1, -1, -1 }
    };
    int faces[6][4] // Each row represents a face by specifying indices of 4 vertices
    {
        {0, 1, 3, 2},  // Front face
        {4, 5, 7, 6},  // Back face
        {0, 1, 5, 4},  // Right face
        {2, 3, 7, 6},  // Left face
        {0, 2, 6, 4},  // Top face
        {1, 3, 7, 5}   // Bottom face
    };

    for (int i = 0; i < 6; i++)
    {
        std::vector<Point> points;
        for (int j = 0; j < 4; j++)
        {
            points.push_back(Point(vertices[faces[i][j]]));
        }
        shapeData.push_back(points);
    }
    Point p = Point(0, 0, 0);
    return Shape(p, shapeData);
}
