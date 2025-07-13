#pragma once
#include "Vector3.h"
#include "Ray.h"

class Camera {
    public:
        Vector3 origin;
        Vector3 lowerLeftCorner;
        Vector3 horizontal;
        Vector3 vertical;
        Camera();
        Camera(const Vector3& from, const Vector3& at, const Vector3& vup, float verticalFov, float aspect);
        Ray getRay(float u, float v) const;
    };