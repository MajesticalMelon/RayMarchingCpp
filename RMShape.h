#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

using namespace sf::Glsl;

#include "RMEnums.h"

namespace rm {
    
    namespace VectorHelper {
        float length(Vec3 p);

        Vec3 vectorAbs(Vec3 p);

        Vec3 vectorMax(Vec3 p, Vec3 q);

        Vec3 vectorMin(Vec3 p, Vec3 q);

        float dot(Vec3 p, Vec3 q);

        float clamp(float val, float low, float high);

        Vec3 normalize(Vec3 p);
    }

    class RMShape {
    private:
        Vec3 position;
        Vec3 rotation;
        Vec4 color;
        Vec3 param1;
        Vec3 param2;
        Vec3 origin;

        // Used for combining, subtracting, intersecting, etc. two shapes
        int operation;
        int operandIndex;
        bool checkShape;

        // Shape type and index
        int index;
        int type;

        void setType(ShapeType t);
        void setOperation(Operation t, RMShape* opd);

    public:
        RMShape();

        void draw(sf::Shader* shader);

        void setPosition(Vec3 pos);
        void setRotation(Vec3 pos);
        void setColor(Vec4 col);
        void setParam1(Vec3 p1);
        void setParam2(Vec3 p2);
        void setVisible(bool visible);
        void setOrigin(Vec3 orig);

        void combine(RMShape* opd);
        void intersection(RMShape* opd);
        void subtract(RMShape* opd);
        void smoothCombine(RMShape* opd);
        void smoothIntersection(RMShape* opd);
        void smoothSubtract(RMShape* opd);

        Vec3 getPosition();
        Vec3 getRotation();
        Vec4 getColor();
        Vec3 getParam1();
        Vec3 getParam2();
        rm::ShapeType getType();
        int getIndex();

        float getSignedDistance(Vec3 p);
        Vec3 getNormal(Vec3 p);

        static std::vector<RMShape*> shapes;

        static RMShape* createSphere(Vec3 pos, Vec3 rot, Vec4 col, float r);
        static RMShape* createBox(Vec3 pos, Vec3 rot, Vec4 col, Vec3 size);
        static RMShape* createCapsule(Vec3 pos1, Vec3 pos2, Vec4 col, float r);
        // Infinite plane defined by its normal vector, n and offset from the origin, h
        static RMShape* createPlane(Vec3 pos, Vec3 rot, Vec4 col, Vec3 n, float h);
    };
}