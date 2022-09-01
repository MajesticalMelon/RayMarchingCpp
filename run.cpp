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
	// Scene window
	std::cout << "Creating Window" << std::endl;
	RenderWindow window(VideoMode(1000, 750), "Ray Marcher");
	RenderTexture scene;
	scene.create(window.getSize().x, window.getSize().y);
	scene.clear(Color::Black);

	// Inputs
	short userInput = rm::None;

	// Ray marcher
	std::cout << "Loading Marcher" << std::endl;
	Shader rayMarchingShader;
	rayMarchingShader.loadFromFile("Marcher.frag", Shader::Type::Fragment);

	// Send initial size to shader
	rayMarchingShader.setUniform("windowDimensions", window.getView().getSize());

	Shader fxaaShader;
	fxaaShader.loadFromFile("FXAA.frag", Shader::Type::Fragment);
	fxaaShader.setUniform("windowDimensions", sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));

	// Load texture(s)
	Texture skybox;
	skybox.loadFromFile("alps_field_4k.hdr");
	rayMarchingShader.setUniform("skybox", skybox);

	// Buffer texture for progressive rendering
	Texture buffer;
	buffer.create(window.getSize().x, window.getSize().y);
	buffer.update(window);
	rayMarchingShader.setUniform("buff", buffer);

	std::cout << "Begin Drawing" << std::endl;
	// Some shapes
	RectangleShape screen(Vector2f(
		(float)window.getSize().x, 
		(float)window.getSize().y
	));

	// Clock
	Clock gameClock;
	Clock deltaClock;

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
				fxaaShader.setUniform("windowDimensions", sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
				screen.setSize(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
				buffer.create(window.getSize().x, window.getSize().y);
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

		// Update the buffer
		//buffer.update(window);
		rayMarchingShader.setUniform("buff", scene.getTexture());

		// Draw the scene (Sends objects to the shader)
		draw(&rayMarchingShader, screen);

		// Ray march
		scene.draw(screen, &rayMarchingShader);

		// End the frame and actually draw it to the window
		window.clear(Color::Black);
		window.draw(Sprite(scene.getTexture()));
		window.display();

		// Update here
		update(&deltaClock);


		// Reset clock for calculating delta time
		deltaClock.restart();

		// Send the current running time to the shader
		rayMarchingShader.setUniform("time", gameClock.getElapsedTime().asSeconds());
		rayMarchingShader.setUniform("deltaTime", deltaClock.getElapsedTime().asSeconds());

	}

	// Cleanup any shapes that were created
	for (rm::RMShape* s : rm::RMShape::shapes) {
		delete s;
	}
}