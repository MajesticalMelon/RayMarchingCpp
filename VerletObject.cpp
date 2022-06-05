#include "VerletObject.h"

std::vector<VerletObject*> VerletObject::verletObjects = std::vector<VerletObject*>();

VerletObject::VerletObject(Vector3f pos)
{
	positionCurrent = pos;
	positionOld = pos;
	velocity = Vector3f();
	acceleration = Vector3f();
	// Default collider is a sphere
	collider = rm::RMShape::createSphere(pos, { 0, 0, 0 }, { 1, 0, 0, 1 }, 1);

	verletObjects.push_back(this);
}

VerletObject::VerletObject(rm::RMShape* shape)
{
	positionCurrent = shape->getPosition();
	positionOld = positionCurrent;
	velocity = Vector3f();
	acceleration = Vector3f();
	collider = shape;

	verletObjects.push_back(this);
}

VerletObject::~VerletObject()
{
	// Does nothing for now
}

void VerletObject::update(float deltaTime)
{
	// Calculate velocity
	velocity = positionCurrent - positionOld;

	// Save position
	positionOld = positionCurrent;

	// Update position
	positionCurrent = positionCurrent + velocity + acceleration * deltaTime * deltaTime;
	acceleration = Vector3f(); // Reset acceleration

	// Update collider
	collider->setPosition(positionCurrent);
}

void VerletObject::accelerate(Vector3f acc)
{
	acceleration += acc;
}

Vector3f VerletObject::getPosition()
{
	return positionCurrent;
}
