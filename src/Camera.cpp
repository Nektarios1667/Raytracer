#include <cmath>

#include "Vector3.h"
#include "Camera.h"
#include "Ray.h"

Camera::Camera() {}

Camera::Camera(const Vector3& from, const Vector3& at, const Vector3& vup, float verticalFov, float aspect) {
    origin = from;

    float theta = verticalFov * M_PI / 180.0f;
    float h = tan(theta / 2.0f);
    float viewportHeight = 2.0f * h;
    float viewportWidth = aspect * viewportHeight;

    Vector3 w = (from - at).normalized();
    Vector3 u = vup.cross(w).normalized();
    Vector3 v = w.cross(u);

    horizontal = u * viewportWidth;
    vertical = v * viewportHeight;
    lowerLeftCorner = origin - horizontal * 0.5f - vertical * 0.5f - w;
}

Ray Camera::getRay(float u, float v) const {
    Vector3 imagePoint = lowerLeftCorner + horizontal * u + vertical * v;
    Vector3 direction = (imagePoint - origin).normalized();
    return Ray(origin, direction);
}