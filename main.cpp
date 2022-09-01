#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "main.h"

#include "RMShape.h"
#include "RMEnums.h"
#include "Rotations.h"
#include "VerletObject.h"
#include "VerletSolver.h"

// Global variables
float gameTime;

int userInput;

// Camera
sf::Vector3f position(0, 0.1f, 0);
sf::Vector3f rotation(0, 0, 0);
sf::Vector3f look(0, 0, 1);
sf::Vector3f right(1, 0, 0);
sf::Vector3f forward(0, 0, 1);

// Camera variables
float walkScalar = 3;
float rotateScalar = 0.7f;

// Shapes
rm::RMShape* sphere1;
rm::RMShape* box1;
rm::RMShape* sphere2;
rm::RMShape* line;
rm::RMShape* ground;
rm::RMShape* sphere3;
rm::RMShape* box2;

// Verlet Objects
VerletObject* testSphere;
VerletObject* testSphere2;
VerletObject* testPlane;
VerletObject* testBox;
VerletObject* wall1;
VerletObject* wall2;
VerletObject* wall3;
VerletObject* wall4;

// Initilization of global variables
void init() {
	gameTime = 0;

	userInput = rm::None;

	// Redoing sphere stuff
	sphere1 = rm::RMShape::createSphere(
		sf::Glsl::Vec3(-1.5f, 3, 0),
		sf::Glsl::Vec3(0, 0, 0),
		sf::Glsl::Vec4(cosf(5), 0, sinf(2), 0.1f),
		0.5f
	);

	box1 = rm::RMShape::createBox(
		sf::Glsl::Vec3(-2, 3, 0),
		sf::Glsl::Vec3(3, 1, 0),
		sf::Glsl::Vec4(0, 1, 0.5f, 0.1f),
		sf::Glsl::Vec3(1, 3, 0.2f)
	);

	sphere2 = rm::RMShape::createSphere(
		sf::Glsl::Vec3(0, 10, 5),
		sf::Glsl::Vec3(0, 0, 0),
		sf::Glsl::Vec4(1, 0, 0, 1),
		2
	);

	line = rm::RMShape::createCapsule(
		sf::Glsl::Vec3(5, 0.5, 0),
		sf::Glsl::Vec3(3, 3, 2),
		sf::Glsl::Vec4(0.2f, 0.69f, 0.42f, 1.f),
		0.1f
	);

	ground = rm::RMShape::createPlane(
		sf::Glsl::Vec3(0, 0, 0),
		sf::Glsl::Vec3(0, 0, 0),
		sf::Glsl::Vec4(0.65, 0.65, 0.7, 1),
		sf::Glsl::Vec3(0, 1, 0),
		0
	);

	sphere3 = rm::RMShape::createSphere(
		sf::Glsl::Vec3(1, 3, 2.5f),
		sf::Glsl::Vec3(0, 0, 0),
		sf::Glsl::Vec4(0.7f, 0.2f, 0.5f, 1),
		1.f
	);

	box2 = rm::RMShape::createBox(
		sf::Glsl::Vec3(1, 3, 2.5f),
		sf::Glsl::Vec3(1, 0.2f, 0.7f),
		sf::Glsl::Vec4(0, 1, 0, 1),
		sf::Glsl::Vec3(1, 2, 0.5f)
	);

	// Form a union of sphere1 and box1
	box1->smoothCombine(sphere1);
	box1->setOrigin(sf::Glsl::Vec3(-5, 0, 0));

	//testSphere2 = new VerletObject(sphere3);
	testSphere = new VerletObject(sphere2);
	testBox = new VerletObject(box2);
	testPlane = new VerletObject(ground, true);

	printf("%f\n", box1->getSignedDistance(Vec3()));
}

void draw(sf::Shader* shader, sf::RectangleShape screen) {

	// Send variables to the shader
	shader->setUniform("camPosition", position);
	shader->setUniform("camRotation", rotation);

	// Start drawing here (Gets redrawn every frame so positions could be modified)
	box1->draw(shader);

	sphere2->draw(shader);

	sphere3->draw(shader);

	line->draw(shader);

	box2->draw(shader);

	ground->draw(shader);
}

void update(sf::Clock* gameClock) {
	// Grab deltaTime
	float deltaTime = gameClock->getElapsedTime().asSeconds();

	// Update total game time
	gameTime += deltaTime;

	// Rotate the user's look vector
	look = rotateXYZ(sf::Vector3f(0, 0, 1), rotation);
	right = rotateXYZ(sf::Vector3f(1, 0, 0), sf::Vector3f(0, rotation.y, 0));
	forward = rotateXYZ(sf::Vector3f(0, 0, 1), sf::Vector3f(0, rotation.y, 0));

	/*sf::Glsl::Vec3 spherePos = sphere1->getPosition();
	spherePos.x = -1.5f + cosf(0.5f * gameTime);
	sphere1->setPosition(spherePos);

	box1->setRotation(sf::Glsl::Vec3(
		0,
		gameTime,
		0
	));*/

	// Physics updates
	//VerletSolver::update(deltaTime);

	//sphere2->setPosition(testSphere->getPosition());

	// Check userInput
	if ((userInput | rm::None) != rm::None) {

		// Temporarily zero look.y so movement is along a plane
		float tempY = look.y;
		look.y = 0;

		// Movement
		if ((userInput & rm::Forward) == rm::Forward) {
			position += forward * deltaTime * walkScalar;
		}

		if ((userInput & rm::Left) == rm::Left) {
			position -= right * deltaTime * walkScalar;
		}

		if ((userInput & rm::Back) == rm::Back) {
			position -= forward * deltaTime * walkScalar;
		}

		if ((userInput & rm::Right) == rm::Right) {
			position += right * deltaTime * walkScalar;
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