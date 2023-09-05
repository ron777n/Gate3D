#pragma once
#include "Definitions.hpp"
#include "Shapes.h"
#include "Renderer.h"

class Program
{
private:
    std::vector<Shape> _shapes;
    PixelCoordinate _lastMousePos;
public:
    Program() {};
    void start();
    void update();
    void onKeyDown(char key);
    void scroll(int scrollAmount, int keys);
    void onMouseMoved(int posX, int posY, int keysHeld);
};

