#pragma once
#include "Definitions.hpp"
#include "Shapes.h"
#include "Renderer.h"

class Program
{
private:
    std::vector<Shape> _shapes;
    Matrix<float, 2> _normalizedLastMousePos;
    float angle = 0.0;
public:
    Program() {};
    void start();
    void update(float deltaTime);
    void onKeyDown(char key);
    void scroll(int scrollAmount, int keys);
    void onMouseMoved(int posX, int posY, int keysHeld);
};

