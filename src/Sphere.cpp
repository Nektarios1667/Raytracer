#include "Sphere.h"
#include "Color.h"
#include "Ray.h"

Sphere::Sphere() : center(Vector3()), radius(1), radiusSquared(1) {
    this->color = Color();
    this->reflectivity = 0;
    this-> fuzz = 0;
}
Sphere::Sphere(const Vector3& center, float radius, Color color, float reflectivity, float fuzz) : center(center), radius(radius), radiusSquared(radius*radius) {
    this->color = color;
    this->reflectivity = reflectivity;
    this->fuzz = fuzz;
}
bool Sphere::isPointInside(const Vector3& point) const {
    return point.distanceSquared(center) < radiusSquared;
}
bool Sphere::intersectsRay(const Ray& ray, float& outT) const {
    Vector3 oc = ray.origin - center;

    float b = 2.0f * oc.dot(ray.direction);
    float c = oc.dot(oc) - radiusSquared;
    float discriminant = b * b - 4 * c;

    if (discriminant < 0.0f) return false;

    float sqrtD = sqrt(discriminant);
    float t1 = (-b - sqrtD) / 2.0f;
    float t2 = (-b + sqrtD) / 2.0f;

    float t = (t1 >= 0.0f) ? t1 : ((t2 >= 0.0f) ? t2 : -1);
    if (t < 0.0f) return false;

    outT = t;
    return true;
}
Vector3 Sphere::getNormalAt(const Vector3& point) const {
    return (point - center).normalized();
}

