#pragma once

#include <thread>
#include <future>

#include <SFML/System.hpp>
#include "VerletObject.h"
#include "RMShape.h"

using namespace sf;

struct VerletSolver {
	static void update(float deltaTime) {

		const int subSteps = 4;
		const float subDelta = deltaTime / (float)subSteps;
		const unsigned int numChecks = 2;

		for (unsigned int step = 0; step < subSteps; step++) {
			// Gravity
			applyGravity();

			solveCollsions();

			// Update positions
			for (VerletObject* vo : VerletObject::verletObjects) {
				vo->update(subDelta);
			}
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
					Vector3f moveDir;
					float dist = abs(s1->collider->getSignedDistance(collisionPoint));
					if (!s1->isStatic) {
						moveDir = s2->collider->getNormal(collisionPoint);
						s1->positionCurrent += moveDir * dist;
					}

					if (!s2->isStatic) {
						moveDir = s1->collider->getNormal(collisionPoint);
						s2->positionCurrent += moveDir * dist;
					}
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
	/// <returns>Whether there was a collision, Point of collision</returns>
	static std::pair<bool, Vector3f> checkCollision(const VerletObject& o1, const VerletObject& o2, Vector3f startOffset = Vector3f()) {
		rm::RMShape s1 = *o1.collider;
		rm::RMShape s2 = *o2.collider;
		float minDist = s2.getSignedDistance(s1.getPosition() + startOffset);

		const unsigned int numOffsets = 14;
		Vector3f offsets[numOffsets] = {
			{startOffset.x + minDist, startOffset.y, startOffset.z}, {startOffset.x - minDist, startOffset.y, startOffset.z},
			{startOffset.x, startOffset.y + minDist, startOffset.z}, {startOffset.x, startOffset.y - minDist, startOffset.z},
			{startOffset.x, startOffset.y, startOffset.z + minDist}, {startOffset.x, startOffset.y, startOffset.z - minDist},
			rm::VectorHelper::normalize( {startOffset.x + minDist, startOffset.y + minDist, startOffset.z + minDist}),
			-rm::VectorHelper::normalize({startOffset.x + minDist, startOffset.y + minDist, startOffset.z + minDist}),
			rm::VectorHelper::normalize({startOffset.x + minDist, startOffset.y, startOffset.z}),
			-rm::VectorHelper::normalize({startOffset.x + minDist, startOffset.y, startOffset.z}),
			rm::VectorHelper::normalize({startOffset.x, startOffset.y + minDist, startOffset.z}),
			-rm::VectorHelper::normalize({startOffset.x, startOffset.y + minDist, startOffset.z}),
			rm::VectorHelper::normalize({startOffset.x, startOffset.y, startOffset.z + minDist}),
			-rm::VectorHelper::normalize({startOffset.x, startOffset.y, startOffset.z + minDist}),
		};

		// Assume there is no collision until proven otherwise
		bool isCollision = false;
		Vector3f closestOffset = { 0, 0, 0 };

		for (unsigned int i = 0; i < numOffsets; i++) {

			// Make sure the check point is within the shape
			if (s1.getSignedDistance(s1.getPosition() + offsets[i]) > 0.1f) continue;

			// Find the closest check point
			float dist = s2.getSignedDistance(s1.getPosition() + offsets[i]);
			if (abs(dist) < minDist) {
				minDist = dist;
				closestOffset = offsets[i];
				isCollision = true;
			}
		}

		if (isCollision && minDist > 0.f) {
			return checkCollision(o1, o2, closestOffset);
		}

		Vector3f collisionPoint = s1.getPosition() + closestOffset;
		if (s1.getSignedDistance(collisionPoint) >= 0.f) {
			isCollision = false;
		}

		return std::make_pair(isCollision, collisionPoint);
	}
};