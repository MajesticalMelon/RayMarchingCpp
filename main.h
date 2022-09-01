#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

void init();

void keyPressed(sf::Event* event);

void keyReleased(sf::Event* event);

void draw(sf::Shader* shader, sf::RectangleShape screen);

void update(sf::Clock* gameClock);