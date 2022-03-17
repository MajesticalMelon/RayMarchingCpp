#define SFML_STATIC
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <conio.h>

#include "RMEnums.h"
#include "RMShape.h"
#include "Rotations.h"

using namespace sf;

Vector3f rotateX(Vector3f p, float theta);
Vector3f rotateY(Vector3f p, float theta);
Vector3f rotateZ(Vector3f p, float theta);
Vector3f rotateXYZ(Vector3f p, Vector3f rot);

Vector3f normalize(Vector3f p);

int main() {
	std::cout << "Creating Window" << std::endl;
	RenderWindow window(VideoMode(800, 600), "Ray Marcher");

	// Inputs
	short userInput = rm::None;

	// Camera
	Vector3f position(0, 1, 0);
	Vector3f rotation(0, 0, 0);
	Vector3f look(0, 0, 1);

	// Ray marcher
	std::cout << "Loading Marcher" << std::endl;
	Shader rayMarchingShader;
	rayMarchingShader.loadFromFile("Marcher.frag", Shader::Type::Fragment);

	// Send initial position and size to shader
	rayMarchingShader.setUniform("camPosition", position);
	rayMarchingShader.setUniform("camRotation", rotation);
	rayMarchingShader.setUniform("windowDimensions", window.getView().getSize());

	std::cout << "Begin Drawing" << std::endl;
	// Some shapes
	RectangleShape screen(Vector2f(window.getSize().x, window.getSize().y));

	// Camera variables
	float walkScalar = 3;
	float rotateScalar = 0.7f;

	// Clock
	Clock gameClock;
	Clock deltaClock;
	float deltaTime = 0;

	// Check for window events
	Event event;

	// Redoing sphere stuff
	rm::RMShape* sphere1 = rm::RMShape::createSphere(
		Glsl::Vec3(0, 0, 0),
		Glsl::Vec3(0, 0, 0),
		Glsl::Vec4(cos(5 * gameClock.getElapsedTime().asSeconds()), 0, sin(2 * gameClock.getElapsedTime().asSeconds()), 0.05),
		0.2
	);

	rm::RMShape* box1 = rm::RMShape::createBox(
		Glsl::Vec3(-2, 3, 0),
		Glsl::Vec3(3, 1, 0),
		Glsl::Vec4(0, 1, 0.7, 0.1),
		Glsl::Vec3(1, 3, 0.01)
	);

	rm::RMShape* sphere2 = rm::RMShape::createSphere(
		Glsl::Vec3(0, 3, 5),
		Glsl::Vec3(0, 0, 0),
		Glsl::Vec4(1, 0, 0, 1),
		2
	);

	sphere1->combine(box1);

	while (window.isOpen()) {
		// Rotate the user's look vector
		look = rotateXYZ(Vector3f(0, 0, 1), rotation);

		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}

			if (event.type == Event::Resized) {
				rayMarchingShader.setUniform("windowDimensions", sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
				screen.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
			}

			// Go in the direction that was pressed
			if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::Escape) {
					window.close();
				}

				// Movement

				// Don't keep adding if already going in that direction
				if (event.key.code == Keyboard::W && (userInput & rm::Forward) != rm::Forward) {
					userInput += rm::Forward;
					break;
				}

				if (event.key.code == Keyboard::A && (userInput & rm::Left) != rm::Left) {
					userInput += rm::Left;
					break;
				}

				if (event.key.code == Keyboard::S && (userInput & rm::Back) != rm::Back) {
					userInput += rm::Back;
					break;
				}

				if (event.key.code == Keyboard::D && (userInput & rm::Right) != rm::Right) {
					userInput += rm::Right;
					break;
				}

				if (event.key.code == Keyboard::Space && (userInput & rm::Up) != rm::Up) {
					userInput += rm::Up;
					break;
				}

				if (event.key.code == Keyboard::LControl && (userInput & rm::Down) != rm::Down) {
					userInput += rm::Down;
					break;
				}

				// Rotation
				if (event.key.code == Keyboard::Right && (userInput & rm::LookRight) != rm::LookRight) {
					userInput += rm::LookRight;
					break;
				}

				if (event.key.code == Keyboard::Left && (userInput & rm::LookLeft) != rm::LookLeft) {
					userInput += rm::LookLeft;
					break;
				}

				if (event.key.code == Keyboard::Up && (userInput & rm::LookUp) != rm::LookUp) {
					userInput += rm::LookUp;
					break;
				}

				if (event.key.code == Keyboard::Down && (userInput & rm::LookDown) != rm::LookDown) {
					userInput += rm::LookDown;
					break;
				}
			}

			// Stop moving in whichever direction was released
			if (event.type == Event::KeyReleased) {

				// Movement
				if (event.key.code == Keyboard::W) {
					userInput -= rm::Forward;
					break;
				}

				if (event.key.code == Keyboard::A) {
					userInput -= rm::Left;
					break;
				}

				if (event.key.code == Keyboard::S) {
					userInput -= rm::Back;
					break;
				}

				if (event.key.code == Keyboard::D) {
					userInput -= rm::Right;
					break;
				}

				if (event.key.code == Keyboard::Space) {
					userInput -= rm::Up;
					break;
				}

				if (event.key.code == Keyboard::LControl) {
					userInput -= rm::Down;
					break;
				}

				// Rotation
				if (event.key.code == Keyboard::Right) {
					userInput -= rm::LookRight;
					break;
				}

				if (event.key.code == Keyboard::Left) {
					userInput -= rm::LookLeft;
					break;
				}

				if (event.key.code == Keyboard::Up) {
					userInput -= rm::LookUp;
					break;
				}

				if (event.key.code == Keyboard::Down) {
					userInput -= rm::LookDown;
					break;
				}

			}
		}

		// Draw the background color
		window.clear(Color::Blue);

		// Start drawing here (Gets redrawn every frame so positions could be modified)
		sphere1->draw(&rayMarchingShader);

		sphere2->draw(&rayMarchingShader);

		// End drawing here
		window.draw(screen, &rayMarchingShader);

		// End the frame and actually draw it
		window.display();

		deltaTime = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		// Update here

		sphere1->setPosition(Glsl::Vec3(-1.5 + cos(0.5 * gameClock.getElapsedTime().asSeconds()), 3, 0));

		// Check userInput
		if ((userInput | rm::None) != rm::None) {

			// Temporarily zero look.y so movement is along a plane
			float tempY = look.y;
			look.y = 0;

			// Movement
			if ((userInput & rm::Forward) == rm::Forward) {
				position += look * deltaTime * walkScalar;
			}

			if ((userInput & rm::Left) == rm::Left) {
				position -= sf::Vector3f(look.z, 0, -look.x) * deltaTime * walkScalar;
			}

			if ((userInput & rm::Back) == rm::Back) {
				position -= look * deltaTime * walkScalar;
			}

			if ((userInput & rm::Right) == rm::Right) {
				position += sf::Vector3f(look.z, 0, -look.x) * deltaTime * walkScalar;
			}

			if ((userInput & rm::Up) == rm::Up) {
				position += sf::Vector3f(0, 1, 0) * deltaTime * walkScalar;
			}

			if ((userInput & rm::Down) == rm::Down) {
				position -= sf::Vector3f(0, 1, 0) * deltaTime * walkScalar;
			}

			// Reassign look
			look.y = tempY;

			// Rotation
			if ((userInput & rm::LookRight) == rm::LookRight) {
				rotation.y += (deltaTime * rotateScalar);
			}

			if ((userInput & rm::LookLeft) == rm::LookLeft) {
				rotation.y -= (deltaTime * rotateScalar);
			}

			if ((userInput & rm::LookUp) == rm::LookUp) {
				rotation.x -= (deltaTime * rotateScalar);
			}

			if ((userInput & rm::LookDown) == rm::LookDown) {
				rotation.x += (deltaTime * rotateScalar);
			}
		}

		rayMarchingShader.setUniform("time", gameClock.getElapsedTime().asSeconds());

		rayMarchingShader.setUniform("camPosition", position);
		rayMarchingShader.setUniform("camRotation", rotation);
	}

	for (rm::RMShape* s : rm::RMShape::shapes) {
		delete s;
	}
}

Vector3f normalize(sf::Vector3f p) {
	float magnitude = sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);

	if (magnitude > 1) {
		return p / magnitude;
	}

	return p;
}