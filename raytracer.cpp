#include <iostream>
#include <vector>

#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "raytracer.h"

using namespace std;

int main() {
    
    // IMAGE
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    // CAMERA
    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0;

    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin
                            - (horizontal / 2)
                            - (vertical / 2)
                            - vec3(0, 0, focal_length);

    // RENDER
    cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height ; j >= 0 ; --j) {
        cerr << "\rScanlines remaining: " << j << ' ' << flush;         // Progress Indicator
        for (int i = 0 ; i < image_width ; ++i) {
            // throughout the loop... image_width / -1 < u < image_width / 1
            auto u = double(i) / (image_width - 1);
            auto v = double(j) / (image_height - 1);
            ray r = ray(origin, lower_left_corner + (u * horizontal) + (v * vertical) - origin);
            color pixel_color = ray_color(r);
            write_color(cout, pixel_color);
        }
    }
    cerr << "\nDone.\n";
}