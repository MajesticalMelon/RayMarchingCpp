#include "RMShape.h"

#include "Rotations.h"

std::vector<rm::RMShape*> rm::RMShape::shapes;

rm::RMShape::RMShape() {
    position = Vec3(0, 0, 0);
    rotation = Vec3(0, 0, 0);
    color = Vec4(0, 0, 0, 1);
    param1 = Vec3(0, 0, 0);
    param2 = Vec3(0, 0, 0);
    origin = Vec3(0, 0, 0);

    // Used for combining, subtracting, intersecting, etc. two shapes
    operation = rm::NoOp;
    operandIndex = -1;
    checkShape = true;

    // Keeping track of the shape
    index = rm::RMShape::shapes.size();
    type = rm::Invalid; // Default to sphere because there is no 'null' shape

    // Save this shape
    rm::RMShape::shapes.push_back(this);
}

// Drawing - Sending values to shader //
void rm::RMShape::draw(sf::Shader* shader) {
    shader->setUniform("shapes[" + std::to_string(index) + "].position", position);
    shader->setUniform("shapes[" + std::to_string(index) + "].rotation", rotation);
    shader->setUniform("shapes[" + std::to_string(index) + "].color",    color   );
    shader->setUniform("shapes[" + std::to_string(index) + "].param1",   param1  );
    shader->setUniform("shapes[" + std::to_string(index) + "].param2",   param2  );

    shader->setUniform("shapes[" + std::to_string(index) + "].operation",    (int)operation );
    shader->setUniform("shapes[" + std::to_string(index) + "].operandIndex", operandIndex   );
    shader->setUniform("shapes[" + std::to_string(index) + "].checkShape",   checkShape     );

    shader->setUniform("shapes[" + std::to_string(index) + "].type", type);

    // Send any shapes that are now part of this shape to the shader
    if (operandIndex > -1) {
        shapes.at(operandIndex)->draw(shader);
    }
}

// Different Shapes

rm::RMShape* rm::RMShape::createSphere(Vec3 pos, Vec3 rot, Vec4 col, float r) {
    RMShape* sphere = new RMShape();
    sphere->setPosition(pos);
    sphere->setRotation(rot);
    sphere->setColor(col);
    sphere->setParam1(Vec3(r, 0, 0));
    sphere->setType(rm::Sphere);

    return sphere;
}

rm::RMShape* rm::RMShape::createBox(Vec3 pos, Vec3 rot, Vec4 col, Vec3 size) {
    RMShape* box = new RMShape();
    box->setPosition(pos);
    box->setRotation(rot);
    box->setColor(col);
    box->setParam1(size);
    box->setType(rm::Box);

    return box;
}

rm::RMShape* rm::RMShape::createCapsule(Vec3 pos1, Vec3 pos2, Vec4 col, float r) {
    RMShape* capsule = new RMShape();
    capsule->setPosition(pos1);
    capsule->setColor(col);
    capsule->setParam1(pos2);
    capsule->setParam2(Vec3(r, 0, 0));
    capsule->setType(rm::Capsule);

    return capsule;
}


// Different Operations //
// Mostly jsut a wrapper around the setOperation function

void rm::RMShape::combine(rm::RMShape* opd) {
    setOperation(rm::Union, opd);
}

void rm::RMShape::intersection(rm::RMShape* opd) {
    setOperation(rm::Intersection, opd);
}

void rm::RMShape::subtract(rm::RMShape* opd) {
    setOperation(rm::Subtract, opd);
}

void rm::RMShape::smoothCombine(rm::RMShape* opd) {
    setOperation(rm::SmoothUnion, opd);
}

void rm::RMShape::smoothIntersection(rm::RMShape* opd) {
    setOperation(rm::SmoothIntersection, opd);
}

void rm::RMShape::smoothSubtract(rm::RMShape* opd) {
    setOperation(rm::SmoothSubtract, opd);
}

// Setters //

void rm::RMShape::setPosition(Vec3 pos) {
    position = pos;
}

// Rotates the shape about the origin (defaults to position)
void rm::RMShape::setRotation(Vec3 rot) {
    rotation = rot;

    /*Vec3 offset = position - origin;
    position = rotateXYZ(offset, rot) + origin;*/
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

void rm::RMShape::setOperation(rm::Operation op, rm::RMShape* opd) {
    operation = op;
    operandIndex = opd->getIndex();
    opd->setVisible(false);
}

void rm::RMShape::setVisible(bool visible) {
    checkShape = visible;
}

// Set the origin of rotation relative to position
void rm::RMShape::setOrigin(Vec3 orig) {
    origin = orig;
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
    return (rm::ShapeType)type;
}

int rm::RMShape::getIndex() {
    return index;
}