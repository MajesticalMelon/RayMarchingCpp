#pragma once

#include <SFML/Graphics.hpp>

sf::Vector3f rotateX(sf::Vector3f p, float theta);
sf::Vector3f rotateY(sf::Vector3f p, float theta);
sf::Vector3f rotateZ(sf::Vector3f p, float theta);
sf::Vector3f rotateXYZ(sf::Vector3f p, sf::Vector3f rot);
sf::Vector3f rotateZYX(sf::Vector3f p, sf::Vector3f rot);