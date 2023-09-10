#pragma once
#include "Definitions.hpp"
#include <vector>


class Shape
{
private:
    Point _center;
    Rotation _rotation;
    Matrix<Matrix<float, 3>, 3> _rotationMatrix;
    std::vector<Point> _vertices;
    std::vector<Face> _faces;
    void _addFace(Face face);

public:
    Shape(const ShapeData& faces);
    Shape(const ShapeData& faces, const Point& center);
    Shape(const ShapeData& faces, const Point& center, const Rotation& rotation);
    std::vector<Point> getVertices() { return this->_vertices; };
    const std::vector<Face> getFaces() 
    {     
        std::vector<Face> faces;
        for (const Face& face : this->_faces) 
        {
            std::vector<Point> vertices;
            for (const Point& vertex : face.getVertices()) 
            {
                vertices.push_back(vertex * this->_rotationMatrix + this->_center);
            }
            faces.push_back(Face(vertices));
        }
        return faces;
    };
    Rotation getRotation() const { return this->_rotation; };
    void setRotation(const Rotation& rot);
};

Shape LoadModel(std::string fileName); // works only with stl files
bool saveModel(Shape shape, std::string fileName);
Shape makeCube();
