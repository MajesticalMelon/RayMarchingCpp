#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <conio.h>

#include "main.h"

#include "RMEnums.h"
#include "RMShape.h"
#include "Rotations.h"

using namespace sf;

int main() {
	std::cout << "Creating Window" << std::endl;
	RenderWindow window(VideoMode(800, 600), "Ray Marcher");

	// Inputs
	short userInput = rm::None;

	// Ray marcher
	std::cout << "Loading Marcher" << std::endl;
	Shader rayMarchingShader;
	rayMarchingShader.loadFromFile("Marcher.frag", Shader::Type::Fragment);

	// Send initial size to shader
	rayMarchingShader.setUniform("windowDimensions", window.getView().getSize());

	// Load texture
	Texture testTexture;
	testTexture.loadFromFile("testTexture.jpg");
	rayMarchingShader.setUniform("testTexture", testTexture);

	std::cout << "Begin Drawing" << std::endl;
	// Some shapes
	RectangleShape screen(Vector2f(window.getSize().x, window.getSize().y));

	// Clock
	Clock gameClock;
	Clock deltaClock;
	float deltaTime = 0;

	// Initializes global variable within main.cpp before starting
	init();

	// Check for window events
	Event event;

	while (window.isOpen()) {

		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}

			if (event.key.code == sf::Keyboard::Escape) {
				window.close();
			}

			// Dynamically change the size of the window
			if (event.type == Event::Resized) {
				rayMarchingShader.setUniform("windowDimensions", sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
				screen.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
			}

			// Go in the direction that was pressed
			if (event.type == Event::KeyPressed) {
				keyPressed(&event);
			}

			// Stop moving in whichever direction was released
			if (event.type == Event::KeyReleased) {
				keyReleased(&event);
			}
		}

		// Draw the scene
		draw(&window, &rayMarchingShader, screen);

		// End the frame and actually draw it to the window
		window.display();

		// Update here
		update(&deltaClock);

		// Reset clock for calculating delta time
		deltaClock.restart();

		// Send the current running time to the shader
		rayMarchingShader.setUniform("time", gameClock.getElapsedTime().asSeconds());
	}

	// Cleanup any shapes that were created
	for (rm::RMShape* s : rm::RMShape::shapes) {
		delete s;
	}
}