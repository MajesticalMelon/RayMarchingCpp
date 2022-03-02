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

	// Run until the window is closed
	while (window->isOpen()) {
		// Check for window events
		sf::Event event;

		while (window->pollEvent(event)) {
			// Close the window if the user tries to
			if (event.type == sf::Event::Closed) {
				window->close();
			}

			if (event.type == sf::Event::KeyPressed) {

				switch (event.key.code)
				{
				case sf::Keyboard::W:
					moveForward == true;
					break;
				case sf::Keyboard::A:
					moveLeft == true;
					break;
				case sf::Keyboard::S:
					moveBackward == true;
					break;
				case sf::Keyboard::D:
					moveRight == true;
					break;
				default:
					break;
				}
			} else if (event.type == sf::Event::KeyReleased) {

				switch (event.key.code)
				{
				case sf::Keyboard::W:
					moveForward == false;
					break;
				case sf::Keyboard::A:
					moveLeft == false;
					break;
				case sf::Keyboard::S:
					moveBackward == false;
					break;
				case sf::Keyboard::D:
					moveRight == false;
					break;
				default:
					break;
				}
			}

			if (moveForward) {
				position.z += 0.1f;
			}

			if (moveLeft) {
				position.x -= 0.1f;
			}

			if (moveBackward) {
				position.z -= 0.1f;
			}

			if (moveRight) {
				position.x += 0.1f;
			}

			rayMarchingShader.setUniform("camPosition", position);

			// Draw the background color
			window->clear(sf::Color::Black);

			// Draw here
			window->draw(screen, &rayMarchingShader);

			// End the frame and actually draw it
			window->display();
		}
	}

	delete window;
}