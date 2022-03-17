#include "RMShape.h"

std::vector<rm::RMShape> rm::RMShape::shapes;

rm::RMShape::RMShape() {
    position = Vec3(0, 0, 0);
    rotation = Vec3(0, 0, 0);
    color = Vec4(0, 0, 0, 1);
    param1 = Vec3(0, 0, 0);
    param2 = Vec3(0, 0, 0);

    // Used for combining, subtracting, intersecting, etc. two shapes
    operation = rm::NoOp;
    operandIndex = -1;
    checkShape = true;

    // Keeping track of the shape
    index = rm::RMShape::shapes.size();
    type = rm::Invalid; // Default to sphere because there is no 'null' shape

    // Save this shape
    rm::RMShape::shapes.push_back(*this);
}

// Drawing - Sending values to shader //
void rm::RMShape::draw(sf::Shader* shader) {
    shader->setUniform("shapes[" + std::to_string(index) + "].poisiton", position);
    shader->setUniform("shapes[" + std::to_string(index) + "].rotation", rotation);
    shader->setUniform("shapes[" + std::to_string(index) + "].poisiton", color   );
    shader->setUniform("shapes[" + std::to_string(index) + "].param1",   param1  );
    shader->setUniform("shapes[" + std::to_string(index) + "].param2",   param2  );

    shader->setUniform("shapes[" + std::to_string(index) + "].operation",    operation   );
    shader->setUniform("shapes[" + std::to_string(index) + "].operandIndex", operandIndex);
    shader->setUniform("shapes[" + std::to_string(index) + "].checkShape",   checkShape  );

    shader->setUniform("shapes[" + std::to_string(index) + "].type", 0);
}

// Different Shapes

rm::RMShape& rm::RMShape::createSphere(Vec3 pos, Vec3 rot, Vec4 col, float r) {
    RMShape sphere;
    sphere.setPosition(pos);
    sphere.setRotation(rot);
    sphere.setColor(col);
    sphere.setParam1(Vec3(r, 0, 0));

    return sphere;
}

rm::RMShape& rm::RMShape::createBox(Vec3 pos, Vec3 rot, Vec4 col, Vec3 size) {
    RMShape box;
    box.setPosition(pos);
    box.setRotation(rot);
    box.setColor(col);
    box.setParam1(size);

    return box;
}

// Setters //

void rm::RMShape::setPosition(Vec3 pos) {
    position = pos;
}

void rm::RMShape::setRotation(Vec3 rot) {
    rotation = rot;
}

void rm::RMShape::setColor(Vec4 col) {
    color = col;
}

void rm::RMShape::setParam1(Vec3 p1) {
    param1 = p1;
}

void rm::RMShape::setParam2(Vec3 p2) {
    param2 = p2;
}

void rm::RMShape::setType(rm::ShapeType t) {
    type = t;
}

// Getters //

Vec3 rm::RMShape::getPosition() {
    return position;
}

Vec3 rm::RMShape::getRotation() {
    return rotation;
}

Vec4 rm::RMShape::getColor() {
    return color;
}

Vec3 rm::RMShape::getParam1() {
    return param1;
}

Vec3 rm::RMShape::getParam2() {
    return param2;
}

rm::ShapeType rm::RMShape::getType() {
    return type;
}