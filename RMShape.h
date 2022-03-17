#pragma once
#include <SFML/Graphics.hpp>

using namespace sf::Glsl;

class RMShape {
private:
    Vec3 position;
    Vec3 rotation;
    Vec4 color;
    Vec3 param1;
    Vec3 param2;

    // Used for combining, subtracting, intersecting, etc. two shapes
    int operation;
    int operandIndex;
    bool checkShape;
public:
    RMShape();

    void draw(Shader &shader);
    static RMShape& createSphere();
    static RMShape& createBox();
};