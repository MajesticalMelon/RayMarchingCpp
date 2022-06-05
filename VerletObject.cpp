#include "VerletObject.h"

std::vector<VerletObject*> VerletObject::verletObjects = std::vector<VerletObject*>();

VerletObject::VerletObject(Vector3f pos)
{
	positionCurrent = pos;
	positionOld = pos;
	velocity = Vector3f();
	acceleration = Vector3f();

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
}

void VerletObject::accelerate(Vector3f acc)
{
	acceleration += acc;
}

Vector3f VerletObject::getPosition()
{
	return positionCurrent;
}
