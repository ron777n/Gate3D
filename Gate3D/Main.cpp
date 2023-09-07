#include "Main.h"


void Program::start()
{
    Shape cube = makeCube();
    this->_shapes.push_back(cube);
}

void Program::update()
{
    View.resetFrame();
    Shape shape = this->_shapes.back();
    std::vector<Line> lines;
    for (const Face& face : shape.getFaces())
    {
        // View.drawPolygon(shape.getFaces()[0]);
        View.drawPolygon(face);
        for (const Line& line : face.getLines())
        {
            lines.push_back(line);
        }
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
        std::stringstream egg;
        egg << "Camera Pos: " << View.getCameraPose() << std::endl;
        Debug(egg.str().c_str());
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
    PixelCoordinate pos(posX, posY);
    this->_lastMousePos = pos;
    PixelCoordinate diff = pos - this->_lastMousePos;
    if (keysHeld & 16)
    {
        Point p = {0};
        p[0] = diff[0];
        p[1] = -diff[1];
        View.moveCameraPos(p);
    }
}
