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
	None		= 0b0000000000,
	Forward		= 0b0000000001,
	Left		= 0b0000000010,
	Right		= 0b0000000100,
	Back		= 0b0000001000,
	Up			= 0b0000010000,
	Down		= 0b0000100000,
	LookLeft	= 0b0001000000,
	LookRight	= 0b0010000000,
	LookUp		= 0b0100000000,
	LookDown	= 0b1000000000,
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

	while (window->isOpen()) {
		rayMarchingShader.setUniform("time", gameClock.getElapsedTime().asSeconds());

		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window->close();
			}

			// Go in the direction that was pressed
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) {
					window->close();
				}

				// Movement

				// Don't keep adding if already going in that direction
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

				// Rotation
				if (event.key.code == sf::Keyboard::Right && (userInput & LookRight) != LookRight) {
					userInput += LookRight;
					break;
				}

				if (event.key.code == sf::Keyboard::Left && (userInput & LookLeft) != LookLeft) {
					userInput += LookLeft;
					break;
				}

				if (event.key.code == sf::Keyboard::Up && (userInput & LookUp) != LookUp) {
					userInput += LookUp;
					break;
				}

				if (event.key.code == sf::Keyboard::Down && (userInput & LookDown) != LookDown) {
					userInput += LookDown;
					break;
				}
			}

			// Stop moving in whichever direction was released
			if (event.type == sf::Event::KeyReleased) {

				// Movement
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

				// Rotation
				if (event.key.code == sf::Keyboard::Right) {
					userInput -= LookRight;
					break;
				}

				if (event.key.code == sf::Keyboard::Left) {
					userInput -= LookLeft;
					break;
				}

				if (event.key.code == sf::Keyboard::Up) {
					userInput -= LookUp;
					break;
				}

				if (event.key.code == sf::Keyboard::Down) {
					userInput -= LookDown;
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
			// Movement
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

			// Rotation
			if ((userInput & LookRight) == LookRight) {
				rotation.y += deltaTime;
			}

			if ((userInput & LookLeft) == LookLeft) {
				rotation.y -= deltaTime;
			}

			if ((userInput & LookUp) == LookUp) {
				rotation.x -= deltaTime;
			}

			if ((userInput & LookDown) == LookDown) {
				rotation.x += deltaTime;
			}

			rayMarchingShader.setUniform("camRotation", rotation);
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