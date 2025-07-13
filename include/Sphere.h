#pragma once
#include "Vector3.h"
#include "Color.h"
#include "Ray.h"
#include "Hittable.h"

class Sphere : public Hittable {
public:
    Vector3 center;
    float radius;
    float radiusSquared;

    Sphere();
    Sphere(const Vector3& center, float radius, Color color, float reflectivity, float fuzz);

    bool isPointInside(const Vector3& point) const override;
    bool intersectsRay(const Ray& rayOrigin, float& outT) const override;
    Vector3 getNormalAt(const Vector3& point) const override;
};