#include "Main.h"


void Program::start()
{
    Shape loaded = LoadModel("saves\\cube.stl");
    loaded.setCenter(Point(0, 0, -70)); // in case it's a big model
    this->_shapes.push_back(loaded);
}

void Program::update(float deltaTime)
{
    // if (deltaTime)
    //     return;
    View.resetFrame();
    Shape shape = this->_shapes.back();
    std::vector<Line> lines;
    this->_shapes[0].setRotation(this->_shapes[0].getRotation());
    for (const Face& face : shape.getFaces())
    {
        View.drawTriangle(face);
        continue;
        if (!View.drawPolygon(face))
            continue;
        for (const Line& line : face.getLines())
            lines.push_back(line);
    }
    for (const Line& line : lines)
    {
        View.drawLine(line);
    }
}

void Program::onKeyDown(char key)
{
    Point cameraMovement(0, 0, 0);
    switch (key)
    {
    case VK_RIGHT:
    {
        cameraMovement[0] = 1;
    } break;
    case VK_LEFT:
    {
        cameraMovement[0] = -1;
    } break;
    case VK_UP:
    {
        cameraMovement[1] = 1;
    } break;
    case VK_DOWN:
    {
        cameraMovement[1] = -1;
    } break;
    case VK_SPACE:
    {
        this->update(0);
    } break;
    default:
    {
        return;
        break;
    }
    }

    if (!(cameraMovement == Point(0, 0, 0)))
        View.moveCameraPos(cameraMovement);
}

void Program::scroll(int scrollAmount, int keys)
{
    Point p = Point(0, 0, 0);
    p[2] = 10 * (scrollAmount > 0 ? 1 : -1);
    View.moveCameraPos(p);
}

void Program::onMouseMoved(int posX, int posY, int keysHeld)
{
    Matrix<float, 2> normalPos = Matrix<float, 2>(posX, posY) / View.getWindowSize();
    Matrix<float, 2> diff = normalPos - this->_normalizedLastMousePos;
    this->_normalizedLastMousePos = normalPos;
    if (keysHeld & 16)
    {
        View.moveCameraPos(Point(diff[0], -diff[1], 0) * 5);
    }
    else if (keysHeld & MK_RBUTTON)
    {
        Rotation rot = this->_shapes[0].getRotation();
        rot[0] += diff[1] * 3.14;
        rot[1] += diff[0] * 3.14;
        this->_shapes[0].setRotation(rot);
    }
}
