#define SFML_STATIC
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <conio.h>

sf::RenderWindow* window;

enum class Input {
	None	= 0b000000,
	Forward = 0b000001,
	Left	= 0b000010,
	Right	= 0b000100,
	Back	= 0b001000,
	Up		= 0b010000,
	Down	= 0b100000
};

int main() {
	// Inputs
	short userInput = (short)Input::Down;

	// Different kinds of shapes
	float* shapeTypes = new float[100];

	// Each type starts at -1 unless modified
	for (int i = 0; i < 100; i++) {
		if (i == 0) {
			shapeTypes[i] = 1;
			continue;
		}
		shapeTypes[i] = -1;
	}
	
	sf::Vector3f position(1, 0, 0);

	std::cout << shapeTypes[0] << std::endl;

	std::cout << "Initializing Marcher" << std::endl;
	sf::Shader rayMarchingShader;
	rayMarchingShader.loadFromFile("Marcher.frag", sf::Shader::Type::Fragment);

	// Load shapeTypes into the shader
	rayMarchingShader.setUniformArray("shapeTypes", shapeTypes, 100);

	// Send initial position to shader
	rayMarchingShader.setUniform("camPosition", position);

	std::cout << "Creating Window" << std::endl;
	// Create the window
	window = new sf::RenderWindow(sf::VideoMode(800, 600), "Ray Marcher");

	std::cout << "Begin Drawing" << std::endl;
	// Some shapes
	sf::RectangleShape screen(sf::Vector2f(window->getSize().x, window->getSize().y));

	// Clock
	sf::Clock deltaClock;
	float deltaTime = 0;

	// Check for window events
	sf::Event event;

	// Run until the window is closed
	while (window->isOpen()) {
		while (window->pollEvent(event)) {
			// Close the window if the user tries to
			if (event.type == sf::Event::Closed) {
				window->close();
			}

			// Start off with no input
			if (event.type == sf::Event::KeyReleased) {
				userInput = (short)Input::None;
			}

			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::W && (userInput & (short)Input::Forward) != (short)Input::Forward) {
					userInput += (short)Input::Forward;
				}

				if (event.key.code == sf::Keyboard::A && (userInput & (short)Input::Left) != (short)Input::Left) {
					userInput += (short)Input::Left;
				}

				if (event.key.code == sf::Keyboard::S) {
					userInput += (short)Input::Back;
				}

				if (event.key.code == sf::Keyboard::D) {
					userInput = (short)Input::Right;
				}

				printf("%i", userInput);
			}
		}

		// Draw the background color
		window->clear(sf::Color::Black);

		// Draw here
		window->draw(screen, &rayMarchingShader);

		// End the frame and actually draw it
		window->display();

		deltaTime = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		// Update here

		// Check userInput
		if (userInput & Input::None != Input::None) {
			if (userInput & Input::Forward == Input::Forward) {
				position.z += 1 * deltaTime;
			}

			if (userInput & Input::Left == Input::Left) {
				position.x -= 1 * deltaTime;
			}

			if (userInput & Input::Back == Input::Back) {
				position.z -= 1 * deltaTime;
			}

			if (userInput & Input::Right == Input::Right) {
				position.x += 1 * deltaTime;
			}

			rayMarchingShader.setUniform("camPosition", position);
		}
	}

	delete window;
}