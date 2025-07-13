#include <iostream>
#include <chrono>
#include <cmath>
#include <fstream>
#include <windows.h>
#include <cfloat>
#include <vector>
#include <memory>
#include <random>
#include <sstream>

#include "Ray.h"
#include "Camera.h"
#include "Color.h"
#include "Sphere.h"
#include "Light.h"
#include "Utilities.h"
#include "lodepng.h"

using namespace std;
using namespace std::chrono;

constexpr unsigned MAX_DEPTH = 3;
constexpr unsigned SAMPLES_PER_PIXEL = 4; // Anti-aliasing

enum class ImageQuality {
    Low = 640,
    Medium = 1280,
    High = 1920,
    VeryHigh = 3840,
    Ultra = 7680,
};

Color traceRay(const Ray& ray, const vector<unique_ptr<Hittable>>& scene, const vector<Light>& lights, const Light& ambientLight, int depth = 0) {
    if (depth > MAX_DEPTH)  // max recursion depth
        return Color(ray.direction.x + 1 / 2, ray.direction.y + 1 / 2, ray.direction.z + 1 / 2).corrected().inverted();

    // Hit objects
    float closestT = FLT_MAX;
    const Hittable* hitObject = nullptr;

    for (const auto& item : scene) {
        float t;
        if (item->intersectsRay(ray, t) && t < closestT) {
            closestT = t;
            hitObject = item.get();
        }
    }

    // Skybox
    if (!hitObject) {
        return Color(ray.direction.x + 1 / 2, ray.direction.y + 1 / 2, ray.direction.z + 1 / 2).corrected().inverted();  // sky box
    }

    Vector3 hitPoint = ray.at(closestT);
    Vector3 normal = hitObject->getNormalAt(hitPoint);

    // Ambient light
    Color pixelColor = hitObject->color * ambientLight.color * ambientLight.intensity;

    // Add lights up
    for (const Light& light : lights) {
        // Setup
        Vector3 lightVec = light.position - hitPoint;
        float distanceSquared = lightVec.lengthSquared();
        Vector3 lightDir = lightVec.normalized();
        
        // Cast a ray to the light
        Ray shadowRay(hitPoint + lightDir * 0.001f, lightDir);

        // Check blocks
        bool shadow = false;
        float t;
        for (const auto& item : scene) {
            if (item.get() != hitObject && item->intersectsRay(shadowRay, t)) {
                shadow = true;
                break;
            }
        }
        if (shadow) continue;

        // Apply light
        float lambert = max(0.0f, normal.dot(lightDir));
        float attenuation = 1 / (distanceSquared + FLT_MIN);

        pixelColor += hitObject->color * light.color * light.intensity * lambert * attenuation;
    }

    // No reflectivity - return normal value
    if (hitObject->reflectivity <= 1e-4f) {
        return pixelColor;
    }

    // Reflect
    Vector3 reflectedDir = ray.direction - normal * 2 * ray.direction.dot(normal) + Utilities::randomInUnitSphere() * hitObject->fuzz;
    Ray reflectedRay(hitPoint + reflectedDir * 0.001f, reflectedDir.normalized());

    // Recursively trace
    Color reflectedColor = traceRay(reflectedRay, scene, lights, ambientLight, depth + 1);

    // Mix reflection with local color (reflectivity controls contribution)
    float reflectivity = hitObject->reflectivity;
    pixelColor = pixelColor * (1 - reflectivity) + reflectedColor * reflectivity;

    return pixelColor;
}
Color tracePixel(int x, int y, vector<unique_ptr<Hittable>>& scene, Camera camera, vector<Light> lights, Light ambientLight, int width, int height) {
    Color accumulatedColor(0, 0, 0);

    for (int s = 0; s < SAMPLES_PER_PIXEL; s++) {
        // Random offsets from -1 to 1
        float offsetX = Utilities::randomFloat();  
        float offsetY = Utilities::randomFloat();

        // UV coord
        float u = (x + offsetX) / width;
        float v = 1 - (y + offsetY) / height;

        Ray ray = camera.getRay(u, v);

        // Trace the ray
        Color sampleColor = traceRay(ray, scene, lights, ambientLight);

        accumulatedColor += sampleColor;
    }

    // Average the samples
    return accumulatedColor / SAMPLES_PER_PIXEL;
}


int main() {
    auto startTime = high_resolution_clock::now();

    // Settings
    ImageQuality quality = ImageQuality::High;
    int fov = 70;
    int imageWidth = (int)quality;
    float aspectRatio = 16.0f / 9.0f;
    int imageHeight = static_cast<int>(imageWidth / aspectRatio);

    // Print
    cout << "Settings:\n" << "  FOV: " << fov << "\n  Width: " << imageWidth << "\n  Height: " << imageHeight << "\n  Anti-aliasing: " << SAMPLES_PER_PIXEL << "\n  Depth: " << MAX_DEPTH << endl;

    // Lighting
    Light ambientLight = { Vector3(), Color(1, 1, 1), .1f };
    // NOTE Normalize light directions for correct calculations
    vector<Light> lights = {
        { Vector3(0, 5, 0), Color(0, 1, 1), 15.0f },
        { Vector3(-2, 2, 1), Color(1, 0, 1), 10.0f },
    };

    // Camera
    Vector3 from = Vector3(0, 0, 5);
    Vector3 to = Vector3();
    Camera camera(from, to, Vector3(0, 1, 0), fov, aspectRatio);

    // Objects
    vector<unique_ptr<Hittable>> scene;
    scene.push_back(make_unique<Sphere>(Vector3(0, 0, -1), 2, Color(0, 1, 0), .5f, 0.1f));
    scene.push_back(make_unique<Sphere>(Vector3(3, 0, 0), 1, Color(0, 0, 1), .3f, 0.0f));
    scene.push_back(make_unique<Sphere>(Vector3(0, -102, 0), 100, Color(1, 1, 1), 0, 0.0f));

    // Output
    vector<unsigned char> pixels(imageWidth * imageHeight * 4);

    // Ray tracing
    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {   
            Color color = tracePixel(x, y, scene, camera, lights, ambientLight, imageWidth, imageHeight).corrected().byteColorFormat();
            int i = 4 * (y * imageWidth + x);
            pixels[i] = color.r;
            pixels[i + 1] = color.g;
            pixels[i + 2] = color.b;
            pixels[i + 3] = 255;
        }

        // Progress bar
        if (y % (imageHeight / 33) == 0 || y == imageHeight - 1) {
            float fraction = float(y) / (imageHeight - 1);
            int percentage = round(fraction * 100);
            string bar = "";
            for (int i = 0; i < 33; i++) {
                bar += i < percentage / 3 ? "▮" : "▯";
            }
            cout << "\rProgress: " << percentage << "% " << bar << flush;
        }
    }
    

    // Print render stats
    auto endTime = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(endTime - startTime).count();
    cout << "\nCompleted render in " << duration << " s.\nWriting to file..." << endl;

    // Write to file
    startTime = high_resolution_clock::now();
    lodepng::encode("output.png", pixels, imageWidth, imageHeight);

    // Print file stats
    endTime = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(endTime - startTime).count();
    cout << "Completed write in " << duration << " ms." << endl;

    ShellExecuteA(nullptr, "open", "output.png", nullptr, nullptr, SW_SHOWNORMAL);

    return 0;
}