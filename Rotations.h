#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

Vector3f rotateX(Vector3f p, float theta);
Vector3f rotateY(Vector3f p, float theta);
Vector3f rotateZ(Vector3f p, float theta);
Vector3f rotateXYZ(Vector3f p, Vector3f rot);