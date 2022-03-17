#include "RMShape.h"
#include "RMEnums.h"

RMShape::RMShape() {
    position = Vec3(0, 0, 0);
    rotation = Vec3(0, 0, 0);
    color = Vec4(0, 0, 0, 1);
    param1 = Vec3(0, 0, 0);
    param2 = Vec3(0, 0, 0);

    // Used for combining, subtracting, intersecting, etc. two shapes
    operation = rm::None;
    operandIndex = -1;
    checkShape = true;
}