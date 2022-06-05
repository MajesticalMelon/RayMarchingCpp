#pragma once

#include <SFML/System.hpp>
#include "VerletObject.h"

using namespace sf;

static struct VerletSolver {
	static void update(float deltaTime) {
		// Gravity
		applyGravity();

		// Update positions
		for (VerletObject* vo : VerletObject::verletObjects) {
			vo->update(deltaTime);
		}
	}

	static void applyGravity() {
		const Vector3f GRAVITY = { 0, -1.2f, 0 };
		for (VerletObject* vo : VerletObject::verletObjects) {
			vo->accelerate(GRAVITY);
		}
	}
};