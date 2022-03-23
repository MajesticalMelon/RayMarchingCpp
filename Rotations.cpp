#include "Rotations.h"

using namespace sf;

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
	
	Transform trans;
	priv::Matrix<3, 3> rotation(trans);

	rotation.array[0] = cos(rot.z) * cos(rot.y);
	rotation.array[1] = sin(rot.z) * cos(rot.y);
	rotation.array[2] = -sin(rot.y);

	rotation.array[3] = cos(rot.z) * sin(rot.y) * sin(rot.x) - sin(rot.z) * cos(rot.x);
	rotation.array[4] = sin(rot.z) * sin(rot.y) * sin(rot.x) + cos(rot.z) * cos(rot.x);
	rotation.array[5] = cos(rot.y) * sin(rot.x);

	rotation.array[6] = cos(rot.z) * sin(rot.y) * cos(rot.x) + sin(rot.z) * sin(rot.x);
	rotation.array[7] = sin(rot.z) * sin(rot.y) * cos(rot.x) - cos(rot.z) * sin(rot.x);
	rotation.array[8] = cos(rot.y) * cos(rot.x);

	Vector3f rotatedPoint;
	rotatedPoint.x = p.x * rotation.array[0] + p.y * rotation.array[3] + p.z * rotation.array[6];
	rotatedPoint.y = p.x * rotation.array[1] + p.y * rotation.array[4] + p.z * rotation.array[7];
	rotatedPoint.z = p.x * rotation.array[2] + p.y * rotation.array[5] + p.z * rotation.array[8];

	return rotatedPoint;
}

Vector3f rotateZYX(Vector3f p, Vector3f rot) {

	Transform trans;
	priv::Matrix<3, 3> rotation(trans);

	rotation.array[0] = cos(rot.x) * cos(rot.y);
	rotation.array[1] = sin(rot.x) * cos(rot.y);
	rotation.array[2] = -sin(rot.y);

	rotation.array[3] = cos(rot.x) * sin(rot.y) * sin(rot.z) - sin(rot.x) * cos(rot.z);
	rotation.array[4] = sin(rot.x) * sin(rot.y) * sin(rot.z) + cos(rot.x) * cos(rot.z);
	rotation.array[5] = cos(rot.y) * sin(rot.z);

	rotation.array[6] = cos(rot.x) * sin(rot.y) * cos(rot.z) + sin(rot.x) * sin(rot.z);
	rotation.array[7] = sin(rot.x) * sin(rot.y) * cos(rot.z) - cos(rot.x) * sin(rot.z);
	rotation.array[8] = cos(rot.y) * cos(rot.z);

	Vector3f rotatedPoint;
	rotatedPoint.x = p.x * rotation.array[0] + p.y * rotation.array[3] + p.z * rotation.array[6];
	rotatedPoint.y = p.x * rotation.array[1] + p.y * rotation.array[4] + p.z * rotation.array[7];
	rotatedPoint.z = p.x * rotation.array[2] + p.y * rotation.array[5] + p.z * rotation.array[8];

	return rotatedPoint;
}