#pragma once
#include "Vector3.h"
#include "Ray.h"
#include "Color.h"

class Hittable {
    public:
        Color color;
        float fuzz;
        float reflectivity;

        virtual bool isPointInside(const Vector3& point) const = 0;
        virtual bool intersectsRay(const Ray& rayOrigin, float& outT) const = 0;
        virtual Vector3 getNormalAt(const Vector3& point) const = 0;

        virtual ~Hittable() = default;
};
