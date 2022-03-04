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
	sf::Clock gameClock;
	sf::Clock deltaClock;
	float deltaTime = 0;

	// Check for window events
	sf::Event event;

	sf::Mouse::setPosition(window->getPosition() + sf::Vector2i(400, 300));
	sf::Vector2i prevMousePos;

	while (window->isOpen()) {
		rayMarchingShader.setUniform("time", gameClock.getElapsedTime().asSeconds());

		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window->close();
			}

			if (event.type == sf::Event::MouseMoved && prevMousePos.x != window->getPosition().x + 400) {
				printf("%d, %d\n", event.mouseMove.x, prevMousePos.x);
				rotation.y += (event.mouseMove.x - prevMousePos.x) * deltaTime * 0.1f;
				rotation.x += (event.mouseMove.y - prevMousePos.y) * deltaTime * 0.1f;

				rayMarchingShader.setUniform("camRotation", rotation);

				prevMousePos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
			}

			// Go in the direction that was pressed
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) {
					window->close();
				}

				// Don;t keep adding if already going in that direction
				if (event.key.code == sf::Keyboard::W && (userInput & Forward) != Forward) {
					userInput += Forward;
					break;
				}

				if (event.key.code == sf::Keyboard::A && (userInput & Left) != Left) {
					userInput += Left;
					break;
				}

				if (event.key.code == sf::Keyboard::S && (userInput & Back) != Back) {
					userInput += Back;
					break;
				}

				if (event.key.code == sf::Keyboard::D && (userInput & Right) != Right) {
					userInput += Right;
					break;
				}
			}

			// Stop moving in whichever direction was released
			if (event.type == sf::Event::KeyReleased) {
				if (event.key.code == sf::Keyboard::W) {
					userInput -= Forward;
					break;
				}

				if (event.key.code == sf::Keyboard::A) {
					userInput -= Left;
					break;
				}

				if (event.key.code == sf::Keyboard::S) {
					userInput -= Back;
					break;
				}

				if (event.key.code == sf::Keyboard::D) {
					userInput -= Right;
					break;
				}
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

	rotated = rotateZ(rotated, rot.z);
	rotated = rotateY(rotated, rot.y);
	rotated = rotateX(rotated, rot.x);

	return rotated;
}