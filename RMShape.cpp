#include "RMShape.h"

#include "Rotations.h"


#pragma region Vector Math
float rm::VectorHelper::length(Vec3 p) {
    return sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);
}

Vec3 rm::VectorHelper::vectorAbs(Vec3 p) {
    return Vec3(abs(p.x), abs(p.y), abs(p.z));
}

Vec3 rm::VectorHelper::vectorMax(Vec3 p, Vec3 q) {
    return Vec3(fmax(p.x, q.x), fmax(p.y, q.y), fmax(p.z, q.z));
}

Vec3 rm::VectorHelper::vectorMin(Vec3 p, Vec3 q) {
    return Vec3(fmin(p.x, q.x), fmin(p.y, q.y), fmin(p.z, q.z));
}

float rm::VectorHelper::dot(Vec3 p, Vec3 q) {
    return p.x * q.x + p.y * q.y + p.z * q.z;
}

float rm::VectorHelper::clamp(float val, float low, float high) {
    return fmax(fmin(val, high), low);
}

Vec3 rm::VectorHelper::normalize(Vec3 p) {
    p /= length(p);
    return p;
}

#pragma endregion

#pragma region Init
const float rm::RMShape::EPSILON = 0.01f;
std::vector<rm::RMShape*> rm::RMShape::shapes;
std::vector<rm::RMMaterial*> rm::RMShape::materials({ &defaultMat });

rm::RMShape::RMShape() {
    position = Vec3(0, 0, 0);
    rotation = Vec3(0, 0, 0);
    param1 = Vec3(0, 0, 0);
    param2 = Vec3(0, 0, 0);
    origin = Vec3(0, 0, 0);

    // Used for combining, subtracting, intersecting, etc. two shapes
    operation = rm::NoOp;
    operandIndex = -1;
    checkShape = true;

    // Keeping track of the shape
    index = rm::RMShape::shapes.size();
    type = rm::Invalid; // Doesn't get drawn
    materialIndex = 0; // Default material

    // Save this shape
    rm::RMShape::shapes.push_back(this);
}
#pragma endregion

// Drawing - Sending values to shader //
void rm::RMShape::draw(sf::Shader* shader) {
    shader->setUniform("shapes[" + std::to_string(index) + "].position", position);
    shader->setUniform("shapes[" + std::to_string(index) + "].rotation", rotation);
    shader->setUniform("shapes[" + std::to_string(index) + "].param1",   param1  );
    shader->setUniform("shapes[" + std::to_string(index) + "].param2",   param2  );

    shader->setUniform("shapes[" + std::to_string(index) + "].operation",    (int)operation );
    shader->setUniform("shapes[" + std::to_string(index) + "].operandIndex", operandIndex   );
    shader->setUniform("shapes[" + std::to_string(index) + "].checkShape",   checkShape     );

    shader->setUniform("shapes[" + std::to_string(index) + "].type", type);

    // Material properties
    shader->setUniform("shapes[" + std::to_string(index) + "].color",     materials[materialIndex]->albedo   );
    shader->setUniform("shapes[" + std::to_string(index) + "].roughness", materials[materialIndex]->roughness);
    shader->setUniform("shapes[" + std::to_string(index) + "].metallic",  materials[materialIndex]->metallic );
    shader->setUniform("shapes[" + std::to_string(index) + "].emissive",  materials[materialIndex]->emissive );

    // Send any shapes that are now part of this shape to the shader
    if (operandIndex > -1) {
        shapes.at(operandIndex)->draw(shader);
    }
}

// Different Shapes
#pragma region Shape Creation
rm::RMShape* rm::RMShape::createSphere(Vec3 pos, Vec3 rot, float r) {
    RMShape* sphere = new RMShape();
    sphere->setPosition(pos);
    sphere->setRotation(rot);
    sphere->setParam1(Vec3(r, 0, 0));
    sphere->setType(rm::Sphere);

    return sphere;
}

rm::RMShape* rm::RMShape::createBox(Vec3 pos, Vec3 rot, Vec3 size) {
    RMShape* box = new RMShape();
    box->setPosition(pos);
    box->setRotation(rot);
    box->setParam1(size);
    box->setType(rm::Box);

    return box;
}

rm::RMShape* rm::RMShape::createCapsule(Vec3 pos1, Vec3 pos2, float r) {
    RMShape* capsule = new RMShape();
    capsule->setPosition(pos1);
    capsule->setParam1(pos2);
    capsule->setParam2(Vec3(r, 0, 0));
    capsule->setType(rm::Capsule);

    return capsule;
}

rm::RMShape* rm::RMShape::createPlane(Vec3 pos, Vec3 rot, Vec3 n, float h) {
    RMShape* plane = new RMShape();
    plane->setPosition(pos);
    plane->setRotation(rot);
    plane->setParam1(n);
    plane->setParam2(Vec3(h, 0, 0));
    plane->setType(rm::Plane);

    return plane;
}
#pragma endregion

// Different Operations //
// Mostly jsut a wrapper around the setOperation function
#pragma region Shape Operations
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

#pragma endregion

#pragma region Setters & Getters
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
    materials[materialIndex]->albedo = col;
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

void rm::RMShape::setMaterial(RMMaterial& mat) {
    // Find mat if it exists
    for (int i = 0; i < materials.size(); i++) {
        if (*materials[i] == mat) {
            materialIndex = i;
            return;
        }
    }

    materialIndex = materials.size();
    materials.push_back(&mat);
}

// Getters //

Vec3 rm::RMShape::getPosition() {
    return position;
}

Vec3 rm::RMShape::getRotation() {
    return rotation;
}

Vec4 rm::RMShape::getColor() {
    return materials[materialIndex]->albedo;
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

float rm::RMShape::getSignedDistance(Vec3 p)
{
    float signedDistance;
    p = inverseRotateXYZ(p - position, rotation);

    switch (type) {
    case rm::Invalid:
        signedDistance = 0.f;
        break;

    case rm::Sphere:
        signedDistance = VectorHelper::length(p) - param1.x;
        break;

    case rm::Box:
    {
        Vec3 q = Vec3(abs(p.x), abs(p.y), abs(p.z)) - param1;
        signedDistance = VectorHelper::length(VectorHelper::vectorMax(q, Vec3(0, 0, 0))) + fmin(fmax(q.x, fmax(q.y, q.z)), 0.);
    }
        break;

    case rm::Capsule:
    {
        Vec3 pa = p - position;
        Vec3 ba = param1 - position;
        float h = VectorHelper::clamp(VectorHelper::dot(pa, ba) / VectorHelper::dot(ba, ba), 0.0, 1.0);
        signedDistance = VectorHelper::length(pa - ba * h) - param2.x;
    }
        break;

    case rm::Plane:
    {
        Vec3 n = VectorHelper::normalize(param1);
        signedDistance = VectorHelper::dot(p, n) + param2.x;
    }
        break;

    default:
        signedDistance = FLT_MAX;
        break;
    }

    return signedDistance;
}

Vec3 rm::RMShape::getNormal(Vec3 p)
{
    float dist = getSignedDistance(p);

    Vec3 n = Vec3(dist, dist, dist) - Vec3(
        getSignedDistance({ p.x - 0.01f, p.y, p.z }),
        getSignedDistance({ p.x, p.y - 0.01f, p.z }),
        getSignedDistance({ p.x, p.y, p.z - 0.01f })
    );

    return VectorHelper::normalize(n);
}

const rm::RMMaterial& rm::RMShape::getMaterial() {
    return *materials[materialIndex];
}
#pragma endregion

bool rm::RMMaterial::operator==(RMMaterial const& mat) {
    return this == &mat;
}

rm::RMShape* rm::RMShape::raymarch(Vec3 origin, Vec3 direction, float maxDistance, float maxSteps) {
    float totalDistance = 0.f;
    RMShape* closest = nullptr;
    for (int i = 0; i < maxSteps; i++) {
        Vec3 pos = origin + direction * totalDistance;
        float distance = maxDistance;
        for (auto const& rmShape : shapes) {
            float check = rmShape->getSignedDistance(pos);
            if (check < distance) {
                distance = check;
                closest = rmShape;
            }
        }

        if (distance < EPSILON) {
            return closest;
        }

        totalDistance += distance;

        if (totalDistance > maxDistance) {
            return nullptr;
        }
    }

    return nullptr;
}
