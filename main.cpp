#define SFML_STATIC
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <conio.h>

sf::RenderWindow* window;

int main() {
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

		while (window->pollEvent(event)) {
			// Close the window if the user tries to
			if (event.type == sf::Event::Closed) {
				window->close();
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