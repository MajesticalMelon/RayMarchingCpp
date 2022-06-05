#pragma once;

#include <vector>
#include <SFML/System.hpp>

using namespace sf;

class VerletObject {
private:
	Vector3f positionCurrent;
	Vector3f positionOld;
	Vector3f velocity;
	Vector3f acceleration;

public:
	static std::vector<VerletObject*> verletObjects;

	VerletObject(Vector3f pos);
	~VerletObject();
	void update(float deltaTime);
	void accelerate(Vector3f acc);
	Vector3f getPosition();
};