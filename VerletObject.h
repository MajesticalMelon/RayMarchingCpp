#pragma once;

#include <vector>
#include <SFML/System.hpp>

#include "RMShape.h"

using namespace sf;

class VerletObject {
private:
	friend struct VerletSolver;

	Vector3f positionCurrent;
	Vector3f positionOld;
	Vector3f velocity;
	Vector3f acceleration;
	rm::RMShape* collider;

public:
	static std::vector<VerletObject*> verletObjects;

	VerletObject(Vector3f pos);
	VerletObject(rm::RMShape* shape);
	~VerletObject();
	void update(float deltaTime);
	void accelerate(Vector3f acc);
	Vector3f getPosition();
};