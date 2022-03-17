#pragma once

#include <SFML/Graphics.hpp>

#include "RMShape.h"

using namespace sf::Glsl;

struct Box {
public:
	RMShape base;
	Vec3 size;
};