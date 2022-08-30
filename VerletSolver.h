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

				std::tie(isCollision, collisionPoint) = checkCollision(*s1->collider, *s2->collider);

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
	/// Recursively checks for collisions by marching from one shape to the other
	/// </summary>
	/// <param name="s1">A RMShape collider</param>
	/// <param name="s2">Another RMShape collider</param>
	/// <param name="startOffset">
	/// The point to start marching from relative to s1's position
	/// (Used for recursion)
	/// </param>
	/// <returns>A pair containing whether a valid collision point was found, and the collision point itself</returns>
	static std::pair<bool, Vector3f> checkCollision(rm::RMShape& s1, rm::RMShape& s2, Vector3f startOffset = Vector3f()) {
		// Grab the colliders since they are used a lot
		float minDist = abs(s2.getSignedDistance(s1.getPosition() + startOffset));

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
		bool possibleCollision = false;
		Vector3f closestOffset = { 0, 0, 0 };

		for (unsigned int i = 0; i < numOffsets; i++) {

			// Make sure the check point is within the shape
			if (s1.getSignedDistance(s1.getPosition() + offsets[i]) > FLT_EPSILON) continue;

			// Find the closest check point
			float dist = s2.getSignedDistance(s1.getPosition() + offsets[i]);
			if (abs(dist) < minDist) {
				minDist = dist;
				closestOffset = offsets[i];
				possibleCollision = true;
			}
		}

		// Grab the found collision point
		Vector3f collisionPoint = s1.getPosition() + closestOffset;

		// Recursively march toward a possible collision point
		if (possibleCollision && minDist > FLT_EPSILON) {
			if (minDist > FLT_EPSILON) {
				return checkCollision(s1, s2, closestOffset);
			}

			// A collision point will be on the edge of s1 too
			possibleCollision = s1.getSignedDistance(collisionPoint) < FLT_EPSILON;
		}

		// Give back the found results
		return std::make_pair(possibleCollision, collisionPoint);
	}
};