#define SFML_STATIC
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <conio.h>

#include "Rotations.h"
#include "Shape.h"
#include "Sphere.h"

using namespace sf;

Vector3f rotateX(Vector3f p, float theta);
Vector3f rotateY(Vector3f p, float theta);
Vector3f rotateZ(Vector3f p, float theta);
Vector3f rotateXYZ(Vector3f p, Vector3f rot);

Vector3f normalize(Vector3f p);

void drawSphere(Glsl::Vec3 pos, Glsl::Vec3 rot, Glsl::Vec4 col, float rad);

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

// Shape stuff
std::vector<Sphere> spheres;

int main() {
	std::cout << "Creating Window" << std::endl;
	RenderWindow window(VideoMode(800, 600), "Ray Marcher");

	// Inputs
	short userInput = Down;

	// Camera
	Vector3f position(0, 1, 0);
	Vector3f rotation(0, 0, 0);
	Vector3f look(0, 0, 1);

	// Ray marcher
	std::cout << "Loading Marcher" << std::endl;
	Shader rayMarchingShader;
	rayMarchingShader.loadFromFile("Marcher.frag", Shader::Type::Fragment);

	// Send initial position to shader
	rayMarchingShader.setUniform("camPosition", position);
	rayMarchingShader.setUniform("camRotation", rotation);

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

	while (window.isOpen()) {
		// Rotate the user's look vector
		look = rotateXYZ(Vector3f(0, 0, 1), rotation);

		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}

			// Go in the direction that was pressed
			if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::Escape) {
					window.close();
				}

				// Movement

				// Don't keep adding if already going in that direction
				if (event.key.code == Keyboard::W && (userInput & Forward) != Forward) {
					userInput += Forward;
					break;
				}

				if (event.key.code == Keyboard::A && (userInput & Left) != Left) {
					userInput += Left;
					break;
				}

				if (event.key.code == Keyboard::S && (userInput & Back) != Back) {
					userInput += Back;
					break;
				}

				if (event.key.code == Keyboard::D && (userInput & Right) != Right) {
					userInput += Right;
					break;
				}

				// Rotation
				if (event.key.code == Keyboard::Right && (userInput & LookRight) != LookRight) {
					userInput += LookRight;
					break;
				}

				if (event.key.code == Keyboard::Left && (userInput & LookLeft) != LookLeft) {
					userInput += LookLeft;
					break;
				}

				if (event.key.code == Keyboard::Up && (userInput & LookUp) != LookUp) {
					userInput += LookUp;
					break;
				}

				if (event.key.code == Keyboard::Down && (userInput & LookDown) != LookDown) {
					userInput += LookDown;
					break;
				}
			}

			// Stop moving in whichever direction was released
			if (event.type == Event::KeyReleased) {

				// Movement
				if (event.key.code == Keyboard::W) {
					userInput -= Forward;
					break;
				}

				if (event.key.code == Keyboard::A) {
					userInput -= Left;
					break;
				}

				if (event.key.code == Keyboard::S) {
					userInput -= Back;
					break;
				}

				if (event.key.code == Keyboard::D) {
					userInput -= Right;
					break;
				}

				// Rotation
				if (event.key.code == Keyboard::Right) {
					userInput -= LookRight;
					break;
				}

				if (event.key.code == Keyboard::Left) {
					userInput -= LookLeft;
					break;
				}

				if (event.key.code == Keyboard::Up) {
					userInput -= LookUp;
					break;
				}

				if (event.key.code == Keyboard::Down) {
					userInput -= LookDown;
					break;
				}

			}
		}

		// Draw the background color
		window.clear(Color::Black);

		// Start drawing here (Gets redrawn every frame so positions could be modified)
		drawSphere(Glsl::Vec3(5 * cos(gameClock.getElapsedTime().asSeconds()), 3, 5 * sin(gameClock.getElapsedTime().asSeconds())), Glsl::Vec3(0, 0, 0), Glsl::Vec4(0, 0, 1, 1), 3);

		// End drawing here
		window.draw(screen, &rayMarchingShader);

		// End the frame and actually draw it
		window.display();

		deltaTime = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		// Update here

		// Check userInput
		if ((userInput |  None) !=  None) {

			// Movement
			if ((userInput &  Forward) ==  Forward) {
				position += look * deltaTime * walkScalar;
			}

			if ((userInput & Left) == Left) {
				position -= sf::Vector3f(look.z, 0, -look.x) * deltaTime * walkScalar;
			}

			if ((userInput & Back) == Back) {
				position -= look * deltaTime * walkScalar;
			}

			if ((userInput & Right) == Right) {
				position += sf::Vector3f(look.z, 0, -look.x) * deltaTime * walkScalar;
			}

			// Rotation
			if ((userInput & LookRight) == LookRight) {
				rotation.y += (deltaTime * rotateScalar);
			}

			if ((userInput & LookLeft) == LookLeft) {
				rotation.y -= (deltaTime * rotateScalar);
			}

			if ((userInput & LookUp) == LookUp) {
				rotation.x -= (deltaTime * rotateScalar);
			}

			if ((userInput & LookDown) == LookDown) {
				rotation.x += (deltaTime * rotateScalar);
			}
		}

		rayMarchingShader.setUniform("time", gameClock.getElapsedTime().asSeconds());

		rayMarchingShader.setUniform("camPosition", position);
		rayMarchingShader.setUniform("camRotation", rotation);

		for (int i = 0; i < spheres.size(); i++) {

			Sphere s = spheres.at(i);

			rayMarchingShader.setUniform("spheres[" + std::to_string(i) + "].base.position", s.base.position);
			rayMarchingShader.setUniform("spheres[" + std::to_string(i) + "].base.rotation", s.base.rotation);
			rayMarchingShader.setUniform("spheres[" + std::to_string(i) + "].base.color",    s.base.color	);
			rayMarchingShader.setUniform("spheres[" + std::to_string(i) + "].radius",        s.radius	    );
		}

		rayMarchingShader.setUniform("numSpheres", (int)spheres.size());

		spheres.clear();
	}
}

Vector3f normalize(sf::Vector3f p) {
	float magnitude = sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);

	if (magnitude > 1) {
		return p / magnitude;
	}

	return p;
}

void drawSphere(Glsl::Vec3 pos, Glsl::Vec3 rot, Glsl::Vec4 col, float rad) {
	Sphere sphere;
	sphere.base.position = pos;
	sphere.base.rotation = rot;
	sphere.base.color = col;
	sphere.radius = rad;

	spheres.push_back(sphere);
}