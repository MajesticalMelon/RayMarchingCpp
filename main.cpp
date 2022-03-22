#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "main.h"

#include "RMShape.h"
#include "RMEnums.h"
#include "Rotations.h"

// Global variables
float gameTime;

int userInput;

// Camera
sf::Vector3f position(0, 1, 0);
sf::Vector3f rotation(0, 0, 0);
sf::Vector3f look(0, 0, 1);

// Camera variables
float walkScalar = 3;
float rotateScalar = 0.7f;

// Shapes
rm::RMShape* sphere1;
rm::RMShape* box1;
rm::RMShape* sphere2;
rm::RMShape* line;

// Initilization of global variables
void init() {
	gameTime = 0;

	userInput == rm::None;

	// Redoing sphere stuff
	sphere1 = rm::RMShape::createSphere(
		sf::Glsl::Vec3(-1.5f, 3, 0),
		sf::Glsl::Vec3(0, 0, 0),
		sf::Glsl::Vec4(cos(5), 0, sin(2), 0.05),
		0.5f
	);

	box1 = rm::RMShape::createBox(
		sf::Glsl::Vec3(-2, 3, 0),
		sf::Glsl::Vec3(3, 1, 0),
		sf::Glsl::Vec4(0, 1, 0.7, 0.1),
		sf::Glsl::Vec3(1, 3, 0.2f)
	);

	sphere2 = rm::RMShape::createSphere(
		sf::Glsl::Vec3(0, 3, 5),
		sf::Glsl::Vec3(0, 0, 0),
		sf::Glsl::Vec4(1, 0, 0, 1),
		2
	);

	line = rm::RMShape::createCapsule(
		sf::Glsl::Vec3(-2, 5, 2),
		sf::Glsl::Vec3(2, 5, 2),
		sf::Glsl::Vec3(0, 0, 0),
		sf::Glsl::Vec4(0.2, 0.69, 0.42, 1),
		0.5
	);

	// Form a union of sphere1 and box1
	box1->smoothCombine(sphere1);
}

void draw(sf::RenderWindow* window, sf::Shader* shader, sf::RectangleShape screen) {
	// Send camera's position and rotation to the shader
	shader->setUniform("camPosition", position);
	shader->setUniform("camRotation", rotation);

	// Send variables to the shader
	shader->setUniform("camPosition", position);
	shader->setUniform("camRotation", rotation);

	// Draw the background color
	window->clear(sf::Color::Blue);

	// Start drawing here (Gets redrawn every frame so positions could be modified)
	box1->draw(shader);

	sphere2->draw(shader);

	line->draw(shader);

	// End drawing here
	window->draw(screen, shader);
}

void update(sf::Clock* gameClock) {
	// Grab deltaTime
	float deltaTime = gameClock->getElapsedTime().asSeconds();

	// Update total game time
	gameTime += deltaTime;

	// Rotate the user's look vector
	look = rotateXYZ(sf::Vector3f(0, 0, 1), rotation);

	sf::Glsl::Vec3 spherePos = sphere1->getPosition();
	spherePos.x = -1.5 + cos(0.5 * gameTime);
	sphere1->setPosition(spherePos);

	line->setRotation(Vec3(0, gameTime, 0));

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
}

void keyPressed(sf::Event* event) {
	// Movement

	// Don't keep adding if already going in that direction
	if (event->key.code == sf::Keyboard::W && (userInput & rm::Forward) != rm::Forward) {
		userInput += rm::Forward;
		return;
	}

	if (event->key.code == sf::Keyboard::A && (userInput & rm::Left) != rm::Left) {
		userInput += rm::Left;
		return;
	}

	if (event->key.code == sf::Keyboard::S && (userInput & rm::Back) != rm::Back) {
		userInput += rm::Back;
		return;
	}

	if (event->key.code == sf::Keyboard::D && (userInput & rm::Right) != rm::Right) {
		userInput += rm::Right;
		return;
	}

	if (event->key.code == sf::Keyboard::Space && (userInput & rm::Up) != rm::Up) {
		userInput += rm::Up;
		return;
	}

	if (event->key.code == sf::Keyboard::LControl && (userInput & rm::Down) != rm::Down) {
		userInput += rm::Down;
		return;
	}

	// Rotation
	if (event->key.code == sf::Keyboard::Right && (userInput & rm::LookRight) != rm::LookRight) {
		userInput += rm::LookRight;
		return;
	}

	if (event->key.code == sf::Keyboard::Left && (userInput & rm::LookLeft) != rm::LookLeft) {
		userInput += rm::LookLeft;
		return;
	}

	if (event->key.code == sf::Keyboard::Up && (userInput & rm::LookUp) != rm::LookUp) {
		userInput += rm::LookUp;
		return;
	}

	if (event->key.code == sf::Keyboard::Down && (userInput & rm::LookDown) != rm::LookDown) {
		userInput += rm::LookDown;
		return;
	}
}

void keyReleased(sf::Event* event) {
	// Movement
	if (event->key.code == sf::Keyboard::W) {
		userInput -= rm::Forward;
		return;
	}

	if (event->key.code == sf::Keyboard::A) {
		userInput -= rm::Left;
		return;
	}

	if (event->key.code == sf::Keyboard::S) {
		userInput -= rm::Back;
		return;
	}

	if (event->key.code == sf::Keyboard::D) {
		userInput -= rm::Right;
		return;
	}

	if (event->key.code == sf::Keyboard::Space) {
		userInput -= rm::Up;
		return;
	}

	if (event->key.code == sf::Keyboard::LControl) {
		userInput -= rm::Down;
		return;
	}

	// Rotation
	if (event->key.code == sf::Keyboard::Right) {
		userInput -= rm::LookRight;
		return;
	}

	if (event->key.code == sf::Keyboard::Left) {
		userInput -= rm::LookLeft;
		return;
	}

	if (event->key.code == sf::Keyboard::Up) {
		userInput -= rm::LookUp;
		return;
	}

	if (event->key.code == sf::Keyboard::Down) {
		userInput -= rm::LookDown;
		return;
	}
}