#include "Rotations.h"

Vector3f rotateX(Vector3f p, float theta) {
	return Vector3f(
		p.x,
		p.y * cos(theta) - p.z * sin(theta),
		p.y * sin(theta) + p.z * cos(theta)
	);
}

Vector3f rotateY(Vector3f p, float theta) {
	return Vector3f(
		p.x * cos(theta) + p.z * sin(theta),
		p.y,
		-p.x * sin(theta) + p.z * cos(theta)
	);
}

Vector3f rotateZ(Vector3f p, float theta) {
	return Vector3f(
		p.x * cos(theta) - p.y * sin(theta),
		p.x * sin(theta) + p.y * cos(theta),
		p.z
	);
}

Vector3f rotateXYZ(Vector3f p, Vector3f rot) {
	Vector3f rotated = p;

	rotated = rotateX(rotated, rot.x);
	rotated = rotateY(rotated, rot.y);
	rotated = rotateZ(rotated, rot.z);

	return rotated;
}