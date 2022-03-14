#pragma once
#include <SFML/Graphics.hpp>

using namespace sf::Glsl;

struct RMShape {
public:
	Vec3 position = Vec3(0, 0, 0);
	Vec3 rotation = Vec3(0, 0, 0);
	Vec4 color = Vec4(0, 0, 0, 1);
};