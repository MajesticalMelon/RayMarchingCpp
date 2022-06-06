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

		solveCollsions();
	}

	static void applyGravity() {
		const Vector3f GRAVITY = { 0, -1.2f, 0 };
		for (VerletObject* vo : VerletObject::verletObjects) {
			vo->accelerate(GRAVITY);
		}
	}

	static void solveCollsions() {
		for (int i = 0; i < VerletObject::verletObjects.size(); i++) {
			VerletObject s1 = *VerletObject::verletObjects[i];
			for (int j = 0; j < VerletObject::verletObjects.size(); j++) {
				// Don't check collisions against the same two objects
				if (i == j) continue;

				VerletObject s2 = *VerletObject::verletObjects[j];

				if (checkCollision(*s1.collider, *s2.collider)) {

				}
			}
		}
	}

	static bool checkCollision(rm::RMShape s1, rm::RMShape s2) {
		float minDist = s2.getSignedDistance(s1.getPosition());
		float* offset = &minDist;
		Vector3f offsets[6] = {
			{*offset, 0, 0}, {-*offset, 0, 0},
			{0, *offset, 0}, {0, -*offset, 0},
			{0, 0, *offset}, {0, 0, -*offset}
		};
		while (minDist > 0.01f) {
			for (int i = 0; i < 6; i++) {
				// TODO: Break out of loop if all points are outside of shape

				// Make sure the check point is within the shape
				if (s1.getSignedDistance(s1.getPosition() + offsets[i]) > 0.01f) continue;

				// Find the closest check point
				minDist = fmin(minDist, s2.getSignedDistance(s1.getPosition() + offsets[i]));
			}

			printf("%f\n", offsets[0].x);
		}

		return true;
	}
};