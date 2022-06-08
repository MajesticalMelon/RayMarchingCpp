#pragma once

#include <thread>
#include <future>

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
	static std::pair<bool, Vector3f> checkCollision(VerletObject o1, VerletObject o2) {
		std::vector<std::thread> checkThreads;
		std::vector<std::promise<std::pair<bool, Vector3f>>> promises;
		std::vector<std::future<std::pair<bool, Vector3f>>> results;

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

		for (int i = 0; i < 6; i++) {
			// Make sure the check point is within the shape
			if (s1.getSignedDistance(s1.getPosition() + offsets[i]) > 0.f) continue;

			// Check for collisions starting at all points that are within the shape

			// Doing some multithreading
			promises.push_back(std::promise<std::pair<bool, Vector3f>>());
			results.push_back(promises[promises.size() - 1].get_future());
			checkThreads.push_back(std::thread(&checkCollisionWithStartOffset, o1, o2, offsets[i], std::move(promises[promises.size() - 1])));
		}

		for (std::thread& t : checkThreads) {
			t.join();
		}

		float collisionDist = FLT_MAX;
		Vector3f collisionPoint;
		for (std::future<std::pair<bool, Vector3f>>& futures : results) {
			std::pair<bool, Vector3f> result = futures.get();
			if (!result.first) continue;

			isCollision = true;
			float dist = abs(s1.getSignedDistance(result.second));
			if (dist < collisionDist) {
				collisionDist = dist;
				collisionPoint = result.second;
			}
		}

		// If there are elements in results then collisions were detected
		return std::make_pair(isCollision, collisionPoint);
	}

	// Different checking
	static void checkCollisionWithStartOffset(VerletObject o1, VerletObject o2, Vector3f startOffset, std::promise<std::pair<bool, Vector3f>> promise) {
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

		Vector3f collisionPoint = s1.getPosition() + closestOffset;

		if (s1.getSignedDistance(collisionPoint) >= 0.f) {
			isCollision = false;
		}

		promise.set_value(std::make_pair(isCollision, collisionPoint));
	}
};