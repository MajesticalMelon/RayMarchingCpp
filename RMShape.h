#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

using namespace sf::Glsl;

#include "RMEnums.h"

namespace rm {
    typedef float (*SDF)(Vec3);

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

        // Signed Distance Field function pointer
        rm::SDF signedDistance;
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

        static std::vector<RMShape*> shapes;

        static RMShape* createSphere(Vec3 pos, Vec3 rot, Vec4 col, float r);
        static RMShape* createBox(Vec3 pos, Vec3 rot, Vec4 col, Vec3 size);
        static RMShape* createCapsule(Vec3 pos1, Vec3 pos2, Vec4 col, float r);
        // Infinite plane defined by its normal vector, n and offset from the origin, h
        static RMShape* createPlane(Vec3 pos, Vec3 rot, Vec4 col, Vec3 n, float h);

    // Helper functions
    private:
        float length(Vec3 p) {
            return sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);
        }

        Vec3 vectorAbs(Vec3 p) {
            return Vec3(abs(p.x), abs(p.y), abs(p.z));
        }

        Vec3 vectorMax (Vec3 p, Vec3 q) {
            return Vec3(fmax(p.x, q.x), fmax(p.y, q.y), fmax(p.z, q.z));
        }

        Vec3 vectorMin(Vec3 p, Vec3 q) {
            return Vec3(fmin(p.x, q.x), fmin(p.y, q.y), fmin(p.z, q.z));
        }

        float dot(Vec3 p, Vec3 q) {
            return p.x * q.x + p.y * q.y + p.y * q.y;
        }

        float clamp(float val, float low, float high) {
            return fmax(fmin(val, high), low);
        }

        Vec3 normalize(Vec3 &p) {
            p /= length(p);
        }

        rm::SDF assignSDF() {
            switch (type) {
            case rm::Invalid:
                signedDistance = (SDF&) [](Vec3 p) -> Vec3 {
                    return Vec3();
                };
                break;
            case rm::Sphere:
                signedDistance = (SDF&) [this](Vec3 p) {
                    return length(p) - param1.x;
                };
                break;
            case rm::Box:
                signedDistance = (SDF&)  [this](Vec3 p) {
                    Vec3 q = Vec3(abs(p.x), abs(p.y), abs(p.z)) - param1;
                    return length(vectorMax(q, Vec3(0, 0, 0))) + fmin(fmax(q.x, fmax(q.y, q.z)), 0.);
                };
                break;
            case rm::Capsule:
                signedDistance = (SDF&) [this](Vec3 p) {
                    Vec3 pa = p - position;
                    Vec3 ba = param1 - position;
                    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
                    return length(pa - ba * h) - param2.x;
                };
                break;
            case rm::Plane:
                return (SDF&) [this](Vec3 p) {
                    Vec3 n = normalize(param1);
                    return dot(p, n) + param2.x;
                };
                break;
            default:
                break;
            }

            printf("%f\n", signedDistance(Vec3()));
        }
    };
}