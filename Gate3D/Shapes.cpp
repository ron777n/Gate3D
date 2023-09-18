#include "Shapes.h"
#include <fstream>

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

Shape::Shape(const std::vector<Face>& faces)
{
    for (const Face& face : faces)
    {
        this->_addFace(face);
    }
    this->setRotation(Rotation());
}
Shape::Shape(const std::vector<Face>& faces, const Point& center)
{
    this->_center = center;
    for (const Face& face : faces)
    {
        this->_addFace(Face(face));
    }
    this->setRotation(Rotation());
}
Shape::Shape(const std::vector<Face>& faces, const Point& center, const Rotation& rot)
{
    this->setRotation(rot);
    this->_center = center;
    for (const Face& face : faces)
    {
        this->_addFace(Face(face));
    }
}

void Shape::setRotation(const Rotation& rot)
{
    this->_rotation = rot;
    this->_rotationMatrix[0][0] = cosf(rot[1]) * cosf(rot[2]);
    this->_rotationMatrix[0][1] = cosf(rot[1]) * sinf(rot[2]);
    this->_rotationMatrix[0][2] = -sinf(rot[1]);
    this->_rotationMatrix[1][0] = sinf(rot[0]) * sinf(rot[1]) * cosf(rot[2]) - cosf(rot[0]) * sinf(rot[2]);
    this->_rotationMatrix[1][1] = sinf(rot[0]) * sinf(rot[1]) * sinf(rot[2]) + cosf(rot[0]) * cosf(rot[2]);
    this->_rotationMatrix[1][2] = sinf(rot[0]) * cosf(rot[1]);
    this->_rotationMatrix[2][0] = cosf(rot[0]) * sinf(rot[1]) * cosf(rot[2]) + sinf(rot[0]) * sinf(rot[2]);
    this->_rotationMatrix[2][1] = cosf(rot[0]) * sinf(rot[1]) * sinf(rot[2]) - sinf(rot[0]) * cosf(rot[2]);
    this->_rotationMatrix[2][2] = cosf(rot[0]) * cosf(rot[1]);
}

struct triangle
{
    float normalv[3];
    float v1[3];
    float v2[3];
    float v3[3];
    char attr[2];
};

Shape LoadModel(std::string fileName)
{
    // return makeCube();
    std::ifstream stl(fileName, std::ios::binary);
    if (!stl)
    {
        throw std::exception("unable to open file");
    }
    stl.seekg(80, stl.beg);
    unsigned int verticesCount = 0;
    stl.read((char*)&verticesCount, 4);
    struct triangle triangle1;
    std::vector<Face> faces;
    for (unsigned int i = 0; i < verticesCount; i++)
    {
        std::vector<Point> points;
        stl.read((char*)&triangle1, 50);
        points.push_back(Point(triangle1.v1));
        points.push_back(Point(triangle1.v2));
        points.push_back(Point(triangle1.v3));
        faces.push_back(Face(points, triangle1.normalv));
    }

    stl.close();
    return Shape(faces);
}

Shape makeCube()
{
    std::vector<Face> shapeFaces;
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
        shapeFaces.push_back(Face(points));
    }
    return Shape(shapeFaces);
}
