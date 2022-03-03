#define SFML_STATIC
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <conio.h>

sf::Vector3f rotateX(sf::Vector3f p, float theta);
sf::Vector3f rotateY(sf::Vector3f p, float theta);
sf::Vector3f rotateZ(sf::Vector3f p, float theta);
sf::Vector3f rotateXYZ(sf::Vector3f p, sf::Vector3f rot);

sf::RenderWindow* window;

enum Input {
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
	short userInput = Down;

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
	
	sf::Vector3f position(0, 1, 0);
	sf::Vector3f rotation(0, 0, 0);
	sf::Vector3f look(0, 0, 1);

	std::cout << shapeTypes[0] << std::endl;

	std::cout << "Initializing Marcher" << std::endl;
	sf::Shader rayMarchingShader;
	rayMarchingShader.loadFromFile("Marcher.frag", sf::Shader::Type::Fragment);

	// Load shapeTypes into the shader
	rayMarchingShader.setUniformArray("shapeTypes", shapeTypes, 100);

	// Send initial position to shader
	rayMarchingShader.setUniform("camPosition", position);
	rayMarchingShader.setUniform("camRotation", rotation);

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

	sf::Mouse::setPosition(window->getPosition() + sf::Vector2i(400, 300));

	while (window->isOpen()) {

		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window->close();
			}

			// Start off with no input
			if (event.type == sf::Event::KeyReleased) {
				userInput = (short)None;
			}

			if (event.type == sf::Event::MouseMoved) {
				rotation.y += (event.mouseMove.x - 400) * deltaTime * 0.0001f;
				rotation.x += (event.mouseMove.y - 300) * deltaTime * 0.0001f;

				rayMarchingShader.setUniform("camRotation", rotation);

				//sf::Mouse::setPosition(window->getPosition() + sf::Vector2i(400, 300));
			}

			// TODO: Fix hitching when switching quickly between inputs
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) {
					window->close();
				}

				if (event.key.code == sf::Keyboard::W && (userInput & Forward) != Forward) {
					userInput += Forward;
				}

				if (event.key.code == sf::Keyboard::A && (userInput & (short)Left) != (short)Left) {
					userInput += (short)Left;
				}

				if (event.key.code == sf::Keyboard::S) {
					userInput += (short)Back;
				}

				if (event.key.code == sf::Keyboard::D) {
					userInput = (short)Right;
				}
			}
		}

		printf("%f, %f\n", rotation.x, rotation.y);

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
		if ((userInput |  None) !=  None) {
			if ((userInput &  Forward) ==  Forward) {
				position += look * deltaTime;
			}

			if ((userInput & Left) == Left) {
				position -= sf::Vector3f(look.z, 0, -look.x) * deltaTime;
			}

			if ((userInput & Back) == Back) {
				position -= look * deltaTime;
			}

			if ((userInput & Right) == Right) {
				position += sf::Vector3f(look.z, 0, -look.x) * deltaTime;
			}

			rayMarchingShader.setUniform("camPosition", position);
		}
	}

	delete window;
}

sf::Vector3f rotateX(sf::Vector3f p, float theta) {
	return sf::Vector3f(
		p.x,
		p.y * cos(theta) - p.z * sin(theta),
		p.y * sin(theta) + p.z * cos(theta)
	);
}

sf::Vector3f rotateY(sf::Vector3f p, float theta) {
	return sf::Vector3f(
		p.x * cos(theta) + p.z * sin(theta),
		p.y,
		-p.x * sin(theta) + p.z * cos(theta)
	);
}

sf::Vector3f rotateZ(sf::Vector3f p, float theta) {
	return sf::Vector3f(
		p.x * cos(theta) - p.y * sin(theta),
		p.x * sin(theta) + p.y * cos(theta),
		p.z
	);
}

sf::Vector3f rotateXYZ(sf::Vector3f p, sf::Vector3f rot) {
	sf::Vector3f rotated = p;

	rotated = rotateX(rotated, rot.x);
	rotated = rotateY(rotated, rot.y);
	rotated = rotateZ(rotated, rot.z);

	return rotated;
}