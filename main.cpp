#define SFML_STATIC
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <conio.h>

sf::RenderWindow* window;

int main() {
	bool moveLeft = false;
	bool moveRight = false;
	bool moveForward = false;
	bool moveBackward = false;

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
	
	sf::Vector3f position(0, 0, 0);

	std::cout << shapeTypes[0] << std::endl;

	std::cout << "Initializing Marcher" << std::endl;
	sf::Shader rayMarchingShader;
	rayMarchingShader.loadFromFile("Marcher.frag", sf::Shader::Type::Fragment);

	// Load shapeTypes into the shader
	rayMarchingShader.setUniformArray("shapeTypes", shapeTypes, 100);

	std::cout << "Creating Window" << std::endl;
	// Create the window
	window = new sf::RenderWindow(sf::VideoMode(800, 600), "Ray Marcher");

	std::cout << "Begin Drawing" << std::endl;
	// Some shapes
	sf::RectangleShape screen(sf::Vector2f(window->getSize().x, window->getSize().y));

	// Clock
	sf::Clock deltaClock;

	// Check for window events
	sf::Event event;

	// Run until the window is closed
	while (window->isOpen()) {

		if (moveForward == true) {
			position.z += 10000 * deltaClock.getElapsedTime().asSeconds();
			rayMarchingShader.setUniform("camPosition", position);
		}

		if (moveLeft == true) {
			position.x -= 10000 * deltaClock.getElapsedTime().asSeconds();
			rayMarchingShader.setUniform("camPosition", position);
		}

		if (moveRight == true) {
			position.x += 10000 * deltaClock.getElapsedTime().asSeconds();
			rayMarchingShader.setUniform("camPosition", position);
		}

		if (moveBackward == true) {
			position.z -= 10000 * deltaClock.getElapsedTime().asSeconds();
			rayMarchingShader.setUniform("camPosition", position);
		}

		while (window->pollEvent(event)) {
			// Close the window if the user tries to
			if (event.type == sf::Event::Closed) {
				window->close();
			}

			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::W) {
					moveForward = true;
				}
			}

			if (event.type == sf::Event::KeyReleased) {
				if (event.key.code == sf::Keyboard::W) {
					moveForward = false;
				}
			}

			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::A) {
					moveLeft = true;
				}
			}

			if (event.type == sf::Event::KeyReleased) {
				if (event.key.code == sf::Keyboard::A) {
					moveLeft = false;
				}
			}

			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::D) {
					moveRight = true;
				}
			}

			if (event.type == sf::Event::KeyReleased) {
				if (event.key.code == sf::Keyboard::D) {
					moveRight = false;
				}
			}

			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::S) {
					moveBackward = true;
				}
			}

			if (event.type == sf::Event::KeyReleased) {
				if (event.key.code == sf::Keyboard::S) {
					moveBackward = false;
				}
			}
		}

		// Draw the background color
		window->clear(sf::Color::Black);

		// Draw here
		window->draw(screen, &rayMarchingShader);

		// End the frame and actually draw it
		window->display();
		
		deltaClock.restart();
	}

	delete window;
}