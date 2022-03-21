#pragma once

namespace rm {
	enum Input {
		None = 0b0000000000,
		Forward = 0b0000000001,
		Left = 0b0000000010,
		Right = 0b0000000100,
		Back = 0b0000001000,
		Up = 0b0000010000,
		Down = 0b0000100000,
		LookLeft = 0b0001000000,
		LookRight = 0b0010000000,
		LookUp = 0b0100000000,
		LookDown = 0b1000000000,
	};

	enum Operation {
		NoOp,
		Union,
		Intersection,
		Subtract,
		SmoothUnion,
		SmoothIntersection,
		SmoothSubtract
	};

	enum ShapeType {
		Invalid,
		Sphere,
		Box,
		Capsule
	};
}