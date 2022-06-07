#pragma once

#include <SFML/System.hpp>
#include "VerletObject.h"
#include "RMShape.h"

using namespace sf;

struct VerletSolver {
	static void update(float deltaTime) {

		const int subSteps = 2;
		const float subDelta = deltaTime / (float)subSteps;
		// Gravity
		applyGravity();

		solveCollsions();

		// Update positions
		for (VerletObject* vo : VerletObject::verletObjects) {
			vo->update(deltaTime);
		}
	}

	static void applyGravity() {
		const Vector3f GRAVITY = { 0, -9.8f, 0 };
		for (VerletObject* vo : VerletObject::verletObjects) {
			if (vo->isStatic) continue;
			vo->accelerate(GRAVITY);
		}
	}

	static void solveCollsions() {
		for (unsigned int i = 0; i < VerletObject::verletObjects.size(); i++) {
			VerletObject* s1 = VerletObject::verletObjects[i];
			for (unsigned int j = i; j < VerletObject::verletObjects.size(); j++) {
				// Don't check collisions against the same two objects
				if (i == j) continue;

				VerletObject* s2 = VerletObject::verletObjects[j];

				// Collision variable
				bool isCollision;
				Vector3f collisionPoint;

				std::tie(isCollision, collisionPoint) = checkCollision(*s1, *s2);

				if (isCollision) {
					/*Vector3f moveDir = s2->collider->getNormal(collisionPoint) * abs(s1->collider->getSignedDistance(collisionPoint));
					if (!s1->isStatic) {
						s1->positionCurrent += moveDir;
					}

					if (!s2->isStatic) {
						s2->positionCurrent -= moveDir;
					}*/

					printf("Collision Point: { %f, %f, %f }\n", collisionPoint.x, collisionPoint.y, collisionPoint.z);
				}
			}
		}
	}

	/// <summary>
	/// Checks for collisions between two RMShapes (Colliders)
	/// -- order doesn't matter
	/// </summary>
	/// <param name="s1">Shape 1</param>
	/// <param name="s2">Shape 2</param>
	/// <returns>Whether there was a collision, Point of collision, Minimum intersection distance</returns>
	static std::tuple<bool, Vector3f> checkCollision(VerletObject o1, VerletObject o2) {
		rm::RMShape s1 = *o1.collider;
		rm::RMShape s2 = *o2.collider;
		float minDist = s2.getSignedDistance(s1.getPosition());
		Vector3f offsets[6] = {
			{minDist, 0, 0}, {-minDist, 0, 0},
			{0, minDist, 0}, {0, -minDist, 0},
			{0, 0, minDist}, {0, 0, -minDist}
		};

		// Assume there is one until proven otherwise
		bool isCollision = false;
		Vector3f closestOffset;

		do {
			closestOffset = { 0, 0, 0 };
			isCollision = false;

			for (int i = 0; i < 6; i++) {

				// Make sure the check point is within the shape
				if (s1.getSignedDistance(s1.getPosition() + offsets[i]) > 0.1f) continue;

				// Find the closest check point
				float dist = s2.getSignedDistance(s1.getPosition() + offsets[i]);
				if (dist < minDist) {
					minDist = dist;
					closestOffset = offsets[i];
					isCollision = true;
				}
			}

			// Remake offsets based on the closest offset
			offsets[0] = { closestOffset.x + minDist, closestOffset.y, closestOffset.z };
			offsets[1] = { closestOffset.x - minDist, closestOffset.y, closestOffset.z };
			offsets[2] = { closestOffset.x, closestOffset.y + minDist, closestOffset.z };
			offsets[3] = { closestOffset.x, closestOffset.y - minDist, closestOffset.z };
			offsets[4] = { closestOffset.x, closestOffset.y, closestOffset.z + minDist };
			offsets[5] = { closestOffset.x, closestOffset.y, closestOffset.z - minDist };


		} while (minDist > 0.f && isCollision);

		// TODO: Determine correct collision point more accurately

		Vector3f collisionPoint = s1.getPosition() + closestOffset;

		if (s1.getSignedDistance(collisionPoint) >= 0.f) {
			isCollision = false;
		}

		return std::make_tuple(isCollision, collisionPoint);
	}

	static std::tuple<bool, Vector3f> checkCollisionWithStartOffset(VerletObject o1, VerletObject o2, Vector3f startOffset) {
		rm::RMShape s1 = *o1.collider;
		rm::RMShape s2 = *o2.collider;
		float minDist = s2.getSignedDistance(s1.getPosition() + startOffset);
		Vector3f offsets[6] = {
			{minDist, 0, 0}, {-minDist, 0, 0},
			{0, minDist, 0}, {0, -minDist, 0},
			{0, 0, minDist}, {0, 0, -minDist}
		};

		// Assume there is one until proven otherwise
		bool isCollision = false;
		Vector3f closestOffset;

		do {
			closestOffset = { 0, 0, 0 };
			isCollision = false;

			for (int i = 0; i < 6; i++) {

				// Make sure the check point is within the shape
				if (s1.getSignedDistance(s1.getPosition() + offsets[i]) > 0.1f) continue;

				// Find the closest check point
				float dist = s2.getSignedDistance(s1.getPosition() + offsets[i]);
				if (dist < minDist) {
					minDist = dist;
					closestOffset = offsets[i];
					isCollision = true;
				}
			}

			// Remake offsets based on the closest offset
			offsets[0] = { closestOffset.x + minDist, closestOffset.y, closestOffset.z };
			offsets[1] = { closestOffset.x - minDist, closestOffset.y, closestOffset.z };
			offsets[2] = { closestOffset.x, closestOffset.y + minDist, closestOffset.z };
			offsets[3] = { closestOffset.x, closestOffset.y - minDist, closestOffset.z };
			offsets[4] = { closestOffset.x, closestOffset.y, closestOffset.z + minDist };
			offsets[5] = { closestOffset.x, closestOffset.y, closestOffset.z - minDist };


		} while (minDist > 0.f && isCollision);

		// TODO: Determine correct collision point more accurately

		Vector3f collisionPoint = s1.getPosition() + closestOffset;

		if (s1.getSignedDistance(collisionPoint) >= 0.f) {
			isCollision = false;
		}

		return std::make_tuple(isCollision, collisionPoint);
	}
};